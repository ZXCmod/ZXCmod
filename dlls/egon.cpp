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
#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "customentity.h"
#include "gamerules.h"


#define	EGON_PRIMARY_VOLUME		450
#define EGON_BEAM_SPRITE		"sprites/xbeam1.spr"
#define EGON_FLARE_SPRITE		"sprites/XSpark1.spr"
#define EGON_SOUND_OFF			"weapons/egon_off1.wav"
#define EGON_SOUND_RUN			"weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP		"weapons/egon_windup2.wav"

#define EGON_SWITCH_NARROW_TIME			0.75			// Time it takes to switch fire modes
#define EGON_SWITCH_WIDE_TIME			1.5

enum egon_e {
	EGON_IDLE1 = 0,
	EGON_FIDGET1,
	EGON_ALTFIREON,
	EGON_ALTFIRECYCLE,
	EGON_ALTFIREOFF,
	EGON_FIRE1,
	EGON_FIRE2,
	EGON_FIRE3,
	EGON_FIRE4,
	EGON_DRAW,
	EGON_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_egon, CEgon );

void CEgon::Spawn( )
{
	Precache( );
	m_iId = WEAPON_EGON;
	SET_MODEL(ENT(pev), "models/w_egon.mdl");

	m_iDefaultAmmo = EGON_DEFAULT_GIVE;
	float m_flNextChatTime10 = gpGlobals->time; //delay
	FallInit();// get ready to fall down.
}


void CEgon::Precache( void )
{
	PRECACHE_MODEL("models/w_egon.mdl");
	PRECACHE_MODEL("models/v_egon.mdl");
	PRECACHE_MODEL("models/p_egon.mdl");
	PRECACHE_MODEL( "models/alt_nuke2.mdl" );

	PRECACHE_MODEL("models/w_9mmclip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_MODEL( "sprites/gradbeam.spr" );

	PRECACHE_SOUND( EGON_SOUND_OFF );
	PRECACHE_SOUND( EGON_SOUND_RUN );
	PRECACHE_SOUND( EGON_SOUND_STARTUP );

	PRECACHE_MODEL( EGON_BEAM_SPRITE );
	PRECACHE_MODEL( EGON_FLARE_SPRITE );

	PRECACHE_SOUND ("weapons/357_cock1.wav");

	m_usEgonFire = PRECACHE_EVENT ( 1, "events/egon_fire.sc" );
	m_usEgonStop = PRECACHE_EVENT ( 1, "events/egon_stop.sc" );
	
}


BOOL CEgon::Deploy( void )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 270 );
	m_deployed = FALSE;
	m_fireState = FIRE_OFF;
	return DefaultDeploy( "models/v_egon.mdl", "models/p_egon.mdl", EGON_DRAW, "egon" );
}

int CEgon::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}



void CEgon::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( EGON_HOLSTER );

    EndAttack();
}

int CEgon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_EGON;
	p->iFlags = 0;
	p->iWeight = EGON_WEIGHT;

	return 1;
}

#define EGON_PULSE_INTERVAL			0.1
#define EGON_DISCHARGE_INTERVAL		0.1

float CEgon::GetPulseInterval( void )
{
	return EGON_PULSE_INTERVAL;
}

float CEgon::GetDischargeInterval( void )
{
	return EGON_DISCHARGE_INTERVAL;
}

BOOL CEgon::HasAmmo( void )
{
	if ( m_pPlayer->ammo_uranium <= 0 )
		return FALSE;

	return TRUE;
}

void CEgon::UseAmmo( int count )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= count )
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;
	else
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CEgon::Attack( void )
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		
		if ( m_fireState != FIRE_OFF || m_pBeam )
		{
			EndAttack();
		}
		else
		{
			PlayEmptySound( );
		}
		return;
	}

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if ( !HasAmmo() )
			{
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
				PlayEmptySound( );
				return;
			}

			m_flAmmoUseTime = gpGlobals->time;// start using ammo ASAP.

			PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usEgonFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_fireState, m_fireMode, 1, 0 );
						
			m_shakeTime = 1.5;

			m_pPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
			pev->fuser1	= UTIL_WeaponTimeBase() + 2;

			pev->dmgtime = gpGlobals->time + GetPulseInterval();
			m_fireState = FIRE_CHARGE;
		}
		break;

		case FIRE_CHARGE:
		{
			Fire( vecSrc, vecAiming );
			
			m_pPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;
		
			if ( pev->fuser1 <= UTIL_WeaponTimeBase() )
			{
				PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usEgonFire, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_fireState, m_fireMode, 0, 0 );
				pev->fuser1 = 1000;
			}

			if ( !HasAmmo() )
			{
				EndAttack();
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
			}

		}
		break;
	}
