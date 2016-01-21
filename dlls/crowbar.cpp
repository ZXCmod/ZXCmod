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
#include "gamerules.h"
#include "soundent.h"
#include "shake.h"
#include "func_break.h"
#include "decals.h"
#include "crowbar.h"




enum gauss_e {
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW,
	CROWBAR_HOLSTER,
	CROWBAR_ATTACK1HIT,
	CROWBAR_ATTACK1MISS,
	CROWBAR_ATTACK2MISS,
	CROWBAR_ATTACK2HIT,
	CROWBAR_ATTACK3MISS,
	CROWBAR_ATTACK3HIT
};


void CCrowbar::Spawn( )
{
	Precache( );
	m_flNextChatTime11 = gpGlobals->time;
	m_iId = WEAPON_CROWBAR;
	SET_MODEL(ENT(pev), "models/w_crowbar.mdl");
	FallInit();// get ready to fall down.
	
}



void CCrowbar::Precache( void )
{
	PRECACHE_MODEL("models/v_crowbar.mdl");
	PRECACHE_MODEL("models/w_crowbar.mdl");
	PRECACHE_MODEL("models/p_crowbar.mdl");
	PRECACHE_SOUND("weapons/cbar_hit1.wav");
	PRECACHE_SOUND("weapons/cbar_hit2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod3.wav");
	PRECACHE_SOUND("weapons/cbar_miss1.wav");
	PRECACHE_MODEL( "sprites/shock.spr" );
	PRECACHE_SOUND("debris/beamstart7.wav");
	PRECACHE_MODEL( "sprites/blast.spr" );
	PRECACHE_MODEL( "sprites/cexplo.spr" );
	
	PRECACHE_MODEL( "models/rpgrocket.mdl" );
	PRECACHE_SOUND ("weapons/rocket1.wav");

	m_usCrowbar = PRECACHE_EVENT ( 1, "events/crowbar.sc" );
}

int CCrowbar::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = NULL;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP; //GLOCK_MAX_CLIP
	p->iSlot = 0;
	p->iPosition = 0;
	p->iId = WEAPON_CROWBAR;
	p->iWeight = CROWBAR_WEIGHT;
	return 1;
}

BOOL CCrowbar::Deploy( )
{
	if (allowmonsters10.value == 1)
		{
			g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 386 ); //304
		}
	else
		{
			g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 317 ); //304
		}
	return DefaultDeploy( "models/v_crowbar.mdl", "models/p_crowbar.mdl", CROWBAR_DRAW, "crowbar" );
}

void CCrowbar::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( CROWBAR_HOLSTER );
}


void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void CCrowbar::Reload( void )
{

}


void CCrowbar::PrimaryAttack()
{
	if (m_touch != 0)
		m_touch = 0;
		
	if (! Swing( 1 ))
	{
		SetThink( SwingAgain );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}


void CCrowbar::SecondaryAttack()
{ 
	if (allowmonsters3.value != 1)
		return;
	if (allowmonsters9.value == 0)
		return;
		
	if (m_touch != 0)
		m_touch = 0;
		
	if (allowmonsters10.value == 0)
		{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
		{
			if (! Swing( 1 ))
			{
				UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
				Vector GunPosition = m_pPlayer->GetGunPosition( );
				GunPosition = GunPosition + gpGlobals->v_forward * 0;
				GunPosition = GunPosition + gpGlobals->v_right   * 7;
				GunPosition = GunPosition + gpGlobals->v_up      * 0;
				CRC* Lr = CRC :: Create( GunPosition,
														 m_pPlayer->pev->v_angle,
														 m_pPlayer );


				Lr->pev->velocity = Lr->pev->velocity + gpGlobals->v_right;
				Lr->pev->velocity = Lr->pev->velocity + gpGlobals->v_up;
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=3;
				
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0; 
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
				SetThink( SwingAgain );
			}
		}
	}
	else
	{

		Vector GunPosition = m_pPlayer->GetGunPosition( );
		GunPosition = GunPosition + gpGlobals->v_forward * 0;
		GunPosition = GunPosition + gpGlobals->v_right   * 7;
		GunPosition = GunPosition + gpGlobals->v_up      * 0;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_pPlayer->m_flNextChatTime11 = gpGlobals->time + 2;
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecThrow = gpGlobals->v_forward;
		CBaseEntity::Create( "weapon_rocketlauncher", GunPosition, vecThrow, m_pPlayer->edict() );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.75;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.75;
		
		switch( RANDOM_LONG(0,2) )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
		}
	}
}



