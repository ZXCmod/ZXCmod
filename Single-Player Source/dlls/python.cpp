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



//class uranium bullet
class   CUraniumBullet : public CGrenade
{
        public:
        void    Spawn              ( );
        void    Precache           ( );
		void 	EXPORT MoveThink	( );
        void 	EXPORT MoveTouch	( CBaseEntity *pOther );
		static  CUraniumBullet* Create( Vector, Vector, CBaseEntity* );

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
	
	
	m_Sprite    = PRECACHE_MODEL( "sprites/explode1.spr" );
	m_SpriteExp = PRECACHE_MODEL( "sprites/mushroom.spr" );
	PRECACHE_SOUND ("zxc/explode3.wav");
	
	PRECACHE_MODEL("sprites/mommaspit.spr"); // client side spittle.
	PRECACHE_MODEL("sprites/bluejet1.spr"); // reaction
	PRECACHE_MODEL("sprites/fexplo1.spr");  //expld
	PRECACHE_SOUND ("zxc/lrgexpl2.wav");
	
	PRECACHE_MODEL("sprites/xbeam4.spr");
	
	
	m_usFirePython = PRECACHE_EVENT( 1, "events/python.sc" );
}

BOOL CPython::Deploy( )
{
	pev->body = 1;
	return DefaultDeploy( "models/v_357.mdl", "models/p_357.mdl", PYTHON_DRAW, "python", UseDecrement(), pev->body );
}


void CPython::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;

	if ( m_fInZoom )
		SecondaryAttack();

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
	SendWeaponAnim( PYTHON_HOLSTER );
}


void CPython::PrimaryAttack()
{


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

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;
	
	Vector vecDir = gpGlobals->v_forward;
	
	BOOL	is_body_hit = false;
	TraceResult	tr;	
	UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	CBaseEntity *pOther = NULL;
	
	while ((pOther = UTIL_FindEntityInSphere( pOther, tr.vecEndPos, 64 )) != NULL)
	{
		if ( pOther->pev->solid != SOLID_BSP )
		{
			if ( pOther->pev->flags & (FL_MONSTER|FL_CLIENT) )  // body hit
			{
				is_body_hit = true;
				vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_2DEGREES, 8192, 7, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed ); // just 7 dmg
				
				// animated sprite
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
					WRITE_BYTE( TE_SPRITE );
					WRITE_COORD( tr.vecEndPos.x );
					WRITE_COORD( tr.vecEndPos.y );
					WRITE_COORD( tr.vecEndPos.z );
					WRITE_SHORT( m_Sprite );
					WRITE_BYTE( 60 ); // scale
					WRITE_BYTE( 172 ); // brightness
				MESSAGE_END();
				
				if ( (pev->flags & FL_ONGROUND) )
				{
					// animated sprite
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
						WRITE_BYTE( TE_SPRITE );
						WRITE_COORD( tr.vecEndPos.x );
						WRITE_COORD( tr.vecEndPos.y );
						WRITE_COORD( tr.vecEndPos.z );
						WRITE_SHORT( m_SpriteExp );
						WRITE_BYTE( 60 ); // scale
						WRITE_BYTE( 172 ); // brightness
					MESSAGE_END();
				}
				
				//lights
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
					WRITE_BYTE(TE_DLIGHT);
					WRITE_COORD(tr.vecEndPos.x);	// X
					WRITE_COORD(tr.vecEndPos.y);	// Y
					WRITE_COORD(tr.vecEndPos.z);	// Z
					WRITE_BYTE( 24 );		// radius * 0.1
					WRITE_BYTE( 250 );		// r
					WRITE_BYTE( 250 );		// g
					WRITE_BYTE( 150 );		// b
					WRITE_BYTE( 128 );		// time * 10
					WRITE_BYTE( 16 );		// decay * 0.1
				MESSAGE_END( );
				EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); 
				::RadiusDamage( tr.vecEndPos, pev, VARS( pev->owner ), 63, 175, CLASS_NONE, DMG_BULLET  ); 
				m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - (gpGlobals->v_forward * 300);
			}
		}
	}
	if ( is_body_hit == false )
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_2DEGREES, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		UTIL_Sparks( tr.vecEndPos );
	}
	
	
	
	
	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usFirePython, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	m_flNextPrimaryAttack = 0.75;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
		


}