//			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 12.9;	
//		m_fireState = FIRE_CHARGE;
}




void CEgon::PrimaryAttack( void )
{
	m_fireMode = FIRE_WIDE;
	Attack();


}

void CEgon::SecondaryAttack(void)
{
	
	#ifndef CLIENT_DLL
		m_pPlayer->pev->velocity = gpGlobals->v_forward * -260;	//fly!
		m_fireState = FIRE_CHARGE;
	#endif
		
		if ( gpGlobals->time >= m_flAmmoUseTime )
			{
				UseAmmo( 1 ); //eat ammo
				m_flAmmoUseTime = gpGlobals->time + 1.25; //eat 1 ammo\1.25 sec
			}
		//ammo out, end attack
		if ( !HasAmmo() ) 
			{
				EndAttack();
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
			}
}

void CEgon::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	Vector vecDest = vecOrigSrc + vecDir * 2048;
	edict_t		*pentIgnore;
	TraceResult tr;

	pentIgnore = m_pPlayer->edict();
	Vector tmpSrc = vecOrigSrc + gpGlobals->v_up * -8 + gpGlobals->v_right * 3;

	// ALERT( at_console, "." );
	
	UTIL_TraceLine( vecOrigSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr );

	if (tr.fAllSolid)
		return;

#ifndef CLIENT_DLL
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	if (pEntity == NULL)
		return;

	if ( g_pGameRules->IsMultiplayer() )
	{
		if ( m_pSprite && pEntity->pev->takedamage )
		{
			m_pSprite->pev->effects &= ~EF_NODRAW;
		}
		else if ( m_pSprite )
		{
			m_pSprite->pev->effects |= EF_NODRAW;
		}
	}


#endif

	float timedist;

	switch ( m_fireMode )
	{
	case FIRE_NARROW:
#ifndef CLIENT_DLL
		if ( pev->dmgtime < gpGlobals->time )
		{
			// Narrow mode only does damage to the entity it hits
			ClearMultiDamage();
			if (pEntity->pev->takedamage)
			{
				pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgEgonNarrow, vecDir, &tr, DMG_ENERGYBEAM );
			}
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// multiplayer uses 1 ammo every 1/10th second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.1;
				}
			}
			else
			{
				// single player, use 3 ammo/second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.166;
				}
			}

			pev->dmgtime = gpGlobals->time + GetPulseInterval();
		}
#endif
		timedist = ( pev->dmgtime - gpGlobals->time ) / GetPulseInterval();
		break;
	
	case FIRE_WIDE:
#ifndef CLIENT_DLL
		if ( pev->dmgtime < gpGlobals->time )
		{
			// wide mode does damage to the ent, and radius damage
			ClearMultiDamage();
			if (pEntity->pev->takedamage)
			{
				pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgEgonWide, vecDir, &tr, DMG_ENERGYBEAM | DMG_ALWAYSGIB);
			}
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// radius damage a little more potent in multiplayer.
				::RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, gSkillData.plrDmgEgonWide/4, 128, CLASS_NONE, DMG_ENERGYBEAM | DMG_BLAST | DMG_ALWAYSGIB );
			}

			if ( !m_pPlayer->IsAlive() )
				return;

			if ( g_pGameRules->IsMultiplayer() )
			{
				//multiplayer uses 5 ammo/second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.2;
				}
			}
			else
			{
				// Wide mode uses 10 charges per second in single player
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.1;
				}
			}

			pev->dmgtime = gpGlobals->time + GetDischargeInterval();
			if ( m_shakeTime < gpGlobals->time )
			{
				UTIL_ScreenShake( tr.vecEndPos, 5.0, 150.0, 0.75, 250.0 );
				m_shakeTime = gpGlobals->time + 1.5;
			}
		}
#endif
		timedist = ( pev->dmgtime - gpGlobals->time ) / GetDischargeInterval();
		break;
	}

	if ( timedist < 0 )
		timedist = 0;
	else if ( timedist > 1 )
		timedist = 1;
	timedist = 1-timedist;

	UpdateEffect( tmpSrc, tr.vecEndPos, timedist );
}


void CEgon::UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend )
{
	if ( !m_pBeam )
	{
		CreateEffect();
	}

	m_pBeam->SetStartPos( endPoint );
	m_pBeam->SetBrightness( RANDOM_FLOAT(15,235) - (timeBlend*180) );
	m_pBeam->SetWidth( RANDOM_FLOAT(25,55) - (timeBlend*20) );

	if ( m_fireMode == FIRE_WIDE )
		m_pBeam->SetColor( 255 + (25*timeBlend), 3 + (30*timeBlend), 3 + 80*fabs(sin(gpGlobals->time*10)) );
	else
		m_pBeam->SetColor( 41 + (25*timeBlend), 1 + (30*timeBlend), 255 + 80*fabs(sin(gpGlobals->time*10)) );


	UTIL_SetOrigin( m_pSprite->pev, endPoint );
	m_pSprite->pev->frame += 8 * gpGlobals->frametime;
	if ( m_pSprite->pev->frame > m_pSprite->Frames() )
		m_pSprite->pev->frame = 0;

	m_pNoise->SetStartPos( endPoint );

}