void CCrowbar::ThirdAttack()
{
	// new weapon: teleporter. Target is spawn points (info_deathmatch). Linked with subs.cpp 
	// 1.27
	if (allowmonsters8.value != 1)
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]<=0)
		return;

	entvars_t* pevToucher = m_pPlayer->pev; //player object
	edict_t	*pentTarget = NULL; //teleport target
	edict_t	*pentTarget2 = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(500) ); //teleport target
	index = g_pGameRules->GetTeamIndex( m_pPlayer->TeamID() ); //1.30 for teamplay
	

	// if first team (used coop ent)
	if ( g_pGameRules->IsTeamplay() && index == 0 )
	{

		
		for ( int i = RANDOM_LONG(1,16); i > 0; i-- )
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(500) ); //find team targetname at "500"

		if (FNullEnt(pentTarget2))
		{
		for ( int i = RANDOM_LONG(1,16); i > 0; i-- )
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(75) ); //find targetname at "75"
		}

	}
	
	// if second team
	if ( g_pGameRules->IsTeamplay() && index == 1 )
	{
	for ( int i = RANDOM_LONG(1,16); i > 0; i-- )
		pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(75) ); //find targetname at "75"
	}

	// only deathmatch
 	if ( !g_pGameRules->IsTeamplay() )
	{
	for ( int i = RANDOM_LONG(1,16); i > 0; i-- )
		pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(75) ); //find targetname at "75"
	}
	
	if (FNullEnt(pentTarget)) // not execute, if singleplayer (?)
	   return;	
	
	// vis effects #1
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_TELEPORT );
		WRITE_COORD( m_pPlayer->pev->origin.x );
		WRITE_COORD( m_pPlayer->pev->origin.y );
		WRITE_COORD( m_pPlayer->pev->origin.z );
	MESSAGE_END();
	
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "debris/beamstart7.wav", 0.9, ATTN_NORM); //play sound 1st
	
	Vector tmp = VARS( pentTarget )->origin; //teleport to the point

	tmp.z++;
	pevToucher->flags &= ~FL_ONGROUND;
	UTIL_SetOrigin( pevToucher, tmp );
	pevToucher->angles = pentTarget->v.angles;

	if ( m_pPlayer->IsPlayer() )
	{
		pevToucher->v_angle = pentTarget->v.angles;
	}

	pevToucher->velocity = pevToucher->basevelocity = g_vecZero;
	
    // delays and sound
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=10;
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "debris/beamstart7.wav", 0.8, ATTN_NORM); //play sound 2nd
	
	if (m_pPlayer->pev->friction != 1.0) // 1.31 3rd antifriction feature
		m_pPlayer->pev->friction = 1.0;
	if (m_pPlayer->PTime > 0) // reset paralize
		m_pPlayer->PTime = 0;
	
	// vis effects #2 in destin...
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_TELEPORT );
		WRITE_COORD( m_pPlayer->pev->origin.x );
		WRITE_COORD( m_pPlayer->pev->origin.y );
		WRITE_COORD( m_pPlayer->pev->origin.z );
	MESSAGE_END();
	
}


void CCrowbar::FourthAttack()
{
	if (m_touch != 1)
		m_touch = 1;

	if (! Swing( 1 ))
	{
		SetThink( SwingAgain );
		pev->nextthink = gpGlobals->time + 0.1;
	}


}




void CCrowbar::Smack( )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}


void CCrowbar::SwingAgain( void )
{
	Swing( 0 );
}

