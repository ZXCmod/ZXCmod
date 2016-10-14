/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "effects.h"
#include "gamerules.h"
#include "tripmine.h"
#include "decals.h"

#define	TRIPMINE_PRIMARY_VOLUME		450


enum tripmine_e {
	TRIPMINE_IDLE1 = 0,
	TRIPMINE_IDLE2,
	TRIPMINE_ARM1,
	TRIPMINE_ARM2,
	TRIPMINE_FIDGET,
	TRIPMINE_HOLSTER,
	TRIPMINE_DRAW,
	TRIPMINE_WORLD,
	TRIPMINE_GROUND,
};




void CTripmineGrenade :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	SET_MODEL(ENT(pev), "models/v_tripmine.mdl");
	pev->frame = 0;
	pev->body = 3;
	pev->sequence = TRIPMINE_WORLD;
	// ResetSequenceInfo( );
	pev->framerate = 0;
	
	UTIL_SetSize(pev, Vector( -8, -8, -8), Vector(8, 8, 8));
	UTIL_SetOrigin( pev, pev->origin );


	// power up in 2.5 seconds
	m_flPowerUp = gpGlobals->time + 2.5;

	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 0.2;

	pev->takedamage = DAMAGE_YES;
	pev->dmg = 150;
	pev->health = 5; // don't let die normally

	if (pev->owner != NULL)
	{
		// play deploy sound
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM );
		EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM ); // chargeup

		m_pRealOwner = pev->owner;// see CTripmineGrenade for why.
	}

	UTIL_MakeAimVectors( pev->angles );

	m_vecDir = gpGlobals->v_forward;
	m_vecEnd = pev->origin + m_vecDir * 2048;
}


void CTripmineGrenade :: Precache( void )
{
	PRECACHE_MODEL("models/v_tripmine.mdl");
	PRECACHE_SOUND("weapons/mine_deploy.wav");
	PRECACHE_SOUND("weapons/mine_activate.wav");
	PRECACHE_SOUND("weapons/mine_charge.wav");
	PRECACHE_SOUND ("zxc/Fon12m.wav");
	
}


void CTripmineGrenade :: WarningThink( void  )
{
	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 1.0;
}


void CTripmineGrenade :: PowerupThink( void  )
{
	TraceResult tr;

	if (m_hOwner == NULL)
	{
		// find an owner
		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 32, dont_ignore_monsters, ENT( pev ), &tr );
		if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if (tr.flFraction < 1.0)
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance( pev->owner );
			m_posOwner = m_hOwner->pev->origin;
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
			SetThink( SUB_Remove );
			pev->nextthink = gpGlobals->time + 0.1;
			KillBeam();
			return;
		}
	}
	else if (m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{
		// disable
		STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
		STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
		CBaseEntity *pMine = Create( "weapon_tripmine", pev->origin + m_vecDir * 24, pev->angles );
		pMine->pev->spawnflags |= SF_NORESPAWN;

		SetThink( SUB_Remove );
		KillBeam();
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}

	if (gpGlobals->time > m_flPowerUp)
	{
		// make solid
		pev->solid = SOLID_BBOX;
		UTIL_SetOrigin( pev, pev->origin );

		MakeBeam( );

		// play enabled sound
        EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1.0, 75 );
	}
	pev->nextthink = gpGlobals->time + 0.1;
}


void CTripmineGrenade :: KillBeam( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
}


void CTripmineGrenade :: MakeBeam( void )
{
	TraceResult tr;

	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

	m_flBeamLength = tr.flFraction;

	// set to follow laser spot
	SetThink( BeamBreakThink );
	pev->nextthink = gpGlobals->time + 0.1;

	Vector vecTmpEnd = pev->origin + m_vecDir * 2048 * m_flBeamLength;

	m_pBeam = CBeam::BeamCreate( g_pModelNameLaser, 10 );
	m_pBeam->PointEntInit( vecTmpEnd, entindex() );
	m_pBeam->SetColor( 255, 128, 128 );
	m_pBeam->SetScrollRate( 128 );
	m_pBeam->SetBrightness( 50 );
}


