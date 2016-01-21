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
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "decals.h"
#include "gamerules.h"
#include "game.h"

extern float g_flWeaponCheat;



//class uranium bullet
class   CU : public CGrenade
{
        public:
        void    Spawn              ( );
        void    Precache           ( );
		void 	EXPORT Update	   ( );
        void 	EXPORT Hit         ( CBaseEntity* );
        void    Explode            ( TraceResult*, int);
        static  CU* Create( Vector, Vector, CBaseEntity* );
		
		private:
        int     BeamSprite;
		int 	m_iSpriteTexture;
		int		iSquidSpitSprite;
		unsigned short	m_LaserSprite;
		int m_flDie;
};



// weapon_vulcan
// class vulcan
class   CVulcan : public CGrenade
{
        public:
        void    Spawn				( );
		void 	Precache			( );
        void 	EXPORT MoveThink	( );
		void 	EXPORT MoveTouch	( CBaseEntity *pOther );
		
		private:
		int 	m_flDie;
		int 	m_flDie2;
		int     BeamSprite;
		int     BeamSprite2;
		int 	m_iSpriteTexture;
		unsigned short	m_LaserSprite;
};



// halo_base
// class halo
class   CHalo : public CGrenade
{
        public:
        void    Spawn				( );
		void 	Precache			( );
        void 	EXPORT MoveThink	( );
		void 	EXPORT MoveTouch	( CBaseEntity *pOther );
		
		private:
		int 	m_flDie;
		int 	m_iSpriteTexture;
		int		iSquidSpitSprite;
};


enum python_e {
	PYTHON_IDLE1 = 0,
	PYTHON_FIDGET,
	PYTHON_FIRE1,
	PYTHON_RELOAD,
	PYTHON_HOLSTER,
	PYTHON_DRAW,
	PYTHON_IDLE2,
	PYTHON_IDLE3
};

LINK_ENTITY_TO_CLASS( weapon_python, CPython );
LINK_ENTITY_TO_CLASS( weapon_357, CPython );
LINK_ENTITY_TO_CLASS( weapon_vulcan, CVulcan );
LINK_ENTITY_TO_CLASS( halo_base, CHalo );


int CPython::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = PYTHON_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_PYTHON;
	p->iWeight = PYTHON_WEIGHT;

	return 1;
}

int CPython::AddToPlayer( CBasePlayer *pPlayer )
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

void CPython::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_357"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_PYTHON;
	SET_MODEL(ENT(pev), "models/w_357.mdl");

	m_iDefaultAmmo = PYTHON_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CPython::Precache( void )
{
	PRECACHE_MODEL("models/v_357.mdl");
	PRECACHE_MODEL("models/w_357.mdl");
	PRECACHE_MODEL("models/p_357.mdl");
	PRECACHE_MODEL("models/w_antidote.mdl");
	PRECACHE_MODEL("models/w_antidotet.mdl");
	//PRECACHE_MODEL("models/xdm_rune.mdl");
	PRECACHE_MODEL("models/w_rad.mdl");
	PRECACHE_MODEL("models/w_radt.mdl");
	
	
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");
	
	

	PRECACHE_MODEL("models/w_357ammobox.mdl");
	
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/357_reload1.wav");
	PRECACHE_SOUND ("weapons/357_cock1.wav");
	PRECACHE_SOUND ("weapons/357_shot1.wav");
	PRECACHE_SOUND ("weapons/357_shot2.wav");
	PRECACHE_SOUND ("zxc/bemlsr2.wav");
	PRECACHE_SOUND ("buttons/spark1.wav");
	PRECACHE_SOUND ("buttons/spark2.wav");
	PRECACHE_SOUND ("buttons/spark3.wav");
	PRECACHE_SOUND ("buttons/spark4.wav");
	PRECACHE_SOUND ("buttons/spark5.wav");
	PRECACHE_SOUND ("buttons/spark6.wav");
	
	PRECACHE_MODEL("sprites/mommaspit.spr"); // client side spittle.
	PRECACHE_MODEL("sprites/bluejet1.spr"); // reaction
	PRECACHE_MODEL("sprites/fexplo1.spr");  //expld
	PRECACHE_SOUND ("zxc/lrgexpl2.wav");
	
	PRECACHE_MODEL("sprites/xbeam4.spr");
	
	
	m_usFirePython = PRECACHE_EVENT( 1, "events/python.sc" );
}

