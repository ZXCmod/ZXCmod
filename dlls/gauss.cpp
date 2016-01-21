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
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"
#include "game.h"
#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

/////new weap/////

//nukebomb values
#define BLASTER_BEAM_RED                RANDOM_LONG( 128, 255 )
#define BLASTER_BEAM_GREEN              RANDOM_LONG( 1, 3 )
#define BLASTER_BEAM_BLUE               RANDOM_LONG( 1, 3 )
#define BLASTER_BEAM_BRIGHTNESS 128
#define BLASTER_BEAM_WIDTH      RANDOM_LONG( 3, 4 )
#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"
#define BLASTER_BEAM_SPEED      300
#define BLASTER_DAMAGE          RANDOM_LONG( 60, 90 )
#define BLASTER_BEAM_LENGTH     RANDOM_LONG( 26, 34 )
#define BLASTER_BEAM_RANDOMNESS RANDOM_LONG( 1, 24 )
#define BLASTER_OFFSET_FORWARD  0
#define BLASTER_OFFSET_RIGHT    7
#define BLASTER_OFFSET_UP               0







//nuke class
class   CBlaster2Beam : public CGrenade
{
        public:
        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void 	EXPORT Hit      ( CBaseEntity* );
        void    Explode         ( TraceResult*, int );
        static 	CBlaster2Beam* Create( Vector, Vector, CBaseEntity* );
		CBasePlayer *pPlayer;
        int     BeamSprite;
		float 	m_flDie;
		float 	m_flDie2;
		float 	m_flDie3;
		void    Ef              ( );
		void    Explode2 ( TraceResult*, int);
		float 	dmge;
		int 	rad;
		int 	m_iSpriteTexture2;
};

//Radiation by nuke 
class   CRadiation : public CBaseEntity
{
        public:

        void    Spawn           ( );
        void    MoveThink       ( );
        void    Explode         ( );
		int m_flDie10;
		int m_iSpriteTexture2;
};
LINK_ENTITY_TO_CLASS( trigger_killmonster, CRadiation );

/////end new weap's/////


enum gauss_e {
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW,
	GAUSS_RELOAD
};

LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );

float CGauss::GetFullChargeTime( void )
{

	return 1.5;
	
}

#ifdef CLIENT_DLL
	extern int g_irunninggausspred;
#endif


void CGauss::Spawn( )
{
	Precache( );
	m_iId = WEAPON_GAUSS;
	SET_MODEL(ENT(pev), "models/w_gauss.mdl");

	m_iDefaultAmmo = GAUSS_DEFAULT_GIVE;
	m_flNextChatTime3 = gpGlobals->time;

	FallInit();
}

void CGauss::Precache( void )
{

	PRECACHE_MODEL("models/w_gauss.mdl");
	PRECACHE_MODEL("models/v_gauss.mdl");
	PRECACHE_MODEL("models/p_gauss.mdl");
	PRECACHE_MODEL("models/nuke.mdl");
	PRECACHE_MODEL("models/nukeT.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("buttons/bell1.wav");

	PRECACHE_SOUND("weapons/gauss2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("ambience/pulsemachine.wav");
	PRECACHE_SOUND("zxc/hold_loop.wav");
	
	PRECACHE_SOUND("zxc/superphys_launch2.wav");
	PRECACHE_SOUND("zxc/superphys_launch3.wav");
	PRECACHE_SOUND("zxc/superphys_launch4.wav");
	PRECACHE_SOUND("zxc/energy_sing_explosion2.wav");
	PRECACHE_SOUND("zxc/ar2_altfire1.wav");
	
	
	
	m_iGlow = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBeam = PRECACHE_MODEL( "sprites/smoke.spr" );

	m_usGaussFire = PRECACHE_EVENT( 1, "events/gauss.sc" );
	m_usGaussSpin = PRECACHE_EVENT( 1, "events/gaussspin.sc" );
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");
	PRECACHE_MODEL("sprites/shrinkf.spr");
	PRECACHE_MODEL( "sprites/zbeam1.spr" );
	m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/cexplo.spr" );
	
	PRECACHE_MODEL( "sprites/nhth1.spr" );
	PRECACHE_MODEL( "sprites/yelflare1.spr" );
	PRECACHE_MODEL( "sprites/richo2.spr" ); //explode
	PRECACHE_MODEL( "sprites/zbeam4.spr" );

}

int CGauss::AddToPlayer( CBasePlayer *pPlayer )
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

int CGauss::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_GAUSS;
	p->iFlags = 0;
	p->iWeight = GAUSS_WEIGHT;
	p->iId = m_iId = WEAPON_GAUSS;
	return 1;
}

BOOL CGauss::Deploy( )
{
	m_pPlayer->m_flPlayAftershock = 0.0;
	return DefaultDeploy( "models/v_gauss.mdl", "models/p_gauss.mdl", GAUSS_DRAW, "gauss" );
            //SetThink(Spawn2);

}

void CGauss::Holster( int skiplocal /* = 0 */ )
{
	PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );
	
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( GAUSS_HOLSTER );
	m_fInAttack = 0;

}