void CTripmineGrenade :: BeamBreakThink( void  )
{
	BOOL bBlowup = 0;

	TraceResult tr;

	// HACKHACK Set simple box using this really nice global!
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

	// respawn detect. 
	if ( !m_pBeam )
	{
		MakeBeam( );
		if ( tr.pHit )
			m_hOwner = CBaseEntity::Instance( tr.pHit );	// reset owner too
	}

	if (fabs( m_flBeamLength - tr.flFraction ) > 0.001)
	{
		bBlowup = 1;
	}
	else
	{
		if (m_hOwner == NULL)
			bBlowup = 1;
		else if (m_posOwner != m_hOwner->pev->origin)
			bBlowup = 1;
		else if (m_angleOwner != m_hOwner->pev->angles)
			bBlowup = 1;
	}

	if (bBlowup)
	{
		// a bit of a hack, but all CGrenade code passes pev->owner along to make sure the proper player gets credit for the kill
		// so we have to restore pev->owner from pRealOwner, because an entity's tracelines don't strike it's pev->owner which meant
		// that a player couldn't trigger his own tripmine. Now that the mine is exploding, it's safe the restore the owner so the 
		// CGrenade code knows who the explosive really belongs to.
		pev->owner = m_pRealOwner;
		pev->health = 0;
		Killed( VARS( pev->owner ), GIB_NORMAL );
		// return;
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

int CTripmineGrenade :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if (gpGlobals->time < m_flPowerUp && flDamage < pev->health)
	{
		// SetThink( SUB_Remove );
		pev->takedamage = DAMAGE_NO;
		pev->nextthink = gpGlobals->time + 0.1;
		KillBeam();
		UTIL_Remove( this );
		return FALSE;
	}
	return CGrenade::TakeDamage( pevInflictor, pevAttacker, pev->dmg, bitsDamageType );
}

void CTripmineGrenade::Killed( entvars_t *pevAttacker, int iGib )
{
	
	
	if ( pevAttacker && ( pevAttacker->flags & FL_CLIENT ) )
	{
		// some client has destroyed this mine, he'll get credit for any kills
		pev->owner = ENT( pevAttacker );
	}

	SetThink( DelayDeathThink );
	pev->nextthink = gpGlobals->time + 0.1;

	EMIT_SOUND( ENT(pev), CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM ); // shut off chargeup
}


void CTripmineGrenade::DelayDeathThink( void )
{
	KillBeam();
	TraceResult tr;
	UTIL_TraceLine ( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 64,  dont_ignore_monsters, ENT(pev), & tr);

	Explode( &tr, DMG_CRUSH );
}


LINK_ENTITY_TO_CLASS( weapon_tripmine, CTripmine );

void CTripmine::Spawn( )
{
	Precache( );
	m_iId = WEAPON_TRIPMINE;
	SET_MODEL(ENT(pev), "models/v_tripmine.mdl");
	pev->frame = 0;
	pev->body = 3;
	pev->sequence = TRIPMINE_GROUND;
	pev->framerate = 0;
	
	

	FallInit();// get ready to fall down

	m_iDefaultAmmo = TRIPMINE_DEFAULT_GIVE;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 28) ); 

}

void CTripmine::Precache( void )
{
	PRECACHE_MODEL ("models/v_tripmine.mdl");
	PRECACHE_MODEL ("models/p_tripmine.mdl");
	UTIL_PrecacheOther( "monster_tripmine" );
	g_sModelIndexLaser = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	PRECACHE_MODEL( "sprites/laserbeam.spr" );

	m_usTripFire = PRECACHE_EVENT( 1, "events/tripfire.sc" );
}

int CTripmine::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Trip Mine";
	p->iMaxAmmo1 = TRIPMINE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_TRIPMINE;
	p->iWeight = TRIPMINE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}

BOOL CTripmine::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 280 );
	return DefaultDeploy( "models/v_tripmine.mdl", "models/p_tripmine.mdl", TRIPMINE_DRAW, "trip" );
}