BOOL CPython::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 280 );
#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		// enable laser sight geometry.
		pev->body = 1;
	}
	else
	{
		pev->body = 0;
	}

	return DefaultDeploy( "models/v_357.mdl", "models/p_357.mdl", PYTHON_DRAW, "python", UseDecrement(), pev->body );
}


void CPython::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.

	if ( m_fInZoom )
	{
		SecondaryAttack();
	}

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	SendWeaponAnim( PYTHON_HOLSTER );
}


void CPython::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		if (!m_fFireOnEmpty)
			Reload( );
		else
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );


	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

    int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFirePython, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = 0.75;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}



void CPython::SecondaryAttack( void )
{
//removed old code, now create new weapon
	if (m_iClip == 6 && allowmonsters5.value != 0)
		{
		#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); // player "shoot" animation
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_shot2.wav", 0.8, ATTN_NORM); //play sound
			m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
			m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
			m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
		
			UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
			Vector GunPosition = m_pPlayer->GetGunPosition( );
			GunPosition = GunPosition + gpGlobals->v_forward * 0;
			GunPosition = GunPosition + gpGlobals->v_right   * 7;
			GunPosition = GunPosition + gpGlobals->v_up      * 0;
			CU* Beam = CU :: Create( GunPosition, m_pPlayer->pev->v_angle, m_pPlayer ); //create think
			Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right; //shot pos
			Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up; //shot pos
			m_iClip-=6; //-6 bullets


			float flZVel = m_pPlayer->pev->velocity.z; 
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 1500; 
		#endif
	}
else //here old code
	{
		if ( m_pPlayer->pev->fov != 0 )
		{
			m_fInZoom = FALSE;
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
			m_flNextSecondaryAttack = 0.3;
			//return;
		}
		else if ( m_pPlayer->pev->fov != 40 )
		{
			m_fInZoom = TRUE;
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 40;
			m_flNextSecondaryAttack = 0.3;
			//return;
		}
		PlayEmptySound( );
		m_flNextSecondaryAttack = 0.3;
		return;
	}

	m_flNextSecondaryAttack = 0.5;
}