void CGauss::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] < 2 )
	{
		PlayEmptySound( );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	m_fPrimaryFire = TRUE;
	m_fFourthFire = FALSE;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1; 

	StartFire();
	m_fInAttack = 0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.2;
}

void CGauss::SecondaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		if ( m_fInAttack != 0 )
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			SendWeaponAnim( GAUSS_IDLE );
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound( );
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	//*
	if ( m_fInAttack == 0 )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}
		// fire
		m_fPrimaryFire = FALSE;
		m_fFourthFire = FALSE;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		// spin up
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		SendWeaponAnim( GAUSS_SPINUP );
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH;
	}
	
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 900 )
		{

			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
			}

		}

		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			return;
		}
		
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
		{
			// don't eat any more ammo after gun is fully charged.
			m_pPlayer->m_flNextAmmoBurn = 900;
		}

		int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 150 / GetFullChargeTime() ) + 100;
		if ( pitch > 270 ) 
			 pitch = 270;

		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		// m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 9 )
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
				
			#ifndef CLIENT_DLL
				m_pPlayer->TakeDamage( VARS(eoNullEntity), VARS(eoNullEntity), 50, DMG_SHOCK );
				UTIL_ScreenFade( m_pPlayer, Vector(255,0,0), 4, 8.5, 200, FFADE_IN );
			#endif
			
			SendWeaponAnim( GAUSS_IDLE );
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
}


//// magnet ray ////

void CGauss::ThirdAttack( void )
{
	CBaseEntity *pEntity;
	TraceResult	tr;	
	Vector vecSrc;
	vecSrc = m_pPlayer->GetGunPosition( );
	Vector vecDir = gpGlobals->v_forward;
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 2048, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	pEntity = CBaseEntity::Instance(tr.pHit); //trace hit to entity
		
	#ifndef CLIENT_DLL
	if (pEntity != NULL && pEntity->pev->takedamage && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1) //  && pEntity->IsPlayer()
    {
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;

		//ray
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(vecSrc.x);
            WRITE_COORD(vecSrc.y);
            WRITE_COORD(vecSrc.z);
            WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
            WRITE_COORD( pEntity->pev->origin.y );
            WRITE_COORD( pEntity->pev->origin.z );
            WRITE_SHORT( BSpr ); //sprite
            WRITE_BYTE( 0 ); // Starting frame
            WRITE_BYTE( 0  ); // framerate * 0.1
            WRITE_BYTE( 1 ); // life * 0.1
            WRITE_BYTE( 12 ); // width
            WRITE_BYTE( 15 ); // noise
            WRITE_BYTE( 125 ); // color r,g,b
            WRITE_BYTE( 25 ); // color r,g,b
            WRITE_BYTE( 1 ); // color r,g,b
            WRITE_BYTE( 100 ); // brightness
            WRITE_BYTE( 100 ); // scroll speed
		MESSAGE_END();
			
		pEntity->pev->punchangle.z = RANDOM_LONG(-35,25);
		pEntity->pev->punchangle.x = RANDOM_LONG(-25,35);
		pEntity->TakeDamage(pev, VARS( pev->owner ), 3, DMG_FALL);
		
		//fix cheat in teamplay
		CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
		if ( (g_pGameRules->PlayerRelationship( pOwner, pEntity ) != GR_TEAMMATE) && m_pPlayer->pev->health <= 300 && pEntity->IsPlayer())
			m_pPlayer->pev->health += 3.0; //take unlimited health

		// pEntity->pev->velocity.x = ( ( m_pPlayer->pev->velocity.x + m_pPlayer->pev->origin.x) - pEntity->pev->origin.x);
		// pEntity->pev->velocity.y = ( ( m_pPlayer->pev->velocity.y + m_pPlayer->pev->origin.y) - pEntity->pev->origin.y);
		// pEntity->pev->velocity.z = ( ( m_pPlayer->pev->velocity.z + m_pPlayer->pev->origin.z) - pEntity->pev->origin.z);
		pEntity->pev->velocity = m_pPlayer->pev->velocity + gpGlobals->v_forward  * -220; //new code
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 0.015; 
		#endif
	
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;
	}
	
	

}