int CCrowbar::Swing( int fFirst )
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;
	Vector	vecDir;
	vecDir = Vector( 0, 0, 0 );

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );


	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( (!pHit || pHit->IsBSPModel()) )
				{
					FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
				}

				vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
				UTIL_Ricochet( tr.vecEndPos, 7 ); // FX
		}
	}


	PLAYBACK_EVENT_FULL( FEV_GLOBAL| FEV_GLOBAL, m_pPlayer->edict(), m_usCrowbar, 
	0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, 0,
	0.0, 0, 0.0 );


	if ( tr.flFraction >= 1.0 )
	{
		if (fFirst)
		{
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		switch( RANDOM_LONG(0,2) )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
		}
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage( );
		
		pev->dmg = RANDOM_LONG(20,30)*m_pPlayer->TripleShotS;

		if ( (m_flNextPrimaryAttack = m_flNextSecondaryAttack + 1 < UTIL_WeaponTimeBase() ) || g_pGameRules->IsMultiplayer() && allowmonsters3.value != 1 )
		{
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, pev->dmg*2, gpGlobals->v_forward, &tr, DMG_CRUSH ); 
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, pev->dmg, gpGlobals->v_forward, &tr, DMG_CRUSH ); 
		}	
		
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if ( pEntity->Classify( ) != CLASS_NONE && pEntity->Classify( ) != CLASS_MACHINE )
			{

				// play thwack or smack sound
				switch( RANDOM_LONG(0,2) )
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hitbod1.wav", 1, ATTN_NORM); break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hitbod2.wav", 1, ATTN_NORM); break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hitbod3.wav", 1, ATTN_NORM); break;
				}
				m_pPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;
				
				if ( !pEntity->IsAlive() )
					  return TRUE;
				else
					  flVol = 0.1;

				fHitWorld = FALSE;
				
				if (m_touch == 1)
					{
						pEntity->pev->velocity = gpGlobals->v_forward  * 600; //relative by player, well done 1.31
						m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
					}

			}
		}
		if (fHitWorld)
		{
			if (m_touch == 1)
				{
					m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 600;
					m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
					m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
					m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
				}
				
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd-vecSrc)*2, BULLET_PLAYER_CROWBAR);

			if ( g_pGameRules->IsMultiplayer() )
			{
				fvolbar = 1;
			}
			switch( RANDOM_LONG(0,1) )
			{
			case 0:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			}
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;

		// world hit
		if (m_touch == 0)
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
		
		SetThink( Smack );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;
	}
	return fDidHit;
}

void CCrowbar::WeaponIdle( void )
{

if (allowmonsters3.value != 1)
	return;
if (allowmonsters10.value == 1)
	return;
if (allowmonsters9.value == 0)
	return;

	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 6) 
	{
		if (  m_pPlayer->m_flNextChatTime11 < gpGlobals->time ) //need delay
		{
			Vector GunPosition = m_pPlayer->GetGunPosition( );
			GunPosition = GunPosition + gpGlobals->v_forward * 0;
			GunPosition = GunPosition + gpGlobals->v_right   * 7;
			GunPosition = GunPosition + gpGlobals->v_up      * 0;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextChatTime11 = gpGlobals->time + 2;
			m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
			m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			UTIL_MakeVectors( m_pPlayer->pev->v_angle );
			Vector vecThrow = gpGlobals->v_forward;
			CBaseEntity::Create( "weapon_rocketlauncher", GunPosition, vecThrow, m_pPlayer->edict() );

			// its fun prerelease 1.34
			if (allowmonsters15.value != 0)
			{
				CBaseEntity::Create( "weapon_rocketlauncher", GunPosition+Vector(-20,0,0), vecThrow, m_pPlayer->edict() );
				CBaseEntity::Create( "weapon_rocketlauncher", GunPosition+Vector(20,0,0), vecThrow, m_pPlayer->edict() );
			}

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 6;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.75; //1.5
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.75; //1.5
			
			if (m_pPlayer->pev->effects == EF_NODRAW) //reset values
			{
				m_pPlayer->pev->effects &= ~EF_NODRAW;
				UTIL_ScreenFade( m_pPlayer, Vector(100,0,0), 0.95, 0.75, 70, FFADE_IN );
				MESSAGE_BEGIN( MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev) );
					WRITE_STRING( "Invisible has reset." );
				MESSAGE_END();
			}
			
			InvReset(); // reset invis
			TripleReset(); // red crystal fix
			
			switch( RANDOM_LONG(0,2) )
			{
			case 0:
				SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
			case 1:
				SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
			case 2:
				SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
			}
		}
	}
}