void CPython::SecondaryAttack( void )
{
	if (g_zxc_promode.value == 0 && m_iClip == 6)
	{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); 
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_shot2.wav", 0.8, ATTN_NORM); //play sound
	
		UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
		Vector GunPosition = m_pPlayer->GetGunPosition( );
		GunPosition = GunPosition + gpGlobals->v_forward * 0;
		GunPosition = GunPosition + gpGlobals->v_right   * 7;
		GunPosition = GunPosition + gpGlobals->v_up      * 0;
		
		CUraniumBullet* Beam = CUraniumBullet :: Create( GunPosition, m_pPlayer->pev->v_angle, m_pPlayer );
		//CBaseEntity *UraniumBullBeam = CBaseEntity::Create("weapon_uraniumbullet", GunPosition, m_pPlayer->pev->v_angle, m_pPlayer->edict());
		Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right; //shot pos
		Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up; //shot pos
		m_iClip-=6;

		float flZVel = m_pPlayer->pev->velocity.z; 
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 1500; 

	}
else //here old code
	{
		if ( m_pPlayer->pev->fov != 0 )
		{
			m_fInZoom = FALSE;
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
			m_flNextSecondaryAttack = 0.3;
		}
		else if ( m_pPlayer->pev->fov != 40 )
		{
			m_fInZoom = TRUE;
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 40;
			m_flNextSecondaryAttack = 0.3;
		}
		PlayEmptySound( );
		m_flNextSecondaryAttack = 0.3;
		return;
	}

	m_flNextSecondaryAttack = 0.5;
}


// v 1.33 new attack
void CPython::ThirdAttack( void )
{
		
	if (m_iClip == 6 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 12)
	{
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 768;

		m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); // player "shoot" animation
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/bemlsr2.wav", 0.8, ATTN_NORM); //play sound
		
		CBaseEntity *pSatchel = Create( "weapon_vulcan", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
		pSatchel->pev->velocity = vecThrow;
		pSatchel->pev->avelocity.y = 80;
		pSatchel->pev->avelocity.x = 50;
		m_iClip = 0;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
	}
	
	m_flNextPrimaryAttack = 1.0;
	m_flNextSecondaryAttack = 1.0;
	
}


// v 1.33 attack
void CPython::FourthAttack( void )
{
		
	Vector vecSrc = m_pPlayer->pev->origin;
	Vector vecThrow = gpGlobals->v_forward * 300;
	
	if (g_zxc_promode.value == 0 && m_iClip == 6)
	{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/bemlsr2.wav", 0.8, ATTN_NORM);

		CBaseEntity *pSatchel = Create( "halo_base", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
		pSatchel->pev->velocity = vecThrow;
		m_iClip-=6;
		
		float flZVel = m_pPlayer->pev->velocity.z; 
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 700; 
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

	bUseScope = g_pGameRules->IsMultiplayer();

	if (DefaultReload( 6, PYTHON_RELOAD, 2.0, bUseScope ))
	{
		m_flSoundDelay = 1.5;
	}
}


void CPython::WeaponIdle( void )
{
	ResetEmptySound( );

	if (m_flSoundDelay != 0 && m_flSoundDelay <= UTIL_WeaponTimeBase() )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_reload1.wav", 0.8, ATTN_NORM);
		m_flSoundDelay = 0;
	}

	// if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
	// 	return;
	
	// int bUseScope = FALSE;

	// bUseScope = g_pGameRules->IsMultiplayer();
	
	// SendWeaponAnim( PYTHON_IDLE1, UseDecrement() ? 1 : 0, bUseScope );
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


#endif




//////////// Uranium Bullet ///////////
void    CUraniumBullet :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_antidote.mdl" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(2,2,2), Vector(2,2,2) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_357" );

	pev->velocity = gpGlobals->v_forward * 125;

	m_flDie = gpGlobals->time + 16;
	pev->dmg = 250.0;
	pev->health = 1000.0;
	pev->takedamage = DAMAGE_YES;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(iSquidSpitSprite ); // model
		WRITE_BYTE( 6 ); // life
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 80 ); // r, g, b
		WRITE_BYTE( 100 ); // r, g, b
		WRITE_BYTE( 100 ); // r, g, b
		WRITE_BYTE( 100 ); // brightness
	MESSAGE_END();	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(iSquidSpitSprite ); // model
		WRITE_BYTE( 8 ); // life
		WRITE_BYTE( 2 ); // width
		WRITE_BYTE( 108 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // brightness
	MESSAGE_END();
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(iSquidSpitSprite ); // model
		WRITE_BYTE( 10 ); // life
		WRITE_BYTE( 1 ); // width
		WRITE_BYTE( 108 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // brightness
	MESSAGE_END();

	SetThink( MoveThink );
	SetTouch( MoveTouch );
	pev->nextthink = gpGlobals->time + 0.1;
}

void    CUraniumBullet :: Precache( )
{
	BeamSprite = PRECACHE_MODEL( "sprites/smoke.spr" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	iSquidSpitSprite = PRECACHE_MODEL("sprites/mommaspit.spr");// client side spittle.
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
}

void    CUraniumBullet :: MoveTouch( CBaseEntity* pOther )
{
	TraceResult TResult;
	Vector      StartPosition;
	pev->enemy = pOther->edict( );
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );
	
	// EXPL
	if ( TResult.flFraction != 1.0 )
	{
		pev->origin = TResult.vecEndPos + (TResult.vecPlaneNormal * 25.6);
	}
	
    if( TResult.fAllSolid ) 
	{
		SUB_Remove( );
		return;
	}

	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_FLOAT(12,300);	// Pick a random direction
	vecEnd.y = RANDOM_FLOAT(12,300);
	vecEnd.z = RANDOM_FLOAT(12,300);
	vecEnd = pev->origin + vecEnd.Normalize() * 2048;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	
	// lightings 1
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
	// lightings 2
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
			WRITE_BYTE( 128 ); // noise
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 175 ); // brightness
			WRITE_BYTE( 12 ); // scroll speed
	MESSAGE_END();
	// lightings 3
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
			WRITE_BYTE( 64 ); // noise
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 128 ); // color r,g,b
			WRITE_BYTE( 128 ); // color r,g,b
			WRITE_BYTE( 175 ); // brightness
			WRITE_BYTE( 16 ); // scroll speed
	MESSAGE_END();

	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*1.5, CLASS_NONE, DMG_MORTAR  ); //DMG
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg/10, CLASS_NONE, DMG_BULLET  ); //2nd DMG
		
	UTIL_DecalTrace( &TResult, DECAL_SCORCH2 );
		
	// explode
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
		
	// beam
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
		WRITE_BYTE( 2500 );  // width
		WRITE_BYTE( 64 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 ); // brightness
		WRITE_BYTE( 8 );		// speed
	MESSAGE_END();
	//pev->takedamage = DAMAGE_NO;
	SUB_Remove( );

}