void CTripmine::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		// out of mines
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_TRIPMINE);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	SendWeaponAnim( TRIPMINE_HOLSTER );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CTripmine::FourthAttack( void )
{
	CBaseEntity *pEntity;
	TraceResult	tr;	
	Vector vecSrc;
	vecSrc = m_pPlayer->GetGunPosition( );
	Vector vecDir = gpGlobals->v_forward;
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 256, dont_ignore_monsters, ENT(pev), &tr);
	pEntity = CBaseEntity::Instance(tr.pHit); //trace hit to entity
	int m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
	int m_iTrail2 = PRECACHE_MODEL("sprites/blueflare2.spr");
	UTIL_Sparks( tr.vecEndPos );
	
	if ( pEntity != NULL )
		{
		if (pEntity->pev->sequence == TRIPMINE_WORLD)
			{
				{
				if (pEntity->pev->ltime == 0)
					{
					pEntity->pev->ltime = 1;
					pEntity->pev->health += 50;
					}
				else
					{
					pEntity->pev->ltime = 0;
					pEntity->pev->health -= 50;
					}
				}
			}
			// like a monster ally? Make him friend xD 
			else if (pEntity->pev->flags & (FL_MONSTER) && (pEntity->pev->takedamage != DAMAGE_AIM))
			{
				if (pEntity->pev->owner != m_pPlayer->edict()) // make from a monster ally
				{
					pEntity->pev->owner = m_pPlayer->edict();
					pEntity->Classify2 = CLASS_MACHINE;
					
					// torus
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_BEAMTORUS );
						WRITE_COORD( pEntity->pev->origin.x);
						WRITE_COORD( pEntity->pev->origin.y);
						WRITE_COORD( pEntity->pev->origin.z); 
						WRITE_COORD( pEntity->pev->origin.x);
						WRITE_COORD( pEntity->pev->origin.y);
						WRITE_COORD( pEntity->pev->origin.z - 700 ); // reach damage radius over .2 seconds
						WRITE_SHORT( m_iTrail );
						WRITE_BYTE( 0 ); // startframe
						WRITE_BYTE( 8 ); // framerate
						WRITE_BYTE( 4 ); // life
						WRITE_BYTE( 128 );  // width
						WRITE_BYTE( 0 );   // noise
						WRITE_BYTE( 155 );   // r, g, b
						WRITE_BYTE( 155 );   // r, g, b
						WRITE_BYTE( 200 );   // r, g, b
						WRITE_BYTE( 200 ); // brightness
						WRITE_BYTE( 0 );		// speed
					MESSAGE_END();
					
					EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Fon12m.wav", 1.0, ATTN_STATIC);
					EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/Fon12m.wav", 0.75, ATTN_STATIC);
				}
				else // return to inital relationships
				{	
					pEntity->Classify2 = pEntity->Classify();
					pEntity->pev->owner = pEntity->pevCreateTemp; // dinamical restore edict
					// see a monsters.cpp and monstermaker.cpp
				}
				
			}
			
			// capture the nukemissile for create Red Crystal ( >1.35 )
			if (FClassnameIs( pEntity->pev, "nuke"))
			{
				// balls
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
					WRITE_COORD( pEntity->pev->origin.x );
					WRITE_COORD( pEntity->pev->origin.y );
					WRITE_COORD( pEntity->pev->origin.z );
					WRITE_COORD( pEntity->pev->origin.x + tr.vecPlaneNormal.x*2 );
					WRITE_COORD( pEntity->pev->origin.y  + tr.vecPlaneNormal.y*3 );
					WRITE_COORD( pEntity->pev->origin.z + tr.vecPlaneNormal.z*16  );
					WRITE_SHORT( m_iTrail2 );		// model
					WRITE_BYTE( 42  );				// count
					WRITE_BYTE( 12 );				// life * 10
					WRITE_BYTE( 3 );				// size * 10
					WRITE_BYTE( 24 );				// amplitude * 0.1
					WRITE_BYTE( 4 );				// speed * 100
				MESSAGE_END();

				Create( "weapon_bola", pEntity->pev->origin + gpGlobals->v_up * 16, Vector(0,0,0), pev->owner ); 
				m_pPlayer->m_flNextChatTime3 = gpGlobals->time + 45; 
				pEntity->SUB_Remove();
			}
			
			// capture the DCrystal for create White FCrystal ( >1.36 )
			if (FClassnameIs( pEntity->pev, "weapon_aicore"))
			{
				// balls
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
					WRITE_COORD( pEntity->pev->origin.x );
					WRITE_COORD( pEntity->pev->origin.y );
					WRITE_COORD( pEntity->pev->origin.z );
					WRITE_COORD( pEntity->pev->origin.x + tr.vecPlaneNormal.x*2 );
					WRITE_COORD( pEntity->pev->origin.y  + tr.vecPlaneNormal.y*3 );
					WRITE_COORD( pEntity->pev->origin.z + tr.vecPlaneNormal.z*16  );
					WRITE_SHORT( m_iTrail2 );		// model
					WRITE_BYTE( 30  );				// count
					WRITE_BYTE( 12 );				// life * 10
					WRITE_BYTE( 3 );				// size * 10
					WRITE_BYTE( 24 );				// amplitude * 0.1
					WRITE_BYTE( 4 );				// speed * 100
				MESSAGE_END();

				Create( "zone_shelter", pEntity->pev->origin + gpGlobals->v_up * 16, Vector(0,0,0), pev->owner ); 
				pEntity->SUB_Remove();
			}
			
			// capture the satchel for lauch a tripmine rocket ( >1.36 )
			if (FClassnameIs( pEntity->pev, "monster_satchel"))
			{
				// balls
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
					WRITE_COORD( pEntity->pev->origin.x );
					WRITE_COORD( pEntity->pev->origin.y );
					WRITE_COORD( pEntity->pev->origin.z );
					WRITE_COORD( pEntity->pev->origin.x + tr.vecPlaneNormal.x*2 );
					WRITE_COORD( pEntity->pev->origin.y  + tr.vecPlaneNormal.y*3 );
					WRITE_COORD( pEntity->pev->origin.z + tr.vecPlaneNormal.z*16  );
					WRITE_SHORT( m_iTrail2 );		// model
					WRITE_BYTE( 30  );				// count
					WRITE_BYTE( 12 );				// life * 10
					WRITE_BYTE( 3 );				// size * 10
					WRITE_BYTE( 24 );				// amplitude * 0.1
					WRITE_BYTE( 4 );				// speed * 100
				MESSAGE_END();
				Create( "info_airstrike_node", pEntity->pev->origin + gpGlobals->v_up * 16, Vector(80,m_pPlayer->pev->v_angle.y,0), m_pPlayer->edict() ); 
				m_pPlayer->m_flNextChatTime4--;
				pEntity->SUB_Remove();
			}

		}
	// ray
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD( tr.vecEndPos.x ); //tr.vecEndPos.
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_SHORT( m_iTrail ); //sprite
		WRITE_BYTE( 0 ); // Starting frame
		WRITE_BYTE( 0  ); // framerate * 0.1
		WRITE_BYTE( 3 ); // life * 0.1
		WRITE_BYTE( 12 ); // width
		WRITE_BYTE( 8 ); // noise
		WRITE_BYTE( 100 ); // color r,g,b
		WRITE_BYTE( 65 ); // color r,g,b
		WRITE_BYTE( 10 ); // color r,g,b
		WRITE_BYTE( 100 ); // brightness
		WRITE_BYTE( 100 ); // scroll speed
	MESSAGE_END();

	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build2.wav", 0.90, ATTN_NORM, 0, 175);
	
				
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	
}