//////v 1.33 new attack
void CPython::ThirdAttack( void )
{
	// if ( g_flWeaponCheat != 0) //dont shot 
	// {
		// MESSAGE_BEGIN( MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev) );
			// WRITE_STRING( "Attack disabled, while sv_cheats is on." );
		// MESSAGE_END();
		// EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "buttons/bell1.wav", 0.75, ATTN_NORM, 1.0, 102 );
		// m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 3; //delay
		// return;
	// }

	Vector vecSrc = m_pPlayer->pev->origin;
	Vector vecThrow = gpGlobals->v_forward * 768; //512
	
	if (m_iClip == 6 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 12 && allowmonsters5.value != 0)
		{
		#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); // player "shoot" animation
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/bemlsr2.wav", 0.8, ATTN_NORM); //play sound
			m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
			m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
			m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

			CBaseEntity *pSatchel = Create( "weapon_vulcan", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSatchel->pev->velocity = vecThrow;
			pSatchel->pev->avelocity.y = 80;
			pSatchel->pev->avelocity.x = RANDOM_LONG(-20,100);

			if ( g_flWeaponCheat != 0)
			{
				// - all bullets per shot
				m_iClip = 0; //-6 bullets
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
			}
			else
			{
				// - 18 bullets per shot
				m_iClip-=6; //-6 bullets
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 12; //-12 ammo
			}
			

		#endif
		}
	m_flNextPrimaryAttack = 1.0;
	m_flNextSecondaryAttack = 1.0;
	
}


//////v 1.33 attack
void CPython::FourthAttack( void )
{
	Vector vecSrc = m_pPlayer->pev->origin;
	Vector vecThrow = gpGlobals->v_forward * 300;
	
	if (m_iClip == 6 && allowmonsters5.value != 0)
		{
		#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); // player "shoot" animation
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/bemlsr2.wav", 0.8, ATTN_NORM); //play sound
			m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
			m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
			m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

			CBaseEntity *pSatchel = Create( "halo_base", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSatchel->pev->velocity = vecThrow;
			m_iClip-=6; //-6 bullets
			//m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 6; //-6 ammo
			
			float flZVel = m_pPlayer->pev->velocity.z; 
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 700; 
		#endif
		}
	m_flNextPrimaryAttack = 1.0;
	m_flNextSecondaryAttack = 1.0;
}





void CPython::Reload( void )
{
	if ( m_pPlayer->ammo_357 <= 0 )
		return;

	if ( m_pPlayer->pev->fov != 0 )
	{
		m_fInZoom = FALSE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
	}

	int bUseScope = FALSE;
#ifdef CLIENT_DLL
	bUseScope = bIsMultiplayer();
#else
	bUseScope = g_pGameRules->IsMultiplayer();
#endif

	if (DefaultReload( 6, PYTHON_RELOAD, 2.0, bUseScope ))
	{
		m_flSoundDelay = 1.5;
	}
}


void CPython::WeaponIdle( void )
{
	ResetEmptySound( );

	//m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	// ALERT( at_console, "%.2f\n", gpGlobals->time - m_flSoundDelay );
	if (m_flSoundDelay != 0 && m_flSoundDelay <= UTIL_WeaponTimeBase() )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_reload1.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);
		m_flSoundDelay = 0;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	if (flRand <= 0.5)
	{
		iAnim = PYTHON_IDLE1;
		m_flTimeWeaponIdle = (70.0/30.0);
	}
	else if (flRand <= 0.7)
	{
		iAnim = PYTHON_IDLE2;
		m_flTimeWeaponIdle = (60.0/30.0);
	}
	else if (flRand <= 0.9)
	{
		iAnim = PYTHON_IDLE3;
		m_flTimeWeaponIdle = (88.0/30.0);
	}
	else
	{
		iAnim = PYTHON_FIDGET;
		m_flTimeWeaponIdle = (170.0/30.0);
	}
	
	int bUseScope = FALSE;
#ifdef CLIENT_DLL
	bUseScope = bIsMultiplayer();
#else
	bUseScope = g_pGameRules->IsMultiplayer();
#endif
	
	SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0, bUseScope );
}



class CPythonAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_357ammobox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_357ammobox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_357BOX_GIVE, "357", _357_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_357, CPythonAmmo );


#endif









//////////////////////////new weapon///////////////////////////////////
void    CU :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_antidote.mdl" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(2,2,2), Vector(2,2,2) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_357" );
	SetThink( Update );
	SetTouch( Hit );
	pev->velocity = gpGlobals->v_forward * 125;
	pev->nextthink = gpGlobals->time + 0.1;
	pev->effects = EF_MUZZLEFLASH;
	m_flDie = gpGlobals->time + 16;

}

void    CU :: Precache( )
{
	BeamSprite = PRECACHE_MODEL( "sprites/smoke.spr" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	iSquidSpitSprite = PRECACHE_MODEL("sprites/mommaspit.spr");// client side spittle.
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
}

void    CU :: Hit( CBaseEntity* Target )
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
	Explode( &TResult, DMG_CRUSH|DMG_MORTAR );
}