void    CRC :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(8,1,1) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_crowbar" );
	SetThink( MoveThink );
	SetTouch( Hit );
	pev->velocity = gpGlobals->v_forward * BLASTER_BEAM_SPEED;
	pev->angles.x = -(pev->angles.x);
	pev->nextthink = gpGlobals->time + 0.1;
	pev->dmg = 90; //90 old
	pev->effects = EF_MUZZLEFLASH;
	
	MESSAGE_BEGIN           ( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE      ( TE_BEAMFOLLOW );
			WRITE_SHORT     ( entindex() );
			WRITE_SHORT     ( BeamSprite );
			WRITE_BYTE      ( 8 );
			WRITE_BYTE      ( 3 );
			WRITE_BYTE      ( 30 );
			WRITE_BYTE      ( 30 );
			WRITE_BYTE      ( 255 );
			WRITE_BYTE      ( 100 );
	MESSAGE_END    			( );
	
	MESSAGE_BEGIN           ( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE      ( TE_BEAMFOLLOW );
			WRITE_SHORT     ( entindex() );
			WRITE_SHORT     ( BeamSprite );
			WRITE_BYTE      ( 4 );
			WRITE_BYTE      ( 2 );
			WRITE_BYTE      ( 10 );
			WRITE_BYTE      ( 30 );
			WRITE_BYTE      ( 255 );
			WRITE_BYTE      ( 100 );
	MESSAGE_END    			( );
	
	MESSAGE_BEGIN           ( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE      ( TE_BEAMFOLLOW );
			WRITE_SHORT     ( entindex() );
			WRITE_SHORT     ( BeamSprite );
			WRITE_BYTE      ( 3 );
			WRITE_BYTE      ( 1 );
			WRITE_BYTE      ( 250 );
			WRITE_BYTE      ( 210 );
			WRITE_BYTE      ( 0 );
			WRITE_BYTE      ( 200 );
	MESSAGE_END    			( );

}

void    CRC :: Precache( )
{
	BeamSprite = PRECACHE_MODEL( BLASTER_BEAM_SPRITE );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/blast.spr" );
}

void    CRC :: Hit( CBaseEntity* Target )
{
	TraceResult TResult;
	Vector      StartPosition;
	pev->enemy = Target->edict( );
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );
					
	Explode( &TResult, DMG_CRUSH );
}

void    CRC :: Explode( TraceResult* TResult, int DamageType )
{
	if ( TResult->flFraction != 1.0 )
		pev->origin = TResult->vecEndPos + (TResult->vecPlaneNormal * 25.6);
			  
    if( TResult->fAllSolid ) return;
		
	// TE_BEAMDISK
	// TE_BEAMTORUS
	// TE_BEAMRING
		
	UTIL_DecalTrace( TResult, DECAL_SMALLSCORCH1 + RANDOM_LONG(0,2) );
	
	// explode
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_SHORT( m_iSpriteTexture2 ); //other sprite
		WRITE_BYTE( 40  ); // scale * 10
		WRITE_BYTE( 16  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	
	// torus
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMTORUS );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z); 
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 800 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 2 ); // life
		WRITE_BYTE( 3 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 200 );   // r, g, b
		WRITE_BYTE( 64 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_EXPLODEMODEL	 );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x);///
		WRITE_SHORT(g_sModelIndexBloodDrop); // entity
		WRITE_SHORT( 3 ); // count
		WRITE_BYTE( 16 ); // life
	MESSAGE_END();

	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 200, CLASS_NONE, DMG_CRUSH  ); //DMG

	UTIL_Remove( this );
}