void CTripmine::PrimaryAttack( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
		return;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usTripFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

	if (tr.flFraction < 1.0)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if ( pEntity && !(pEntity->pev->flags & FL_CONVEYOR) )
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			CBaseEntity *pEnt = CBaseEntity::Create( "monster_tripmine", tr.vecEndPos + tr.vecPlaneNormal * 8, angles, m_pPlayer->edict() );

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=1;

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			
			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			{
				// no more mines! 
				RetireWeapon();
				return;
			}
		}
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;

}



void CTripmine::SecondaryAttack( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 1)
		return;
	if (allowmonsters9.value == 0)
		return;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usTripFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
	
	if (tr.flFraction < 1.0)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if ( pEntity && !(pEntity->pev->flags & FL_CONVEYOR) )
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			CBaseEntity *pEnt = CBaseEntity::Create( "monster_replicateur", tr.vecEndPos + tr.vecPlaneNormal * 8, angles, m_pPlayer->edict() );

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=2;

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			
			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			{
				// no more mines! 
				RetireWeapon();
				return;
			}
		}
	}
	
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 7.0;
}



void CTripmine::ThirdAttack( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
		return;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	TraceResult tr;

	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 128, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usTripFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
	
	if (tr.flFraction < 1.0)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
		if ( pEntity && !(pEntity->pev->flags & FL_CONVEYOR) )
		{
			Vector angles = UTIL_VecToAngles( tr.vecPlaneNormal );

			CBaseEntity *pEnt = CBaseEntity::Create( "weapon_energy", tr.vecEndPos + tr.vecPlaneNormal * 8, angles, m_pPlayer->edict() );

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=1;

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			
		}
	}
	
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;

}