void    CU :: Explode( TraceResult* TResult, int DamageType )
{

	// Pull out of the wall a bit
	if ( TResult->flFraction != 1.0 )
	{
		pev->origin = TResult->vecEndPos + (TResult->vecPlaneNormal * 25.6);
	}
	
    if( TResult->fAllSolid ) return;

	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_FLOAT(12,300);	// Pick a random direction
	vecEnd.y = RANDOM_FLOAT(12,300);
	vecEnd.z = RANDOM_FLOAT(12,300);
	vecEnd = pev->origin + vecEnd.Normalize() * 2048;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	
	//lightings 1
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMPOINTS );
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );
			WRITE_SHORT( m_LaserSprite );
			WRITE_BYTE( 0 ); // Starting frame
			WRITE_BYTE( 16  ); // framerate * 0.1
			WRITE_BYTE( 3 ); // life * 0.1
			WRITE_BYTE( 8 ); // width
			WRITE_BYTE( 128 ); // noise
			WRITE_BYTE( 128 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 175 ); // brightness
			WRITE_BYTE( 8 ); // scroll speed
	MESSAGE_END();
	//lightings 2
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMPOINTS );
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );
			WRITE_SHORT( m_LaserSprite );
			WRITE_BYTE( 0 ); // Starting frame
			WRITE_BYTE( 16  ); // framerate * 0.1
			WRITE_BYTE( 4 ); // life * 0.1
			WRITE_BYTE( 5 ); // width
			WRITE_BYTE( 255 ); // noise
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 175 ); // brightness
			WRITE_BYTE( 12 ); // scroll speed
	MESSAGE_END();
	//lightings 3
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMPOINTS );
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );
			WRITE_SHORT( m_LaserSprite );
			WRITE_BYTE( 0 ); // Starting frame
			WRITE_BYTE( 24  ); // framerate * 0.1
			WRITE_BYTE( 5 ); // life * 0.1
			WRITE_BYTE( 3 ); // width
			WRITE_BYTE( 255 ); // noise
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 128 ); // color r,g,b
			WRITE_BYTE( 128 ); // color r,g,b
			WRITE_BYTE( 175 ); // brightness
			WRITE_BYTE( 16 ); // scroll speed
	MESSAGE_END();

	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 125, 400, CLASS_NONE, DMG_MORTAR|DMG_BULLET  ); //DMG
		
	UTIL_DecalTrace( TResult, DECAL_SCORCH2 );
		
	//explode
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( 10 + 3  ); // scale * 10
		WRITE_BYTE( 16  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
		
	//beam
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMDISK );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x+1200);
		WRITE_COORD( pev->origin.y + 1200);
		WRITE_COORD( pev->origin.z + 2000 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 16 ); // framerate
		WRITE_BYTE( 2 ); // life
		WRITE_BYTE( 500 );  // width
		WRITE_BYTE( 64 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 ); // brightness
		WRITE_BYTE( 8 );		// speed
	MESSAGE_END();
	SUB_Remove( );
}

CU* CU :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CU* Beam = GetClassPtr( (CU*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CU :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}


void    CU :: Update( void )
{
//fixed stuck in sky
	
if (gpGlobals->time >= m_flDie) //time out
	{
		//explode
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 10 + 3  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 195, 500, CLASS_NONE, DMG_MORTAR|DMG_BULLET  ); //DMG
		SUB_Remove( );
	}
	
	Vector	vecSpitDir;
	pev->angles = UTIL_VecToAngles (pev->velocity);
	pev->angles.x -= 90;
	
	vecSpitDir = ( (pev->angles).Normalize() );
	vecSpitDir.x = 0;
	vecSpitDir.y = 0;
	vecSpitDir.z = 0;
	

	// spew the spittle temporary ents.
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE_SPRAY );
		WRITE_COORD( pev->origin.x);	// pos
		WRITE_COORD( pev->origin.y);	
		WRITE_COORD( pev->origin.z);	
		WRITE_COORD( vecSpitDir.x);	// direction
		WRITE_COORD( vecSpitDir.y);	
		WRITE_COORD( vecSpitDir.z);	
		WRITE_SHORT( iSquidSpitSprite );	// model
		WRITE_BYTE ( 2 );			// count
		WRITE_BYTE ( 210 );			// speed
		WRITE_BYTE ( 8 );			// noise ( client will divide by 100 )
	MESSAGE_END();

	pev->nextthink = gpGlobals->time + 0.11;
}











///////////////////////////
///////////////////////////
///////////////////////////