void CGauss::FourthAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		if ( m_fInAttack != 0 )
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			SendWeaponAnim( GAUSS_IDLE );
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound( );
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	//*
	if ( m_fInAttack == 0 )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}
		
		// fire
		m_fPrimaryFire = FALSE;
		m_fFourthFire = TRUE; //toggle of fires, old permanent bug-crash

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		// spin up
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		SendWeaponAnim( GAUSS_SPINUP );
		m_fInAttack = 1;
		
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

		//PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH;
	}
	
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 900 )
		{

			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
			}

		}

		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			return;
		}
		
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
		{
			// don't eat any more ammo after gun is fully charged.
			m_pPlayer->m_flNextAmmoBurn = 900;
		}

		int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 150 / GetFullChargeTime() ) + 100;
		if ( pitch > 270 ) 
			 pitch = 270;

		
		
		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		
		/////great explode
		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 5 )
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
				
			//heave explode part
			#ifndef CLIENT_DLL
				UTIL_ScreenFade( m_pPlayer, Vector(255,0,0), 4, 8.5, 160, FFADE_IN );
				//explode
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
					WRITE_COORD( pev->origin.x);
					WRITE_COORD( pev->origin.y);
					WRITE_COORD( pev->origin.z);
					WRITE_SHORT( m_iSpriteTexture2 ); //other sprite
					WRITE_BYTE( 80  ); // scale * 10
					WRITE_BYTE( 12  ); // framerate
					WRITE_BYTE( TE_EXPLFLAG_NONE );
				MESSAGE_END();
				::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 640, CLASS_NONE, DMG_MORTAR|DMG_BULLET  ); //DMG
			#endif
			
			SendWeaponAnim( GAUSS_IDLE );
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
}




//now unused
BOOL CGauss::Lock( )
{

	return TRUE;

}



//=========================================================
// StartFire- since all of this code has to run and then 
// call Fire(), it was easier at this point to rip it out 
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names 
//=========================================================
void CGauss::StartFire( void )
{
	float flDamage;
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition( ); // + gpGlobals->v_up * -8 + gpGlobals->v_right * 8;
	
	if ( gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime() )
	{
		flDamage = 150;
	}
	else
	{
		flDamage = 150 * (( gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime() );
	}

	if ( m_fPrimaryFire && !m_fFourthFire )
	{
		// changed damage on primary attack
		flDamage = RANDOM_LONG(10,20);

	}

	if (m_fInAttack != 3)
	{

	#ifndef CLIENT_DLL
		float flZVel = m_pPlayer->pev->velocity.z;

		if ( m_fPrimaryFire==FALSE && allowmonsters6.value != 0 ) // added in 1.30a
		{
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * (flDamage+50) * 5;
		}

		if ( !g_pGameRules->IsMultiplayer() )

		{
			// in deathmatch, gauss can pop you up into the air. Not in single play.
			m_pPlayer->pev->velocity.z = flZVel;
		}
	#endif
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	}

	// time until aftershock 'static discharge' sound
	m_pPlayer->m_flPlayAftershock = gpGlobals->time + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.3, 0.8 );

	Fire( vecSrc, vecAiming, flDamage );
}

void CGauss::Fire( Vector vecOrigSrc, Vector vecDir, float flDamage )
{
	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;

	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;
	edict_t		*pentIgnore;
	TraceResult tr, beam_tr;
	float flMaxFrac = 1.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 40;

	pentIgnore = ENT( m_pPlayer->pev );

	#ifdef CLIENT_DLL
		if ( m_fPrimaryFire == false )
			 g_irunninggausspred = true;
	#endif
	
	// The main firing event is sent unreliably so it won't be delayed.
	if (m_fFourthFire == FALSE) //the laser visual line, no other effects, disable when 4th attack
		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, flDamage, 0.0, 0, 0, m_fPrimaryFire ? 1 : 0, 0 );

	// This reliable event is used to stop the spinning sound
	// It's delayed by a fraction of second to make sure it is delayed by 1 frame on the client
	// It's sent reliably anyway, which could lead to other delays
	PLAYBACK_EVENT_FULL( FEV_GLOBAL | FEV_RELIABLE, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );

	
	/*ALERT( at_console, "%f %f %f\n%f %f %f\n", 
		vecSrc.x, vecSrc.y, vecSrc.z, 
		vecDest.x, vecDest.y, vecDest.z );*/
	