void CTripmine::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 )
	{
		SendWeaponAnim( TRIPMINE_DRAW );
	}
	else
	{
		RetireWeapon(); 
		return;
	}

	SendWeaponAnim( TRIPMINE_IDLE1 );
}

void CTripmineGrenade2 :: Spawn( void )
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
    pev->classname = MAKE_STRING( "weapon_tripmine" );
	SET_MODEL(ENT(pev), "models/v_tripmine.mdl");
	pev->frame = 0;
	pev->ltime = 0;
	pev->body = 3;
	pev->sequence = TRIPMINE_WORLD;
	ResetSequenceInfo( );
	pev->framerate = 0;
	pev->flags |= FL_NOTARGET;
	
	UTIL_SetSize(pev, Vector( -8, -8, -8), Vector(8, 8, 8));
	UTIL_SetOrigin( pev, pev->origin );

	m_flPowerUp = gpGlobals->time + 1.5;

	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 0.2;

	pev->takedamage = DAMAGE_YES;
	pev->dmg = 25;
	pev->health = 10;
	pev->max_health = pev->health; 
	

	if (pev->owner != NULL)
	{
		// play deploy sound
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM );
		EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM ); // chargeup

		m_pRealOwner = pev->owner;// see CTripmineGrenade for why.
	}

	UTIL_MakeAimVectors( pev->angles );

	
	m_vecDir = gpGlobals->v_forward;
	vec_tmp = m_vecDir;
	m_vecEnd = pev->origin + m_vecDir * 2048;
	m_flNextChatTime5 = gpGlobals->time+150; //start timer

}


void CTripmineGrenade2 :: Precache( void )
{

}


void CTripmineGrenade2 :: WarningThink( void  )
{

	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 1.0;
	
}


void CTripmineGrenade2 :: PowerupThink( void  )
{

	CBaseEntity *pEntity = CBaseEntity::Instance(pev->owner);
	CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
	
	CBaseEntity *pList[2]; // check two any entity
	Vector delta = Vector( 1000, 1000, 1000 ); // check size
	Vector mins = pev->origin - delta;
	Vector maxs = pev->origin + delta;
	
	int count = UTIL_EntitiesInBox( pList, 2, mins, maxs, (FL_NOTARGET) );
	
	if ( count ) // check another laser mines for destroy
	{
		for ( int i = 0; i < count; i++ )
		{
			// only clients and monsters
			if (  pList[i] != this && (pList[i]->pev->owner == pPlayer->edict() )) // pList[i] != this ||
			{
				SetThink( DelayDeathThink );
				pev->nextthink = gpGlobals->time + 0.25;
			}
		}
	}
	
	TraceResult tr;
	if (m_hOwner == NULL)
	{


		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 32, dont_ignore_monsters, ENT( pev ), &tr );
		if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if (tr.flFraction < 1.0)
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance( pev->owner );
			m_posOwner = m_hOwner->pev->origin;
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
			SetThink( SUB_Remove );
			pev->nextthink = gpGlobals->time + 0.1;
			KillBeam();
			return;
		}
	}
	
	else if (m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{

		// disable
		STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
		STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
		CBaseEntity *pMine = Create( "weapon_tripmine", pev->origin + m_vecDir * 24, pev->angles );
		pMine->pev->spawnflags |= SF_NORESPAWN;

		SetThink( SUB_Remove );
		KillBeam();
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}

	if (gpGlobals->time > m_flPowerUp)
	{
	
		// make solid
		pev->solid = SOLID_BBOX;
		UTIL_SetOrigin( pev, pev->origin );

		SetThink(MakeBeam);
		pev->nextthink = gpGlobals->time + 0.2;

		// play enabled sound
        EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1.0, 75 );
		
	}
	pev->nextthink = gpGlobals->time + 0.1;
}