CUraniumBullet* CUraniumBullet :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CUraniumBullet* Beam = GetClassPtr( (CUraniumBullet*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CUraniumBullet :: MoveTouch );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}



void    CUraniumBullet :: MoveThink( void )
{
// fixed stuck in sky
	
if (gpGlobals->time >= m_flDie) //time out
	{
		// explode
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
		
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*3, CLASS_NONE, DMG_MORTAR  ); //DMG
		
		SUB_Remove( );
	}
	
	Vector	vecSpitDir;
	pev->angles = UTIL_VecToAngles (pev->velocity);
	pev->angles.x -= 90;
	
	vecSpitDir = ( (pev->angles).Normalize() );
	vecSpitDir.x = 0;
	vecSpitDir.y = 0;
	vecSpitDir.z = 0;
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_IMPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_BYTE( 10 );	// rad
		WRITE_BYTE( 2 );		// count
		WRITE_BYTE( 8 );		// life
	MESSAGE_END();

	pev->nextthink = gpGlobals->time + 0.01;
}





void    CVulcan :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_rad.mdl" );
	
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	pev->takedamage = 0;
	UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );

	UTIL_SetOrigin( pev, Vector(pev->origin.x, pev->origin.y,pev->origin.z));
	pev->classname = MAKE_STRING( "weapon_357" );
	m_flDie = gpGlobals->time + 17.0;
	pev->gravity			= 0.35;
	pev->friction			= 0.01;
	pev->dmg				= 50.0;
	
	

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
	pev->nextthink = gpGlobals->time + 0.1;
}

void CVulcan :: Precache( void )
{
	BeamSprite = PRECACHE_MODEL( "sprites/fexplo1.spr" );
	BeamSprite2 = PRECACHE_MODEL( "sprites/bluejet1.spr" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );

}


////////////////////////////////
//      Explode capsule       //
////////////////////////////////