void    CVulcan :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_rad.mdl" );
	
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
	//fix positions for this model
	UTIL_SetOrigin( pev, Vector(pev->origin.x, pev->origin.y,pev->origin.z));
	pev->classname = MAKE_STRING( "weapon_357" );
	m_flDie = gpGlobals->time + 16.0;
	pev->nextthink = gpGlobals->time + 0.1;
	pev->gravity			= 0.35;
	pev->friction			= 0.01;
	
	BeamSprite = PRECACHE_MODEL( "sprites/fexplo1.spr" );
	BeamSprite2 = PRECACHE_MODEL( "sprites/bluejet1.spr" );

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 16 ); // life
		WRITE_BYTE( 2 ); // width
		WRITE_BYTE( 108 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // brightness
	MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
	
	
	
	SetThink( MoveThink );
	SetTouch( MoveTouch );
}

void CVulcan :: Precache( void )
{

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );

}



void    CVulcan :: MoveThink( )
{
	CBaseEntity *pEntity = NULL;
	Vector direction = Vector(0,0,1); 
	
	
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = pev->origin.x + RANDOM_LONG(-211,211);	// Pick a random direction
	vecEnd.y = pev->origin.y + RANDOM_LONG(-211,211);
	vecEnd.z = pev->origin.z + RANDOM_LONG(0,64);
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	UTIL_ParticleEffect ( vecEnd, g_vecZero, 92, 25 );
		
	// first begin, beware lightings 5 sec
	if (gpGlobals->time <= m_flDie - 11)
		{
			// Make a lightning strike

			vecEnd = pev->origin + vecEnd.Normalize() * RANDOM_LONG(16,128);

			// lightings
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z+10);
				WRITE_COORD( tr.vecEndPos.x );
				WRITE_COORD( tr.vecEndPos.y );
				WRITE_COORD( tr.vecEndPos.z );
				WRITE_SHORT( m_iSpriteTexture );
				WRITE_BYTE( 0 ); // Starting frame
				WRITE_BYTE( 16  ); // framerate * 0.1
				WRITE_BYTE( 1 ); // life * 0.1
				WRITE_BYTE( 8 ); // width
				WRITE_BYTE( 64 ); // noise
				WRITE_BYTE( 50 ); // color r,g,b
				WRITE_BYTE( 100 ); // color r,g,b
				WRITE_BYTE( RANDOM_LONG(128,255) ); // color r,g,b
				WRITE_BYTE( 175 ); // brightness
				WRITE_BYTE( 8 ); // scroll speed
			MESSAGE_END();
			
			
			
		}
		
	// second is hurting 
	if (gpGlobals->time >= m_flDie - 11)
		{
			//spark effects
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_STREAK_SPLASH );
				WRITE_COORD( vecEnd.x );		// origin
				WRITE_COORD( vecEnd.y );
				WRITE_COORD( vecEnd.z );
				WRITE_COORD( direction.x );	//// direction
				WRITE_COORD( direction.y );
				WRITE_COORD( direction.z );
				WRITE_BYTE( 255 );	// Streak color 6
				WRITE_SHORT( 32 );	// count
				WRITE_SHORT( 1024 );
				WRITE_SHORT( 1600 );	// Random velocity modifier
			MESSAGE_END();
			
			::RadiusDamage( vecEnd, pev, VARS( pev->owner ), 100, 200, CLASS_NONE, DMG_GENERIC  ); //blast
			
			EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/lrgexpl2.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,110) );
				
			// lots of expl
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( vecEnd.x );		// origin
				WRITE_COORD( vecEnd.y );
				WRITE_COORD( vecEnd.z );
				WRITE_SHORT( BeamSprite );
				WRITE_BYTE( 64 ); // scale * 10
				WRITE_BYTE( 200 ); // brightness
			MESSAGE_END( );
			// lots of expl 2
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( vecEnd.x );		// origin
				WRITE_COORD( vecEnd.y );
				WRITE_COORD( vecEnd.z );
				WRITE_SHORT( BeamSprite2 );
				WRITE_BYTE( 64 ); // scale * 10
				WRITE_BYTE( 200 ); // brightness
			MESSAGE_END( );
	
		}

	if (gpGlobals->time >= m_flDie - 4) //full explode and self destroy
		{

			//spark effects
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_STREAK_SPLASH );
				WRITE_COORD( pev->origin.x );		// origin
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_COORD( direction.x );	//// direction
				WRITE_COORD( direction.y );
				WRITE_COORD( direction.z );
				WRITE_BYTE( 255 );	// Streak color 6
				WRITE_SHORT( 64 );	// count
				WRITE_SHORT( 1024 );
				WRITE_SHORT( 1600 );	// Random velocity modifier
			MESSAGE_END();


			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 200, CLASS_NONE, DMG_GENERIC  ); //end blast
			


			//lights
			Vector vecSrc = pev->origin;
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
				WRITE_BYTE(TE_DLIGHT);
				WRITE_COORD(vecSrc.x);	// X
				WRITE_COORD(vecSrc.y);	// Y
				WRITE_COORD(vecSrc.z);	// Z
				WRITE_BYTE( 32 );		// radius * 0.1
				WRITE_BYTE( 112 );		// r
				WRITE_BYTE( 112 );		// g
				WRITE_BYTE( 112 );		// b
				WRITE_BYTE( 128 );		// life * 10
				WRITE_BYTE( 0 );		// decay * 0.1
			MESSAGE_END( );
			

			// random explosions
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION );		// This just makes a dynamic light now
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z);
				WRITE_SHORT( g_sModelIndexFireball );
				WRITE_BYTE( 16 ); // scale * 10
				WRITE_BYTE( 16 ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			
			SetTouch( NULL );
			UTIL_Remove( this );
		}
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.05, 0.30); //dynamic update

}