CRC* CRC :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CRC* Beam = GetClassPtr( (CRC*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CRC :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}

void    CRC :: MoveThink( )
{
	// delete object after 2.5 sec
	pev->nextthink = gpGlobals->time + 2.5;
	SetThink( SUB_Remove );
		
}



////////////////////////Rocket #2/////////////////////////


void    CRc2 :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(8,1,1) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_crowbar" );
	SetThink( MoveThink );
	SetTouch( Hit );
	pev->angles.x = -(pev->angles.x);
	pev->nextthink = gpGlobals->time + 0.1;
	pev->effects = EF_MUZZLEFLASH;
	m_flDie = gpGlobals->time + 15;
	pev->dmg = 120;
	
	// make rocket sound
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav", 0.3, 0.4 );
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex());	// entity
		WRITE_SHORT(m_iSpriteTexture );	// model
		WRITE_BYTE( 50 ); // life
		WRITE_BYTE( 2 );  // width
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 200 );	// brightness
	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
	
	pev->rendermode = kRenderNormal;
	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor.x = 255;  // red
	pev->rendercolor.y = 255;  // green
	pev->rendercolor.z = 255; // blue
	pev->renderamt = 100;

	pev->velocity = gpGlobals->v_forward * 475; //*350
}

void    CRc2 :: Precache( )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shock.spr" );
	m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/cexplo.spr" );
}

void    CRc2 :: Hit( CBaseEntity* Target )
{
	TraceResult TResult;
	Vector      StartPosition;
	pev->enemy = Target->edict( );
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );
					
	Explode( &TResult, DMG_CRUSH );
		
}

void    CRc2 :: Explode( TraceResult* TResult, int DamageType )
{
	STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );

	if ( TResult->flFraction != 1.0 )
		pev->origin = TResult->vecEndPos + (TResult->vecPlaneNormal * 25.6);

	if( TResult->fAllSolid ) return;
	UTIL_DecalTrace( TResult, DECAL_GARGSTOMP1 );
	
	// explode
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y + 16);
		WRITE_COORD( pev->origin.z);
		WRITE_SHORT( m_iSpriteTexture2 );
		WRITE_BYTE( 48  ); // scale
		WRITE_BYTE( 24  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_EXPLODEMODEL	 );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x);///
		WRITE_SHORT(g_sModelIndexBloodDrop); // entity
		WRITE_SHORT( 5 ); // count
		WRITE_BYTE( 16 ); // life
	MESSAGE_END();
	
	//beam
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x + 120);
		WRITE_COORD( pev->origin.y + 120);
		WRITE_COORD( pev->origin.z + 200 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 16 ); // framerate
		WRITE_BYTE( 2 ); // life
		WRITE_BYTE( 500 );  // width
		WRITE_BYTE( 64 );   // noise
		WRITE_BYTE( 55 );   // r, g, b
		WRITE_BYTE( 55 );   // r, g, b
		WRITE_BYTE( 55 );   // r, g, b
		WRITE_BYTE( 55 ); // brightness
		WRITE_BYTE( 8 );		// speed
	MESSAGE_END();
	
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 220, CLASS_NONE, DMG_CRUSH  ); //explDMG
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg*3, 36, CLASS_NONE, DMG_BULLET  ); //directalDMG
	
	UTIL_Remove( this );
}

CRc2* CRc2 :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
	CRc2* Beam = GetClassPtr( (CRc2*)NULL );

	UTIL_SetOrigin( Beam->pev, Pos );
	Beam->pev->angles = Aim;
	Beam->Spawn( );
	Beam->SetTouch( CRc2 :: Hit );
	Beam->pev->owner = Owner->edict( );
	return Beam;
}