void CTripmineGrenade2 :: KillBeam( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
}


void CTripmineGrenade2 :: MakeBeam( void )
{
	TraceResult tr;
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	
	short m_LaserSprite = PRECACHE_MODEL( "sprites/xenobeam.spr" );
	
	CBaseEntity *pEntity = NULL;
	
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	
	m_vecEnd = pev->origin + m_vecDir * 2048;
	pev->owner = m_pRealOwner;

	Vector vecTmpEnd = pev->origin + m_vecDir * 2048 * m_flBeamLength;
	
	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT(pev), &tr ); // dont leave player
	m_flBeamLength = tr.flFraction;

	if (pev->ltime == 0)
		{
		m_vecDir = vec_tmp;
		if ( (Instance( tr.pHit )->pev->takedamage) ) 
			{
			::RadiusDamage( tr.vecEndPos, pev, VARS( pev->owner ), pev->dmg, 20, CLASS_NONE, DMG_BULLET  ); //end blast
			UTIL_DecalTrace( &tr, DECAL_GUNSHOT1 );
			}
		}
	else if (pev->ltime == 1)
		{
			while ((pEntity = UTIL_FindEntityByClassname( pEntity, "player" )) != NULL)
			{

			if (pEntity->edict() == pev->owner && (pEntity->edict() != edict()) && ( pEntity->pev->health>1)) // (FVisible( pEntity )) &&
				{

					m_vecDir = (pEntity->EyePosition()-Vector(0,0,20)) - pev->origin;
					if ( (Instance( tr.pHit )->pev->takedamage) ) 
					{
						::RadiusDamage( tr.vecEndPos, pev, VARS( pev->owner ), pev->dmg*2, 20, CLASS_NONE, DMG_BULLET  ); //end blast
						UTIL_DecalTrace( &tr, DECAL_GUNSHOT1 );
					}
				}

			}
			
		}
	// trace laser 
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD( vecTmpEnd.x );
		WRITE_COORD( vecTmpEnd.y );
		WRITE_COORD( vecTmpEnd.z );
		WRITE_SHORT( m_LaserSprite );
		WRITE_BYTE( 0 ); // Starting frame
		WRITE_BYTE( 0  ); // framerate * 0.1
		WRITE_BYTE( 1 ); // life * 0.1
		WRITE_BYTE( 12 ); // width
		WRITE_BYTE( 0 ); // noise
		WRITE_BYTE( 80 ); // color r,g,b
		WRITE_BYTE( 0 ); // color r,g,b
		WRITE_BYTE( 0 ); // color r,g,b
		WRITE_BYTE( 150 ); // brightness
		WRITE_BYTE( 0 ); // scroll speed
	MESSAGE_END();
	
	pev->nextthink = gpGlobals->time + 0.04; //delay redraw

	if ( m_flNextChatTime5 < gpGlobals->time )
	{
		SetThink( SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.2;
		KillBeam();
		return;
	}

}


void CTripmineGrenade2 :: BeamBreakThink( void  )
{

}