void  CVulcan::MoveTouch( CBaseEntity *pOther )
{
	
	
	//play sounds
	switch(RANDOM_LONG(0,2))
	{
	case 0: 
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/grenade_hit1.wav", 1.0, ATTN_NORM); //play sound
	break;
	case 1: 
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/grenade_hit2.wav", 1.0, ATTN_NORM); //play sound
	break;
	case 2: 
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/grenade_hit3.wav", 1.0, ATTN_NORM); //play sound
	break;
	}
	
	pev->velocity.z += 24; //jump
	
	//touch explode
	if ( pOther->pev->solid != SOLID_BSP) 
	{
		m_flDie = gpGlobals->time - 20;
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 256, 128, CLASS_NONE, DMG_GENERIC  ); //end blast
	}
		
	pev->velocity = pev->velocity*0.45;



		
}
///////////////////////////
///////////////////////////
///////////////////////////


void    CHalo :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_rad.mdl" );
	
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
	UTIL_SetOrigin( pev, Vector(pev->origin.x, pev->origin.y,pev->origin.z));
	pev->classname = MAKE_STRING( "weapon_357" );
	m_flDie = gpGlobals->time + 16.0;
	pev->nextthink = gpGlobals->time + 0.1;
	pev->gravity			= 0.15;
	pev->friction			= 0.01;
	
	pev->angles = UTIL_VecToAngles (pev->velocity);
	pev->angles.x -= 90;
	
	SetThink( MoveThink );
	SetTouch( MoveTouch );
}

void CHalo :: Precache( void )
{

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	iSquidSpitSprite = PRECACHE_MODEL("sprites/xbeam4.spr");// client side spittle.

	//foolow trails
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( entindex() );
		WRITE_SHORT( iSquidSpitSprite ); //start
		WRITE_BYTE( 12 );		// life
		WRITE_BYTE( 3 );		// scale
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
	MESSAGE_END();
	
}