void CEgon::CreateEffect( void )
{
	DestroyEffect();

	m_pBeam = CBeam::BeamCreate( EGON_BEAM_SPRITE, 40 );
	m_pBeam->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pBeam->SetFlags( BEAM_FSINE );
	m_pBeam->SetEndAttachment( 1 );
	m_pBeam->pev->spawnflags |= SF_BEAM_TEMPORARY;	// Flag these to be destroyed on save/restore or level transition

	m_pNoise = CBeam::BeamCreate( EGON_BEAM_SPRITE, 55 );
	m_pNoise->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pNoise->SetScrollRate( 225 );
	m_pNoise->SetBrightness( 255 );
	m_pNoise->SetEndAttachment( 1 );
	m_pNoise->pev->spawnflags |= SF_BEAM_TEMPORARY;

	m_pSprite = CSprite::SpriteCreate( EGON_FLARE_SPRITE, pev->origin, FALSE );
	m_pSprite->pev->scale = 1.0;
	m_pSprite->SetTransparency( kRenderGlow, 255, 55, 55, 255, kRenderFxNoDissipation );
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;

	if ( m_fireMode == FIRE_WIDE )
	{
		m_pBeam->SetScrollRate( 50 );
		m_pBeam->SetNoise( 3 );
		m_pNoise->SetColor( 25,34,76 );
		m_pNoise->SetNoise( 43 );
	}
	else
	{
		m_pBeam->SetScrollRate( 7 );
		m_pBeam->SetNoise( 5 );
		m_pNoise->SetColor( 216, 53, 47 );
		m_pNoise->SetNoise( 5 );
	}
}


void CEgon::DestroyEffect( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
	if ( m_pNoise )
	{
		UTIL_Remove( m_pNoise );
		m_pNoise = NULL;
	}
	if ( m_pSprite )
	{
		if ( m_fireMode == FIRE_WIDE )
			m_pSprite->Expand( 10, 500 );
		else
			UTIL_Remove( m_pSprite );
		m_pSprite = NULL;
	}
}



void CEgon::WeaponIdle( void )
{
	ResetEmptySound( );
	
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if ( m_fireState != FIRE_OFF )
		 EndAttack();
	
	int iAnim;

	float flRand = RANDOM_FLOAT(0,1);

	if ( flRand <= 0.5 )
	{
		iAnim = EGON_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
	else 
	{
		iAnim = EGON_FIDGET1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	}

	SendWeaponAnim( iAnim );
	m_deployed = TRUE;
	


//new code - launch big bomb by pressed Reload key
if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 25) 
	{
	if (  m_pPlayer->m_flNextChatTime10 < gpGlobals->time ) //need delay
		{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.9, ATTN_NORM); //play sound
	
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 700; //init and start speed of core

		#ifndef CLIENT_DLL
			CBaseEntity *pSatchel = Create( "weapon_frag", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSatchel->pev->velocity = vecThrow;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 25;
			m_pPlayer->m_flNextChatTime10 = gpGlobals->time + 2.25;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		#endif
		return;
		}
	}
}



void CEgon::EndAttack( void )
{
	bool bMakeNoise = false;
		
	if ( m_fireState != FIRE_OFF ) //Checking the button just in case!.
		 bMakeNoise = true;

	PLAYBACK_EVENT_FULL( FEV_GLOBAL | FEV_RELIABLE, m_pPlayer->edict(), m_usEgonStop, 0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, bMakeNoise, 0, 0, 0 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;

	m_fireState = FIRE_OFF;

	DestroyEffect();
}



class CEgonAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_chainammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_chainammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_egonclip, CEgonAmmo );

#endif


//////////////NEW weapon

class   CBfb : public CBaseEntity
{
        public:

        void    Spawn           ( );
		void Precache ();
        void    MoveThink       ( );
        void    Explode         (int);
		int m_flDie;
		int m_flDie2;
		int     BeamSprite;
		int m_iSpriteTexture;
		short		m_LaserSprite;
		int m_iBalls;
		
		static CBfb* Create( Vector, Vector, CBaseEntity* );
		void EXPORT Hit         ( CBaseEntity* );
		
};

LINK_ENTITY_TO_CLASS( weapon_frag, CBfb );