//	ALERT( at_console, "%f %f\n", tr.flFraction, flMaxFrac );

	#ifndef CLIENT_DLL
	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		// ALERT( at_console, "." );
		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);

		if (tr.fAllSolid)
			break;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (pEntity == NULL)
			break;

		if ( fFirstBeam )
		{
			m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
			fFirstBeam = 0;
	
			nTotal += 26;
		}
		
		if (pEntity->pev->takedamage && m_fFourthFire == FALSE)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecDir, &tr, DMG_BULLET );
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}

		if ( pEntity->ReflectGauss() && m_fFourthFire == FALSE) //normal 2nd attack
		{ //* delete here to disable true laser
			float n;

			pentIgnore = m_pPlayer->edict();

			n = -DotProduct(tr.vecPlaneNormal, vecDir);

			if (n < 0.5) // 60 degrees
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				Vector r;
			
				r = 2.0 * tr.vecPlaneNormal * n + vecDir;
				flMaxFrac = flMaxFrac - tr.flFraction;
				vecDir = r;
				vecSrc = tr.vecEndPos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;

				// explode a bit
				m_pPlayer->RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, flDamage * n, CLASS_NONE, DMG_BLAST );

				nTotal += 34;
				
				// lose energy
				if (n == 0) n = 0.1;
					flDamage = flDamage * (1 - n);
				
			}
			else
			{
				nTotal += 13;

				// limit it to one hole punch
				if (fHasPunched)
					break;
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( m_fPrimaryFire == FALSE && m_fFourthFire == FALSE ) //if second attack
				{
					//UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
					m_pPlayer->pev->punchangle.x = -(flDamage/7); //hardest aim, stop easy fraging
					UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, pentIgnore, &beam_tr);
					if (!beam_tr.fAllSolid)
					{
						// trace backwards to find exit point
						UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, pentIgnore, &beam_tr);

						float n = (beam_tr.vecEndPos - tr.vecEndPos).Length( );

						if (n < flDamage)
						{
							if (n == 0) n = 1;
							flDamage -= n;

							// ALERT( at_console, "punch %f\n", n );
							nTotal += 21;

							// exit blast damage
							//m_pPlayer->RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, CLASS_NONE, DMG_BLAST );
							float damage_radius;
							

				
							damage_radius = flDamage * 2.05;  // Old code == 2.5


							::RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, damage_radius, CLASS_NONE, DMG_BLAST );

							CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );

							nTotal += 53;

							vecSrc = beam_tr.vecEndPos + vecDir;
						}
					}
					else
					{
						 //ALERT( at_console, "blocked %f\n", n );
						flDamage = 0;
					}
				}
				else
				{
					//ALERT( at_console, "blocked solid\n" );
					
					flDamage = 0;
				}
				return;
			}
		
		} //*
		else if (m_fFourthFire == TRUE)
		{
			Vector vecThrow = gpGlobals->v_forward * 800;
			Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
			UTIL_MakeVectors( anglesAim );

			#ifndef CLIENT_DLL
				CBaseEntity *pGauss = Create( "virtual_hull", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
				pGauss->pev->velocity = vecThrow;
				pGauss->pev->scale = (flDamage * 0.02);
				pGauss->pev->dmg = (flDamage * 0.70);
			#endif
			SendWeaponAnim( 5 );
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			//m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 15;
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
			return;

			
		}
		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
		}
	}
	return;
	#endif
}



void CGauss::WeaponIdle( void )
{
	//if 
	//nreloaddelay++;
	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 200) 

	if (  m_pPlayer->m_flNextChatTime3 < gpGlobals->time )
	{

		{
			// reload when reload is pressed, or if no buttons are down and weapon is empty.
			//nreloaddelay = 20;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			Reload();
			//m_flStartThrow = gpGlobals->time;z
			m_pPlayer->m_flNextChatTime3 = gpGlobals->time + 300; //5 minutes
			return;
			
		}
	}

	else
	{
		if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 200) 
		{
			PlayEmptySound();
			return;
		}
	}
	
	
	//ResetEmptySound( );

	// play aftershock static discharge
	if ( m_pPlayer->m_flPlayAftershock && m_pPlayer->m_flPlayAftershock < gpGlobals->time )
	{
		switch (RANDOM_LONG(0,3))
		{
			case 0:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro5.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro6.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
			case 3:	break; // no sound
		}
		m_pPlayer->m_flPlayAftershock = 0.0;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	}
	else
	{
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5)
		{
			iAnim = GAUSS_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		}
		else if (flRand <= 0.75)
		{
			iAnim = GAUSS_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		}
		else
		{
			iAnim = GAUSS_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
		}

		return;
		SendWeaponAnim( iAnim );
	}
	

}