int CTripmineGrenade2 :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if (gpGlobals->time < m_flPowerUp && flDamage < pev->health)
	{
		SetThink( SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
		KillBeam();
		return FALSE;
	}
	return CGrenade::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CTripmineGrenade2::Killed( entvars_t *pevAttacker, int iGib )
{

	pev->takedamage = DAMAGE_NO;
	
	if ( pevAttacker && ( pevAttacker->flags & FL_CLIENT ) )
		pev->owner = ENT( pevAttacker );

	SetThink( DelayDeathThink );
	pev->nextthink = gpGlobals->time + 0.25;

	EMIT_SOUND( ENT(pev), CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM ); // shut off chargeup
}


void CTripmineGrenade2::DelayDeathThink( void )
{
	KillBeam();
	TraceResult tr;
	UTIL_TraceLine ( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 64,  dont_ignore_monsters, ENT(pev), & tr);
	Explode( &tr, DMG_CRUSH );
}

void CTripmineGrenade3 :: Spawn( void )
{
	Precache( );

	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	SET_MODEL(ENT(pev), "models/v_tripmine.mdl");
	pev->frame = 0;
	pev->body = 3;
	pev->sequence = TRIPMINE_WORLD;
	ResetSequenceInfo( );
	pev->framerate = 0;
	
	UTIL_SetSize(pev, Vector( -8, -8, -8), Vector(8, 8, 8));
	UTIL_SetOrigin( pev, pev->origin );
	
	m_flPowerUp = gpGlobals->time + 1.5;

	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 0.2;

	pev->takedamage = DAMAGE_YES;
	pev->dmg = 20;
	pev->health = 30; 
	pev->max_health = pev->health;

	if (pev->owner != NULL)
	{
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", 1.0, ATTN_NORM );
		EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM ); // chargeup
		m_pRealOwner = pev->owner;// see CTripmineGrenade for why.
	}

	UTIL_MakeAimVectors( pev->angles );

	m_vecDir = gpGlobals->v_forward;
	m_vecEnd = pev->origin + m_vecDir * 2048;
}


void CTripmineGrenade3 :: Precache( void )
{
	m_LaserSprite = PRECACHE_MODEL( "sprites/xenobeam.spr" );
}


void CTripmineGrenade3 :: WarningThink( void  )
{
	SetThink( PowerupThink );
	pev->nextthink = gpGlobals->time + 1.0;
}


void CTripmineGrenade3 :: PowerupThink( void  )
{
	TraceResult tr;

	if (m_hOwner == NULL)
	{
		// find an owner
		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 32, dont_ignore_monsters, ENT( pev ), &tr );
		if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if (tr.flFraction < 1.0)
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance( pev->owner );
			m_posOwner = m_hOwner->pev->origin;
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
			STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
			SetThink( SUB_Remove );
			pev->nextthink = gpGlobals->time + 0.1;
			ALERT( at_console, "WARNING:Tripmine at %.0f, %.0f, %.0f removed\n", pev->origin.x, pev->origin.y, pev->origin.z );
			KillBeam();
			return;
		}
	}
	else if (m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{
		// disable
		STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
		STOP_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_charge.wav" );
		CBaseEntity *pMine = Create( "weapon_tripmine", pev->origin + m_vecDir * 24, pev->angles );
		pMine->pev->spawnflags |= SF_NORESPAWN;

		SetThink( SUB_Remove );
		KillBeam();
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}

	if (gpGlobals->time > m_flPowerUp)
	{
		// make solid
		pev->solid = SOLID_BBOX;
		UTIL_SetOrigin( pev, pev->origin );

		MakeBeam( );

		// play enabled sound
        EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "weapons/mine_activate.wav", 0.5, ATTN_NORM, 1.0, 75 );
		SetThink( HealThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}
	pev->nextthink = gpGlobals->time + 0.1;
}



void CTripmineGrenade3 :: KillBeam( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
}


void CTripmineGrenade3 :: MakeBeam( void )
{

}