void    CHalo :: MoveThink( )
{
	
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_LONG(-360,360);	// Pick a random direction
	vecEnd.y = RANDOM_LONG(-360,360);
	vecEnd.z = RANDOM_LONG(-360,360);
	vecEnd = pev->origin + vecEnd.Normalize() * 80;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	/* 
	
	// spew the spittle temporary ents.
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE_SPRAY );
		WRITE_COORD( pev->origin.x);	// pos
		WRITE_COORD( pev->origin.y);	
		WRITE_COORD( pev->origin.z);	
		WRITE_COORD( vecEnd.x);	// direction
		WRITE_COORD( vecEnd.y);	
		WRITE_COORD( vecEnd.z);	
		WRITE_SHORT( iSquidSpitSprite );	// model
		WRITE_BYTE ( 1 );			// count
		WRITE_BYTE ( 256 );			// speed
		WRITE_BYTE ( 4 );			// noise ( client will divide by 100 )
	MESSAGE_END();
 */

/* 	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BREAKMODEL);
		// position
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		// size
		WRITE_COORD( pev->size.x);
		WRITE_COORD( pev->size.y);
		WRITE_COORD( pev->size.z);
		// velocity
		WRITE_COORD( 0 ); 
		WRITE_COORD( -8 );
		WRITE_COORD( 0 );
		// randomization
		WRITE_BYTE( 4 ); 
		// Model
		WRITE_SHORT( iSquidSpitSprite );	//model id#
		// # of shards
		WRITE_BYTE( 1 );	// let client decide
		// duration
		WRITE_BYTE( 2500 );// 2.5 seconds
		// flags
		WRITE_BYTE( BREAK_GLASS );
	MESSAGE_END(); */

		// MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			// WRITE_BYTE( TE_BEAMENTS );
			// WRITE_SHORT( entindex() );
			// WRITE_SHORT( entindex() );
			// WRITE_SHORT( iSquidSpitSprite );
			// WRITE_BYTE( 0 ); // framestart
			// WRITE_BYTE( 0 ); // framerate
			// WRITE_BYTE( 255 ); // life
			// WRITE_BYTE( 800 );  // width
			// WRITE_BYTE( 80 );   // noise
			// WRITE_BYTE( 255 );   // r, g, b
			// WRITE_BYTE( 128 );   // r, g, b
			// WRITE_BYTE( 64 );   // r, g, b
			// WRITE_BYTE( 155 );	// brightness
			// WRITE_BYTE( 130 );		// speed
		// MESSAGE_END();
	
	// MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		// WRITE_BYTE( TE_LAVASPLASH );
		// WRITE_COORD( vecEnd.x );
		// WRITE_COORD( vecEnd.y );
		// WRITE_COORD( vecEnd.z );
	// MESSAGE_END();
/* 	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_IMPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_BYTE( RANDOM_LONG(8,16) );	// rad
		WRITE_BYTE( 64 );		// count
		WRITE_BYTE( 12 );		// life
	MESSAGE_END();
	 */
	// MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		// WRITE_BYTE( TE_SPRITETRAIL );
		// WRITE_COORD( pev->origin.x ); //start
		// WRITE_COORD( pev->origin.y );
		// WRITE_COORD( pev->origin.z );
		// WRITE_COORD( pev->origin.x ); //end
		// WRITE_COORD(  pev->origin.y );
		// WRITE_COORD(  pev->origin.z );
		// WRITE_SHORT( g_sModelIndexFireball );
		// WRITE_BYTE( 1 );	// count
		// WRITE_BYTE( 1 );		// life
		// WRITE_BYTE( 3 );		// scale
		// WRITE_BYTE( 40 );		// veloc
		// WRITE_BYTE( 128 );		// rand
	// MESSAGE_END();
	
	// MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		// WRITE_BYTE( TE_FIREFIELD );
		// WRITE_COORD( pev->origin.x ); //start
		// WRITE_COORD( pev->origin.y );
		// WRITE_COORD( pev->origin.z );
		// WRITE_SHORT( 1 );		// life
		// WRITE_SHORT( g_sModelIndexFireball );		// mdi
		// WRITE_BYTE( 6 );		// cnt
		// WRITE_BYTE( 0 );		// fl
		// WRITE_BYTE( 16 );		// drn
	// MESSAGE_END();

	pev->angles = UTIL_VecToAngles (pev->velocity);
	pev->angles.x -= 90;
	
	// hurting 
	if (gpGlobals->time >= m_flDie - 14.75)
		{
			pev->velocity.x = (pev->velocity.x + RANDOM_LONG(-64,64));
			pev->velocity.y = (pev->velocity.y + RANDOM_LONG(-64,64));
			pev->velocity.z = (pev->velocity.z + RANDOM_LONG(-20,20));

			
		
			// lightings
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD( tr.vecEndPos.x );
				WRITE_COORD( tr.vecEndPos.y );
				WRITE_COORD( tr.vecEndPos.z );
				WRITE_SHORT( m_iSpriteTexture );
				WRITE_BYTE( 0 ); // Starting frame
				WRITE_BYTE( 16  ); // framerate * 0.1
				WRITE_BYTE( 1 ); // life * 0.1
				WRITE_BYTE( 8 ); // width
				WRITE_BYTE( 64 ); // noise
				WRITE_BYTE( 10 ); // color r,g,b
				WRITE_BYTE( 50 ); // color r,g,b
				WRITE_BYTE( 200 ); // color r,g,b
				WRITE_BYTE( 215 ); // brightness
				WRITE_BYTE( 8 ); // scroll speed
			MESSAGE_END();

			
			::RadiusDamage( vecEnd, pev, VARS( pev->owner ), 16, 150, CLASS_NONE, DMG_BLAST  ); //end blast
			
			
	
		}

	if (gpGlobals->time >= m_flDie - 4) //full explode and self destroy
		{
	
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_IMPLOSION );
				WRITE_COORD( pev->origin.x );
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_BYTE( 220 );	// rad
				WRITE_BYTE( 128 );		// count
				WRITE_BYTE( 24 );		// life
			MESSAGE_END();
	



			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 360, CLASS_NONE, DMG_CRUSH  ); //end blast
			EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/lrgexpl2.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,110) );

			//lights
			Vector vecSrc = pev->origin;
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
				WRITE_BYTE(TE_DLIGHT);
				WRITE_COORD(vecSrc.x);	// X
				WRITE_COORD(vecSrc.y);	// Y
				WRITE_COORD(vecSrc.z);	// Z
				WRITE_BYTE( 32 );		// radius * 0.1
				WRITE_BYTE( 112 );		// r
				WRITE_BYTE( 112 );		// g
				WRITE_BYTE( 112 );		// b
				WRITE_BYTE( 64 );		// life * 10
				WRITE_BYTE( 8 );		// decay * 0.1
			MESSAGE_END( );
			

			// random explosions
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION );		// This just makes a dynamic light now
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z);
				WRITE_SHORT( g_sModelIndexFireball );
				WRITE_BYTE( 80 ); // scale * 10
				WRITE_BYTE( 16 ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			
			//beam
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_BEAMDISK );
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z);
				WRITE_COORD( pev->origin.x+ 300);
				WRITE_COORD( pev->origin.y + 300);
				WRITE_COORD( pev->origin.z + 1600 ); // reach damage radius over .2 seconds
				WRITE_SHORT( m_iSpriteTexture );
				WRITE_BYTE( 0 ); // startframe
				WRITE_BYTE( 8 ); // framerate
				WRITE_BYTE( 3 ); // life
				WRITE_BYTE( 200 );  // width
				WRITE_BYTE( 64 );   // noise
				WRITE_BYTE( 200 );   // r, g, b
				WRITE_BYTE( 150 );   // r, g, b
				WRITE_BYTE( 100 );   // r, g, b
				WRITE_BYTE( 200 ); // brightness
				WRITE_BYTE( 4 );		// speed
			MESSAGE_END();
			
			SetTouch( NULL );
			UTIL_Remove( this );
		}
		
	pev->nextthink = gpGlobals->time + 0.1; //dynamic update

}


void  CHalo::MoveTouch( CBaseEntity *pOther )
{
	m_flDie = gpGlobals->time - 20;
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 128, CLASS_NONE, DMG_BLAST  ); //end blast
		
}