void CGauss :: Reload( void )
{

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector GunPosition = m_pPlayer->GetGunPosition( );
	GunPosition = GunPosition + gpGlobals->v_forward * BLASTER_OFFSET_FORWARD;
	GunPosition = GunPosition + gpGlobals->v_right   * BLASTER_OFFSET_RIGHT;
	GunPosition = GunPosition + gpGlobals->v_up      * BLASTER_OFFSET_UP;


	CBlaster2Beam* Beam = CBlaster2Beam :: Create( GunPosition,
											 m_pPlayer->pev->v_angle,
											 m_pPlayer );

	float RandomX = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
								   BLASTER_BEAM_RANDOMNESS );
	float RandomY = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
								   BLASTER_BEAM_RANDOMNESS );

	Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right * RandomX;
	Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up    * RandomY;
	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 200;
	//dont silent spawn
	UTIL_ShowMessageAll( "Nuke bomb launched!"  ); // STRING(m_pPlayer->pev->netname)
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "buttons/bell1.wav", 1, ATTN_NORM);
}



class CGaussAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_gaussammo.mdl");
		CBasePlayerAmmo::Spawn( );
		//
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_gaussammo.mdl");
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
LINK_ENTITY_TO_CLASS( ammo_gaussclip, CGaussAmmo );

#endif




/**********************************

Nuke bomb code

**********************************/
#define SQUEEK_DETONATE_DELAY	16.0
#define SQUEEK_DETONATE_DELAY2	4.0
#define SQUEEK_DETONATE_DELAY3	4.5
void    CBlaster2Beam :: Spawn( )
{
        Precache( );
        SET_MODEL( ENT(pev), "models/nuke.mdl" );
        pev->movetype = MOVETYPE_BOUNCE;
        pev->solid = SOLID_BBOX;
		pev->flags |= FL_MONSTER;
		
        UTIL_SetSize( pev, Vector(-1,-1,-1), Vector(1,1,1) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "nuke" );
        SetThink( MoveThink );
        SetTouch( Hit );
        pev->velocity = gpGlobals->v_forward * BLASTER_BEAM_SPEED;
		pev->gravity = 0.65;
		pev->health = 9999;
		pev->friction = 0.015;
        pev->dmg = DMG_BLAST;
		dmge = pev->dmg = 160; //first explode
		m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;
		m_flDie2 = gpGlobals->time + SQUEEK_DETONATE_DELAY2;
		m_flDie3 = gpGlobals->time + SQUEEK_DETONATE_DELAY3;
		pev->takedamage = DAMAGE_YES;

}

void    CBlaster2Beam :: Precache( )
{
		m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/shockwave.spr" );
        BeamSprite = PRECACHE_MODEL( BLASTER_BEAM_SPRITE );
}

void    CBlaster2Beam :: Hit( CBaseEntity* Target )
{
		pev->angles = UTIL_VecToAngles (pev->velocity);
        TraceResult TResult;
        Vector      StartPosition;
        pev->enemy = Target->edict( );
        StartPosition = pev->origin - pev->velocity.Normalize() * 32;
        UTIL_TraceLine( StartPosition,
                        StartPosition + pev->velocity.Normalize() * 64,
                        dont_ignore_monsters,
                        ENT( pev ),
                        &TResult );
						

	if (gpGlobals->time >= m_flDie)
	{
		dmge = pev->dmg = 0; // Set dmg to zero, test.
		Explode( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR ); //direct damage, moved to radiation spawn
		return;
	}

	if (gpGlobals->time >= m_flDie2)
	{
		pev->effects |= EF_INVLIGHT;
		Explode( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR );
		Explode2( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR );
		pev->nextthink = gpGlobals->time + 0.1;
	}

}
void    CBlaster2Beam :: Explode( TraceResult* TResult, int DamageType )
{
	if( TResult->fAllSolid ) return;
	
        RadiusDamage( pev,
                      VARS( pev->owner ),
                      pev->dmg,
                      CLASS_NONE,
                      DamageType );

	dmge = pev->dmg = RANDOM_LONG(80,100);

	if (gpGlobals->time >= m_flDie)
		{
			pev->takedamage = DAMAGE_NO;
			
			//MORE EFFECTS!
			CBaseEntity *pEntity = NULL;
			Vector	vecDir;
			vecDir = Vector( 0, 0, 0 );

			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 1600 )) != NULL) //1600
				{
					if (pEntity->pev->takedamage || pEntity->pev->solid == SOLID_NOT) ///check only players
					{
						vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize(); ///NOW WORKED! CONGRATULATIONS!
						pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -2048;
						UTIL_ScreenShake( pEntity->pev->origin, 1024.0, 90.5, 154.7, 1 );
						
						//explode, earthquake damage
						if ( (pEntity->pev->health < 49) || (pEntity->pev->health == 100) )
							pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(1,37), DMG_BURN); //nuke wave immune bonus
						else
							pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(100,150), DMG_BURN); //nuke wave immune bonus

						UTIL_ScreenFade( pEntity, Vector(RANDOM_LONG(128,255),RANDOM_LONG(0,64),0), 300, 30, 100, FFADE_IN );
						pEntity->pev->punchangle.x = 10;
						pEntity->pev->punchangle.y = RANDOM_LONG(-74, 40);
						pEntity->pev->punchangle.z = -20;
					}
				}
				
			CBaseEntity::Create( "trigger_killmonster", pev->origin, pev->angles, pev->owner );
			pev->nextthink = gpGlobals->time + 0.1;
			SUB_Remove( );
		}

}