void CTripmineGrenade3 :: HealThink( void  )
{
	CBaseEntity *pEntity;
	TraceResult tr;
	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );
	m_flBeamLength = tr.flFraction;
	Vector vecTmpEnd = pev->origin + m_vecDir * 2048 * m_flBeamLength;
	pEntity = CBaseEntity::Instance(tr.pHit); //trace hit to entity
	
	// trace heal laser 
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD( vecTmpEnd.x );
		WRITE_COORD( vecTmpEnd.y );
		WRITE_COORD( vecTmpEnd.z );
		WRITE_SHORT( m_LaserSprite );
		WRITE_BYTE( 0 ); // Starting frame
		WRITE_BYTE( 0  ); // framerate * 0.1
		WRITE_BYTE( 3 ); // life * 0.1
		WRITE_BYTE( 12 ); // width
		WRITE_BYTE( 0 ); // noise
		WRITE_BYTE( 0 ); // color r,g,b
		WRITE_BYTE( 80 ); // color r,g,b
		WRITE_BYTE( 0 ); // color r,g,b
		WRITE_BYTE( 150 ); // brightness
		WRITE_BYTE( 0 ); // scroll speed
	MESSAGE_END();
	
	if (pEntity != NULL && (pEntity->pev->flags & (FL_CLIENT | FL_MONSTER)) ) //  && pEntity->IsPlayer()
		{
		// recover limit health
		if (pEntity->pev->max_health <= 98)
			pEntity->pev->max_health += 2;
		
		//heal anyone
		if (pEntity->pev->health <= pEntity->pev->max_health )
			pEntity->TakeHealth(2, DMG_PARALYZE);
	
		}
		
	pev->nextthink = gpGlobals->time + 0.2;
}


void CTripmineGrenade3 :: BeamBreakThink( void  )
{
	BOOL bBlowup = 0;

	TraceResult tr;

	// HACKHACK Set simple box using this really nice global!
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine( pev->origin, m_vecEnd, dont_ignore_monsters, ENT( pev ), &tr );


	// respawn detect. 
	if ( !m_pBeam )
	{
		MakeBeam( );
		if ( tr.pHit )
			m_hOwner = CBaseEntity::Instance( tr.pHit );	// reset owner too
	}

	if (fabs( m_flBeamLength - tr.flFraction ) > 0.001)
	{
		bBlowup = 1;
	}
	else
	{
		if (m_hOwner == NULL)
			bBlowup = 1;
		else if (m_posOwner != m_hOwner->pev->origin)
			bBlowup = 1;
		else if (m_angleOwner != m_hOwner->pev->angles)
			bBlowup = 1;
	}

	if (bBlowup)
	{
		pev->owner = m_pRealOwner;
		pev->health = 0;
		Killed( VARS( pev->owner ), GIB_NORMAL );
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

int CTripmineGrenade3 :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if (gpGlobals->time < m_flPowerUp && flDamage < pev->health)
	{
		SetThink( SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
		KillBeam();
		return FALSE;
	}
	return CGrenade::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CTripmineGrenade3::Killed( entvars_t *pevAttacker, int iGib )
{


	pev->takedamage = DAMAGE_NO;
	
	if ( pevAttacker && ( pevAttacker->flags & FL_CLIENT ) )
	{
		// some client has destroyed this mine, he'll get credit for any kills
		pev->owner = ENT( pevAttacker );
	}

	SetThink( DelayDeathThink );
	pev->nextthink = gpGlobals->time + 0.2;

	EMIT_SOUND( ENT(pev), CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM ); // shut off chargeup
}


void CTripmineGrenade3::DelayDeathThink( void )
{
	CBaseEntity *pEntity = NULL;
	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 250 )) != NULL)
		{
		if (pEntity->IsPlayer() && FVisible( pEntity ))
			{	
				pEntity->pev->friction = 0.07;
				pEntity->pev->armorvalue = (pEntity->pev->armorvalue * .50);
				pEntity->pev->max_health = (pEntity->pev->max_health * .90);
			}
		}
		
	KillBeam();
	TraceResult tr;
	UTIL_TraceLine ( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 64,  dont_ignore_monsters, ENT(pev), & tr);
	Explode( &tr, DMG_CRUSH );
}