void    CRc2 :: MoveThink( )
{
	Vector	vecSpitOffset; 
	Vector	vecSpitDir;
	vecSpitOffset = ( pev->origin ); 
	Vector vecTarget;
	vecTarget = gpGlobals->v_forward;
	Vector vecDir;
	CBaseEntity *pEntity = NULL;
	pEntity = CBaseEntity::Instance(pev->owner); // init player owner
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pEntity->pev );
	
	// player have lost
	if (!pl->IsAlive() && pev->renderamt == 100)
	{
		m_flDie=3000; // add more live to rocket
		pev->renderamt = 45;
		pev->rendercolor.y = 75;  // green
		pev->rendercolor.z = 75;  // blue
		STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );

	}
	// 4fun
	if ( (pl->pev->button & IN_JUMP) && (  pl->m_flNextChatTime3 < gpGlobals->time ) && (allowmonsters10.value == 1)) // lol !!  ! EEEEE
		{
			pl->m_flNextChatTime3 = gpGlobals->time + 1; //1 sec reload

			CBaseEntity *pEntity = NULL;
			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 112 )) != NULL)
				{
				if (pEntity->pev->friction != 1.0) //1.30a antifriction
					pEntity->pev->friction = 1.0;
				
				if ((pEntity->pev->takedamage==DAMAGE_YES || pEntity->pev->movetype==MOVETYPE_WALK) && (FVisible( pEntity ))) ///check only players
					{
						if (pEntity->edict() != pev->owner)
						{
							pEntity->TakeHealth(25, DMG_GENERIC);
							pEntity->pev->armorvalue += 3; 
						}
						else
						{
							pEntity->TakeHealth(1, DMG_GENERIC);
							pEntity->pev->armorvalue += 1;
						}
					}
				}
				
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_BEAMCYLINDER );
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z);
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z + 250 ); // reach damage radius over .2 seconds
				WRITE_SHORT( m_iSpriteTexture );
				WRITE_BYTE( 0 ); // startframe
				WRITE_BYTE( 0 ); // framerate
				WRITE_BYTE( 2 ); // life
				WRITE_BYTE( 30 );  // width
				WRITE_BYTE( 0 );   // noise
				WRITE_BYTE( 0 );   // r, g, b
				WRITE_BYTE( 200 );   // r, g, b
				WRITE_BYTE( 200 );   // r, g, b
				WRITE_BYTE( 100 ); // brightness
				WRITE_BYTE( 0 );		// speed
			MESSAGE_END();
			
			// EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/crystal_heal.wav", 0.8, ATTN_NORM); //play sound
			EMIT_SOUND_DYN(ENT(pl->pev), CHAN_WEAPON, "zxc/crystal_heal.wav", 0.8, ATTN_NORM, 0, 80 + RANDOM_LONG(128,0x4f));

			
		}
	
	if (allowmonsters10.value == 1)
	{
		if ( (pl != NULL) && (pEntity->edict() == pev->owner) && (pev->renderamt == 100) && (pl->pev->button & IN_RELOAD))
		{
			if ( (pl->m_pActiveItem->m_iId == WEAPON_CROWBAR)) // once crash reason, moved to this position
				m_flDie = -1;
		}
		else if ((pev->renderamt == 45) && (pEntity->edict() == pev->owner) && (pl->pev->button & IN_RELOAD))
		{
			m_flDie = -1; // destroy that after respawn
		}
	}
	
	
	if (gpGlobals->time >= m_flDie) //time out 15 s
		{
			// explode
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z);
				WRITE_SHORT( g_sModelIndexFireball );
				WRITE_BYTE( 40 ); // scale * 10
				WRITE_BYTE( 16  ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			
			// explode
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION );		// This just makes a dynamic light now
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y + 16);
				WRITE_COORD( pev->origin.z);
				WRITE_SHORT( m_iSpriteTexture2 );
				WRITE_BYTE( 48  ); // scale
				WRITE_BYTE( 24  ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			
			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg*2, 200, CLASS_NONE, DMG_CRUSH  ); //DMG
			UTIL_Remove( this );
		}
		
	if (allowmonsters10.value == 1)
		{
		if (m_moving == 0 && (gpGlobals->time >= m_flDie-12.5))
		{
			switch ( RANDOM_LONG( 0, 15 ) )
			{
				case 5:	m_moving = 1;
			}
		}
	}
	


	if (m_moving == 0)
	{
		vecSpitDir = ( pev->angles );
		vecSpitDir.x = 0;
		vecSpitDir.y = 0;
		vecSpitDir.z = 0;
		pev->velocity.x = (pev->velocity.x + RANDOM_LONG(-100,100));
		pev->velocity.z = (pev->velocity.z + RANDOM_LONG(-30,30));
	}
	else if (pEntity != NULL)
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
		vecDir = pPlayer->pev->origin - pev->origin;
		vecDir = vecDir.Normalize( );
		vecTarget = vecDir;
		pev->velocity = pev->velocity * 0.2 + vecTarget * 390 * 0.798; // fly to owner
	}
	
	pev->angles = UTIL_VecToAngles (pev->velocity);
	
	UTIL_Sparks( pev->origin );
	
	pev->nextthink = gpGlobals->time + 0.1;
			
}