void    CBlaster2Beam :: Explode2( TraceResult* TResult, int DamageType )
{
	{
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -50, 50 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -50, 50 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 10, 50 ));
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( RANDOM_LONG(10,20) + 20  ); // scale * 10
		WRITE_BYTE( 10  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
		
	if (gpGlobals->time >= m_flDie3)
		{
			// blast circle in process (bounce)
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 100 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture2 );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 5 ); // framerate
			WRITE_BYTE( 20 ); // life
			WRITE_BYTE( 4 );  // width
			WRITE_BYTE( 255 );   // noise
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 192 );   // r, g, b
			WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();
		}
	}
}

CBlaster2Beam* CBlaster2Beam :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
	CBlaster2Beam* Beam = GetClassPtr( (CBlaster2Beam*)NULL );
	UTIL_SetOrigin( Beam->pev, Pos );
	Beam->Spawn( );
	Beam->SetTouch( CBlaster2Beam :: Hit );
	Beam->pev->owner = Owner->edict( );
	Beam->pev->angles = UTIL_VecToAngles (Beam->pev->velocity);
	return Beam;
}

void    CBlaster2Beam :: MoveThink( )
{
	pev->angles = UTIL_VecToAngles (pev->velocity); //dynamic angles
	
	MESSAGE_BEGIN           ( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE      ( TE_BEAMFOLLOW );
			WRITE_SHORT     ( entindex() );
			WRITE_SHORT     ( BeamSprite );
			WRITE_BYTE      ( BLASTER_BEAM_LENGTH );
			WRITE_BYTE      ( BLASTER_BEAM_WIDTH );
			WRITE_BYTE      ( BLASTER_BEAM_RED );
			WRITE_BYTE      ( BLASTER_BEAM_GREEN );
			WRITE_BYTE      ( BLASTER_BEAM_BLUE );
			WRITE_BYTE      ( BLASTER_BEAM_BRIGHTNESS );
	MESSAGE_END             ( );
}

//////////////radiation point

void    CRadiation :: Spawn( )
{
        SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
        //pev->movetype = MOVETYPE_NONE;
        pev->solid = SOLID_BBOX;
		//pev->effects |= EF_LIGHT;
        pev->rendermode = kRenderTransTexture;
        pev->renderamt = 0;
        UTIL_SetSize( pev, Vector(2,2,2), Vector(2,2,2) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "Nuke_Radiation" );
		m_flDie10 = gpGlobals->time + 120; // 180 old
		pev->dmg = 5;
		pev->takedamage = DAMAGE_YES;
		pev->nextthink = gpGlobals->time + 0.1;//10 times a second
		SetThink( MoveThink );
		m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/shockwave.spr" );
		pev->health			= 30000;
		pev->gravity		= 0;
		pev->friction		= 0;
	
	
	
	
///////////////////	
////moved from nuke (< 1.26)
///////////////////

	//Explode
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 10240, 10240, CLASS_NONE, DMG_BURN  ); //*10 increase dmg! (< 1.26)

	//effects
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( RANDOM_LONG(10,20) + 70  ); // scale * 10
		WRITE_BYTE( 3  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	// random explosions
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -250, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 250 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( RANDOM_LONG(10,20) + 90  ); // scale * 10
		WRITE_BYTE( 1  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	// random explosions
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( RANDOM_LONG(10,20) + 80  ); // scale * 10
		WRITE_BYTE( 2  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	// random explosions
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( RANDOM_LONG(10,20) + 170  ); // scale * 10
		WRITE_BYTE( 3  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	
	// blast circle
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 100 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture2 );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 99 ); // life
		WRITE_BYTE( 220 );  // width
		WRITE_BYTE( 124 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 192 );   // r, g, b
		WRITE_BYTE( 200 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();
	
	// blast circle
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 40 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture2 );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 99 ); // life
		WRITE_BYTE( 250 );  // width
		WRITE_BYTE( 164 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 160 );   // r, g, b
		WRITE_BYTE( 1 );   // r, g, b
		WRITE_BYTE( 200 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();

	//big lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 64 );		// radius * 0.1
		WRITE_BYTE( 64 );		// r
		WRITE_BYTE( 32 );		// g
		WRITE_BYTE( 1 );		// b
		WRITE_BYTE( 255 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
	
	
}

//edited in 1.26
void    CRadiation:: Explode()
{	
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(3,11), 512, CLASS_NONE, DMG_RADIATION  );
	// lots of smoke
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -16, 16 ) );
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -16, 16 ) );
		WRITE_COORD( pev->origin.z - 8 );
		WRITE_SHORT( g_sModelIndexSmoke );
		WRITE_BYTE( 12 ); // scale * 10
		WRITE_BYTE( 9 ); // framerate
	MESSAGE_END();
		
	//lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 34 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 44 );		// g
		WRITE_BYTE( 0 );		// b
		WRITE_BYTE( 15 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
	
	pev->nextthink = gpGlobals->time + 1.4;
	SetThink(MoveThink);
}