void    CBfb :: Spawn( )
{
		Precache( );
		
        SET_MODEL( ENT(pev), "models/alt_nuke2.mdl" );
        pev->movetype = MOVETYPE_BOUNCE;
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
		pev->angles.x = -(pev->angles.x);
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "BFBomb" );
		m_flDie = gpGlobals->time + 5;
		pev->dmg = 100;
		pev->takedamage = DAMAGE_YES;
		pev->nextthink = gpGlobals->time + 0.3;
		SetThink( MoveThink );
		pev->gravity			= 0.35;
		pev->friction			= 0.35;
		pev->health			= 9999; 
		
		SetTouch( Hit );
		
/* 		CBaseEntity *pEntity;
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
		pPlayer->pev->SET_VIEW( pActivator->edict(), edict() ); */
		
			//SET_VIEW( m_hPlayer->edict(), m_hPlayer->edict() );
			//((CBasePlayer *)((CBaseEntity *)m_hPlayer))->EnableControl(TRUE);
			
		//CBaseEntity *pEntity = NULL;
		//SET_VIEW( edict(), pEntity->edict() );
		
}

void CBfb :: Precache( void )
{
m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
PRECACHE_SOUND( "weapons/gravgren.wav" );
m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
PRECACHE_MODEL( "models/alt_nuke2.mdl" );
m_iBalls = PRECACHE_MODEL( "sprites/gradbeam.spr" );
}

///////////////
CBfb* CBfb :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CBfb* Beam = GetClassPtr( (CBfb*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CBfb :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}

void    CBfb :: Hit( CBaseEntity* Target )
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
   
		
		
//full explode after touch with wall
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(291,549), 205, CLASS_NONE, DMG_MORTAR  ); //end blast
/* 		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x + 350);
			WRITE_COORD( pev->origin.y + 250);
			WRITE_COORD( pev->origin.z + 150 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 1 ); // framerate
			WRITE_BYTE( 12 ); // life
			WRITE_BYTE( 25 );  // width
			WRITE_BYTE( 51 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 12 );		// speed
		MESSAGE_END(); */


		//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 42 );		// radius * 0.1
		WRITE_BYTE( 128 );		// r
		WRITE_BYTE( 16 );		// g
		WRITE_BYTE( 0 );		// b
		WRITE_BYTE( 128 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
//
TraceResult tr, beam_tr;
		// balls
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
			WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( pev->origin.x + tr.vecPlaneNormal.x*3 );
			WRITE_COORD( pev->origin.y  + tr.vecPlaneNormal.y*3 );
			WRITE_COORD( pev->origin.z + tr.vecPlaneNormal.z*3  );
			WRITE_SHORT( m_iBalls );		// model
			WRITE_BYTE( 20  );				// count
			WRITE_BYTE( 7 );				// life * 10
			WRITE_BYTE( RANDOM_LONG( 1, 2 ) );				// size * 10
			WRITE_BYTE( 90 );				// amplitude * 0.1
			WRITE_BYTE( 2 );				// speed * 100
		MESSAGE_END();
		


	
		// random explosions
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 62  ); // scale * 10
			WRITE_BYTE( RANDOM_LONG(8,10)  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	//UTIL_ScreenShake( pEntity->pev->origin, 12.0, 120, 0.9, 1 ); 
	pev->takedamage = DAMAGE_NO;
	SetThink( SUB_Remove );
}
/////////////////



void    CBfb:: Explode(int DamageType)
{


		
		//set trails
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 40 ); // life
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 60 ); // r, g, b
		WRITE_BYTE( 0 ); // r, g, b
		WRITE_BYTE( 128 ); // brightness
		MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
		
		//Vector vecThrow = gpGlobals->v_forward * 1024; //init and start speed of core
		//pev->velocity = vecThrow;
		//pev->velocity = pev->velocity * 2.3;
		pev->movetype = MOVETYPE_FLY;
		
		//CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		//ENT( pEntity->pev );
		




pev->nextthink = gpGlobals->time + 0.15;
pev->effects |= EF_LIGHT;
SetThink(MoveThink);
}


void    CBfb :: MoveThink( )
{
TraceResult tr, beam_tr;
		// balls
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
			WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( m_iBalls );		// model
			WRITE_BYTE( 3  );				// count
			WRITE_BYTE( 2 );				// life * 10
			WRITE_BYTE( 1 );				// size * 10
			WRITE_BYTE( 24 );				// amplitude * 0.1
			WRITE_BYTE( 24 );				// speed * 100
		MESSAGE_END();

Explode(DMG_FREEZE);
if (gpGlobals->time >= m_flDie) //full explode and self destroy
	{
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(71,83), 512, CLASS_NONE, DMG_MORTAR  ); //end blast
	pev->takedamage = DAMAGE_NO;
	SetThink( SUB_Remove );
}

}



















