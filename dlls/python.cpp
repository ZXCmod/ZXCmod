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




#define BLASTER_BEAM_RANDOMNESS 1
#define BLASTER_OFFSET_FORWARD  0
#define BLASTER_OFFSET_RIGHT    7
#define BLASTER_OFFSET_UP               0
class   CU : public CGrenade
{
        public:
        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
		void    MoveThink2       ( );
        void EXPORT Hit         ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static CU* Create( Vector, Vector, CBaseEntity* );
        int     BeamSprite;
		int m_iSpriteTexture;
		int			   iSquidSpitSprite;
		short		m_LaserSprite;
		int m_flDie;
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

	PRECACHE_MODEL("models/w_357ammobox.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/357_reload1.wav");
	PRECACHE_SOUND ("weapons/357_cock1.wav");
	PRECACHE_SOUND ("weapons/357_shot1.wav");
	PRECACHE_SOUND ("weapons/357_shot2.wav");
	PRECACHE_MODEL("sprites/wallpuff.spr");// client side spittle.
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
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

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
	if (m_iClip == 6)
		{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); // player "shoot" animation
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_shot2.wav", 0.8, ATTN_NORM); //play sound
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
	
        UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
        Vector GunPosition = m_pPlayer->GetGunPosition( );
        GunPosition = GunPosition + gpGlobals->v_forward * BLASTER_OFFSET_FORWARD;
        GunPosition = GunPosition + gpGlobals->v_right   * BLASTER_OFFSET_RIGHT;
        GunPosition = GunPosition + gpGlobals->v_up      * BLASTER_OFFSET_UP;
        CU* Beam = CU :: Create( GunPosition, m_pPlayer->pev->v_angle, m_pPlayer ); //create think

		float RandomX = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS, BLASTER_BEAM_RANDOMNESS );//shot pos
		float RandomY = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS, BLASTER_BEAM_RANDOMNESS );//shot pos
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right * RandomX; //shot pos
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up    * RandomY; //shot pos
		m_iClip-=6; //-6 bullets

#ifndef CLIENT_DLL
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

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

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
        SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
        pev->movetype = MOVETYPE_FLY;
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector(2,2,2), Vector(2,2,2) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "weapon_357" );
        SetThink( MoveThink );
        SetTouch( Hit );
        pev->velocity = gpGlobals->v_forward * 125;
        pev->angles.x = -(pev->angles.x);
        pev->nextthink = gpGlobals->time + 0.1;
		pev->effects = EF_MUZZLEFLASH;
		m_flDie = gpGlobals->time + 16;

}

void    CU :: Precache( )
{
        BeamSprite = PRECACHE_MODEL( "sprites/smoke.spr" );
		m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
		iSquidSpitSprite = PRECACHE_MODEL("sprites/wallpuff.spr");// client side spittle.
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
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_FLOAT(12,300);	// Pick a random direction
	vecEnd.y = RANDOM_FLOAT(12,300);
	vecEnd.z = RANDOM_FLOAT(12,300);
	// vecEnd = vecEnd.Normalize();
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

        if( TResult->fAllSolid ) return;
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
			WRITE_BYTE( TE_BEAMCYLINDER );
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

void    CU :: MoveThink( )
{
//fixed stuck in sky

	
if (gpGlobals->time >= m_flDie) //time out 16 s
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

			Vector	vecSpitOffset;
			Vector	vecSpitDir;
			vecSpitOffset = ( pev->origin );
			
			vecSpitDir = ( pev->angles );
			vecSpitDir.x = 0;
			vecSpitDir.y = 0;
			vecSpitDir.z = 0;

			// spew the spittle temporary ents.
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpitOffset );
				WRITE_BYTE( TE_SPRITE_SPRAY );
				WRITE_COORD( vecSpitOffset.x);	// pos
				WRITE_COORD( vecSpitOffset.y);	
				WRITE_COORD( vecSpitOffset.z);	
				WRITE_COORD( vecSpitDir.x);	// direction
				WRITE_COORD( vecSpitDir.y);	
				WRITE_COORD( vecSpitDir.z);	
				WRITE_SHORT( iSquidSpitSprite );	// model
				WRITE_BYTE ( 1 );			// count
				WRITE_BYTE ( 210 );			// speed
				WRITE_BYTE ( 10 );			// noise ( client will divide by 100 )
			MESSAGE_END();
			
			MoveThink2( );
			
}

void    CU :: MoveThink2( )
{
        SetThink( MoveThink );
        pev->nextthink = gpGlobals->time + 0.1;

}