void    CRadiation :: MoveThink( )
{
	Explode();

	if (gpGlobals->time >= m_flDie10)
		{
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 204, 204, CLASS_NONE, DMG_GENERIC  );
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -16, 16 ) );
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -16, 16 ) );
			WRITE_COORD( pev->origin.z - 32 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 55 ); // scale * 10
			WRITE_BYTE( 1 ); // framerate
		MESSAGE_END();

		pev->takedamage = DAMAGE_NO;
		SetThink( SUB_Remove );
		}
}


//Gauss cannon 1.31 [3rd]
class   CGCannon : public CBaseEntity
{
        public:

        void    	Spawn           	   ( void );
        void    	EXPORT MoveThink       ( void );
		void 		EXPORT MoveTouch	   ( CBaseEntity *pOther );
	
		short		m_Sprite;
		short		m_SpriteExp;
		short		m_iSpriteTexture;
		int         m_timer;
		int 		m_iBalls;
};
LINK_ENTITY_TO_CLASS( virtual_hull, CGCannon );

//Gauss particle 1.31 [2nd]
class   CGParticle : public CBaseEntity
{

        public:
		
        void    	Spawn           	   ( void );
		void    	EXPORT MoveThink       ( void );
		short		m_Sprite;
		
};
LINK_ENTITY_TO_CLASS( env_beamtrail, CGParticle );

//Gauss particle 2 1.31 [2nd]
class   CGParticle2 : public CBaseEntity
{

        public:
		
        void    	Spawn           	   ( void );
		short		m_Sprite;
		
};
LINK_ENTITY_TO_CLASS( item_nvg, CGParticle2 );

/////////////////////////////
/////////////////////////////
/////////////////////////////
//Gauss Cannon 3rd update f-e

void CGCannon::Spawn( void )
{
	SET_MODEL( ENT(pev), "sprites/yelflare1.spr" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, -4), Vector(4, 4, 4) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	m_timer = 0;
	
	pev->avelocity.z = -500;
	
	pev->rendermode = kRenderTransAdd; //kRenderTransAlpha
	pev->renderamt = 200;
	
	pev->classname = MAKE_STRING( "weapon_gauss" );
	
	m_Sprite    = PRECACHE_MODEL( "sprites/richo2.spr" ); //explode

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/zbeam1.spr" ); //trails
	
	m_iBalls = PRECACHE_MODEL( "sprites/nhth1.spr" );
	
	//spawn sounds
	switch(RANDOM_LONG(0,3))
		{
		case 0: 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);
		break;
		case 1: 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch3.wav", 1.0, ATTN_NORM);
		break;
		case 2: 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch4.wav", 1.0, ATTN_NORM);
		break;
		case 3: 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/ar2_altfire1.wav", 1.0, ATTN_NORM);
		break;
		}
	
	//loop
	//EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/hold_loop.wav", 1.00, ATTN_NORM, 1.0, 100 );
	

	SetTouch( MoveTouch );
	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 0.1; 
}

void CGCannon::MoveThink( )
{


	m_timer += 1;
	if (m_timer >= 40)
	{
		SetTouch( NULL );
		UTIL_Remove( this );
	}
	
	//trails
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( entindex() );	// entity
		WRITE_SHORT( m_iSpriteTexture );	// model
		WRITE_BYTE( 9 ); // life
		WRITE_BYTE( 10 );  // width
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 160 );	// brightness
	MESSAGE_END();
	
	Create( "env_beamtrail", pev->origin, pev->angles, pev->owner );

	pev->nextthink = gpGlobals->time + 0.1; 
}