void    CVulcan :: MoveThink( )
{
	CBaseEntity *pEntity = NULL;
	Vector direction = Vector(0,0,1); 
	
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = pev->origin.x + RANDOM_LONG(-256,256);	// Pick a random direction
	vecEnd.y = pev->origin.y + RANDOM_LONG(-256,256);
	vecEnd.z = pev->origin.z + RANDOM_LONG(0,64);
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	UTIL_ParticleEffect ( vecEnd, g_vecZero, 92, 25 );

	if ( (UTIL_PointContents(tr.vecEndPos ) == CONTENTS_SOLID) && tr.flFraction != 1.0 && !tr.fAllSolid  )
	{
		m_flDie = gpGlobals->time;
	}
	
		
	// first begin, beware lightings 5 sec
	if (gpGlobals->time <= m_flDie - 12)
		{
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
				WRITE_BYTE( 255 ); // color r,g,b
				WRITE_BYTE( 175 ); // brightness
				WRITE_BYTE( 8 ); // scroll speed
			MESSAGE_END();
		}

	
	
		
	
	// second is hurting 
	if (gpGlobals->time >= m_flDie - 12)
		{

			::RadiusDamage( vecEnd, pev, VARS( pev->owner ), pev->dmg, 100.0+pev->dmg, CLASS_NONE, DMG_CRUSH  ); //blast
			
			EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/lrgexpl2.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,110) );
				
			// lots of expl
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( vecEnd.x );
				WRITE_COORD( vecEnd.y );
				WRITE_COORD( vecEnd.z );
				WRITE_SHORT( BeamSprite );
				WRITE_BYTE( pev->dmg ); // scale * 10
				WRITE_BYTE( 200 ); // brightness
			MESSAGE_END( );
			// lots of expl 2
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( vecEnd.x );
				WRITE_COORD( vecEnd.y );
				WRITE_COORD( vecEnd.z );
				WRITE_SHORT( BeamSprite2 );
				WRITE_BYTE( pev->dmg ); // scale * 10
				WRITE_BYTE( 200 ); // brightness
			MESSAGE_END( );

			pev->dmg += 5.0;
			
			//UTIL_Remove(this);
		}

	// full explode and self destroy
	if (gpGlobals->time >= m_flDie)
		{
			// spark effects
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
				WRITE_BYTE( TE_STREAK_SPLASH );
				WRITE_COORD( pev->origin.x );		// origin
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_COORD( direction.x );	//// direction
				WRITE_COORD( direction.y );
				WRITE_COORD( direction.z );
				WRITE_BYTE( 255 );	// Streak color 6
				WRITE_SHORT( 64 );	// count
				WRITE_SHORT( 255 );
				WRITE_SHORT( 255 );	// Random velocity modifier
			MESSAGE_END();

			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 256.0+pev->dmg, CLASS_NONE, DMG_BULLET  ); //end blast

			// lights
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
				WRITE_BYTE(TE_DLIGHT);
				WRITE_COORD(pev->origin.x);	// X
				WRITE_COORD(pev->origin.y);	// Y
				WRITE_COORD(pev->origin.z);	// Z
				WRITE_BYTE( 32 );		// radius * 0.1
				WRITE_BYTE( 112 );		// r
				WRITE_BYTE( 112 );		// g
				WRITE_BYTE( 112 );		// b
				WRITE_BYTE( 128 );		// life * 10
				WRITE_BYTE( 0 );		// decay * 0.1
			MESSAGE_END( );
			
			// random explosions
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
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
	pev->nextthink = gpGlobals->time + 0.3; // dynamic update

}


void  CVulcan::MoveTouch( CBaseEntity *pOther )
{

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
	
	pev->velocity.z += 24; // jump
	
	// touch explode
	if ( pOther->pev->solid != SOLID_BSP) 
	{
		m_flDie = gpGlobals->time - 20;
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*2, CLASS_NONE, DMG_BLAST  ); //end blast
	}
		
	pev->velocity = pev->velocity*0.45;



		
}

////////////////////////////////
//   Random motion capsule    //
////////////////////////////////

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
	pev->dmg				= 69.0;
	
	pev->angles = UTIL_VecToAngles (pev->velocity);
	pev->angles.x -= 90;
	
	SetThink( MoveThink );
	SetTouch( MoveTouch );
}

void CHalo :: Precache( void )
{

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	iSquidSpitSprite = PRECACHE_MODEL("sprites/xbeam4.spr");// client side spittle.

	// foolow trails
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

			::RadiusDamage( vecEnd, pev, VARS( pev->owner ), pev->dmg, pev->dmg*2, CLASS_NONE, DMG_SHOCK  ); 
		}

	if (gpGlobals->time >= m_flDie - 5) // full explode and self destroy
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
			
			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*3, CLASS_NONE, DMG_CRUSH  ); 
			EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/lrgexpl2.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,110) );

			// lights
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
			
			// beam
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
	m_flDie = gpGlobals->time - 10;
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg, CLASS_NONE, DMG_BLAST  ); //end blast
}

LINK_ENTITY_TO_CLASS( ammo_357, CPythonAmmo );
LINK_ENTITY_TO_CLASS( weapon_python, CPython );
LINK_ENTITY_TO_CLASS( weapon_357, CPython );
LINK_ENTITY_TO_CLASS( weapon_vulcan, CVulcan );
LINK_ENTITY_TO_CLASS( halo_base, CHalo );
LINK_ENTITY_TO_CLASS( weapon_uraniumbullet, CUraniumBullet );