void CGCannon::MoveTouch( CBaseEntity *pOther )
{
	TraceResult TResult;
	Vector      StartPosition;
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;
	

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );

	// Pull out of the wall a bit
	if ( TResult.flFraction != 1.0 )
	{
		pev->origin = TResult.vecEndPos + (TResult.vecPlaneNormal * 25.6);
	}

	if( TResult.fAllSolid ) return;

	// animated sprite
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( m_Sprite );
		WRITE_BYTE( 70 ); // scale
		WRITE_BYTE( 172 ); // brightness
	MESSAGE_END();

	//lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 24 );		// radius * 0.1
		WRITE_BYTE( 150 );		// r
		WRITE_BYTE( 200 );		// g
		WRITE_BYTE( 200 );		// b
		WRITE_BYTE( 128 );		// time * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );
	
	//play sound
	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM); //play sound
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM); //play sound


	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, pev->dmg*2 )) != NULL)
	{
		if (pOther->pev->takedamage && (FVisible( pOther )))
		{
		if ((pOther->edict() != pev->owner) && pOther->pev->takedamage && (pOther->edict() != edict()) && pOther->pev->health >= 3) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				//pOther->TakeDamage(pev, VARS( pev->owner ), pev->dmg/5, DMG_BLAST);
				Create( "item_nvg", pOther->pev->origin, pev->angles, pev->owner );
				
				// balls
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITETRAIL );
					WRITE_COORD( pOther->pev->origin.x );
					WRITE_COORD( pOther->pev->origin.y );
					WRITE_COORD( pOther->pev->origin.z );
					WRITE_COORD( pOther->pev->origin.x );
					WRITE_COORD( pOther->pev->origin.y );
					WRITE_COORD( pOther->pev->origin.z  );
					WRITE_SHORT( m_iBalls );		// model
					WRITE_BYTE( 9  );				// count
					WRITE_BYTE( 1 );				// life * 10
					WRITE_BYTE( RANDOM_LONG( 1, 3 ) );				// size * 10
					WRITE_BYTE( 90 );				// amplitude * 0.1
					WRITE_BYTE( 2 );				// speed * 100
				MESSAGE_END();
			}
		}
		
	}
		
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*3.25, CLASS_NONE, DMG_MORTAR  );
				
	// if (pOther->pev->takedamage != NULL)
	// {
		// UTIL_ScreenShake( pOther->pev->origin, 24.0, 174.5, 0.5, 2 );
		// pOther->TakeDamage(pev, VARS( pev->owner ), pev->dmg, DMG_MORTAR);	
	// }
	STOP_SOUND( ENT(pev), CHAN_VOICE, "zxc/hold_loop.wav" );
	SetTouch( NULL );
	UTIL_Remove( this );

}

/////////////
/////particle
/////////////


void CGParticle::Spawn( )
{
	SET_MODEL( ENT(pev), "sprites/shrinkf.spr" );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	
	pev->scale = 0.5;
	
	pev->rendermode = kRenderTransAdd; //kRenderTransAlpha
	pev->renderamt = 120;

	
	//STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
	
	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 0.1; 
}

void CGParticle::MoveThink( )
{
	pev->scale -= 0.04;
	pev->renderamt -= 7;
	if (pev->scale <= 0.03)
		UTIL_Remove( this );
		
	pev->nextthink = gpGlobals->time + 0.05; 
}



////////////////////
/////particle 2/////
////////////////////


void CGParticle2::Spawn( )
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->classname = MAKE_STRING( "weapon_gauss" );
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	pev->scale = 1.6;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 90;
	m_Sprite = PRECACHE_MODEL( "sprites/zbeam4.spr" );
	CBaseEntity *pEntity = NULL;

	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 462 )) != NULL)
	{
		if (pEntity->pev->takedamage && (FVisible( pEntity )))
		{
			if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict()) && pEntity->pev->health >= 3) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				//::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(15,49), 256, CLASS_NONE, DMG_MORTAR  );
				//shock ray
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMPOINTS );
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
					WRITE_COORD( pEntity->pev->origin.y );
					WRITE_COORD( pEntity->pev->origin.z );
					WRITE_SHORT( m_Sprite ); //sprite
					WRITE_BYTE( 1 ); // Starting frame
					WRITE_BYTE( 0  ); // framerate * 0.1
					WRITE_BYTE( 4 ); // life * 0.1
					WRITE_BYTE( 40 ); // width
					WRITE_BYTE( 4 ); // noise
					WRITE_BYTE( 250 ); // color r,g,b
					WRITE_BYTE( 250 ); // color r,g,b
					WRITE_BYTE( 55 ); // color r,g,b
					WRITE_BYTE( 128 ); // brightness
					WRITE_BYTE( 256 ); // scroll speed
				MESSAGE_END();
				
				UTIL_ScreenShake( pEntity->pev->origin, 12.0, 90.5, 0.3, 1 );
				pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(21,39), DMG_MORTAR);	
			}
		}
	}
	
	UTIL_Remove( this );
}
