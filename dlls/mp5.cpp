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
#include "soundent.h"
#include "gamerules.h"
#include "decals.h"

enum mp5_e
{
	MP5_LONGIDLE = 0,
	MP5_IDLE1,
	MP5_LAUNCH,
	MP5_RELOAD,
	MP5_DEPLOY,
	MP5_FIRE1,
	MP5_FIRE2,
	MP5_FIRE3,
};

//zap
 class   CZap : public CGrenade
{
        public:
        void    Spawn           ( );
		short		m_LaserSprite;
}; 


LINK_ENTITY_TO_CLASS( weapon_mp5, CMP5 );
LINK_ENTITY_TO_CLASS( weapon_9mmAR, CMP5 );
LINK_ENTITY_TO_CLASS( blaster_bolt, CZap );


//=========================================================
//=========================================================
int CMP5::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CMP5::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_9mmAR"); // hack to allow for old names
	Precache( );
	SET_MODEL(ENT(pev), "models/w_9mmAR.mdl");
	m_iId = WEAPON_MP5;

	m_iDefaultAmmo = MP5_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

void CMP5::Precache( void )
{
	PRECACHE_MODEL("models/v_9mmAR.mdl");
	PRECACHE_MODEL("models/w_9mmAR.mdl");
	PRECACHE_MODEL("models/p_9mmAR.mdl");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shellTE_MODEL

	PRECACHE_MODEL("models/grenade.mdl");	// grenade

	PRECACHE_MODEL("models/w_9mmARclip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND("items/clipinsert1.wav");
	PRECACHE_SOUND("items/cliprelease1.wav");

	PRECACHE_SOUND ("weapons/hks1.wav");// H to the K
	PRECACHE_SOUND ("weapons/hks2.wav");// H to the K
	PRECACHE_SOUND ("weapons/hks3.wav");// H to the K
	PRECACHE_SOUND ("debris/beamstart11.wav");

	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );

	PRECACHE_SOUND ("weapons/357_cock1.wav");

	m_usMP5 = PRECACHE_EVENT( 1, "events/mp5.sc" );
	m_usMP52 = PRECACHE_EVENT( 1, "events/mp52.sc" );
	BSpr = PRECACHE_MODEL("sprites/bolt1.spr");
}

int CMP5::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = "ARgrenades";
	p->iMaxAmmo2 = M203_GRENADE_MAX_CARRY;
	p->iMaxClip = MP5_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_MP5;
	p->iWeight = MP5_WEIGHT;

	return 1;
}

int CMP5::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL CMP5::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 270 );
	return DefaultDeploy( "models/v_9mmAR.mdl", "models/p_9mmAR.mdl", MP5_DEPLOY, "mp5" );
}


void CMP5::PrimaryAttack()
{


float speed = m_pPlayer->pev->velocity.Length();

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;


	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

	
	

	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( 0.00016, 0.04976, 0.02976 ), 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );

	
	
  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	
	
	
}



void CMP5::SecondaryAttack( void )
{

	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] == 0)
	{
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_pPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
	m_pPlayer->m_flStopExtraSoundTime = UTIL_WeaponTimeBase() + 0.2;
			
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

 	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	// we don't add in player velocity anymore.
	CGrenade::ShootContact( m_pPlayer->pev, 
							m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, 
							gpGlobals->v_forward * 800 );

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usMP52 );
	
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;// idle pretty soon after shooting.

	if (!m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType])
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
}

/////zap-lasers (1.28)




void CMP5::ThirdAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel >= 2)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}


		TraceResult	tr;	
		Vector vecSrc;
		Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
		UTIL_MakeVectors( anglesAim );
		vecSrc = m_pPlayer->GetGunPosition( )  + gpGlobals->v_right * 9 + gpGlobals->v_up * -10;
		Vector vecDir = gpGlobals->v_forward;
		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 4096, dont_ignore_monsters, m_pPlayer->edict(), &tr);
		
	//if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1)
	if (m_iClip >= 1)
    {


	
		/////beam ray 1
			// MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            // WRITE_BYTE( TE_BEAMPOINTS );
            // WRITE_COORD(vecSrc.x);
            // WRITE_COORD(vecSrc.y);
            // WRITE_COORD(vecSrc.z);
            // WRITE_COORD( tr.vecEndPos.x ); //tr.vecEndPos.
            // WRITE_COORD( tr.vecEndPos.y );
            // WRITE_COORD( tr.vecEndPos.z );
            // WRITE_SHORT( BSpr ); //sprite
            // WRITE_BYTE( 1 ); // Starting frame
            // WRITE_BYTE( 0  ); // framerate * 0.1
            // WRITE_BYTE( 1 ); // life * 0.1
            // WRITE_BYTE( 30 ); // width
            // WRITE_BYTE( 8 ); // noise
            // WRITE_BYTE( 128 ); // color r,g,b
            // WRITE_BYTE( 128 ); // color r,g,b
            // WRITE_BYTE( 200 ); // color r,g,b
            // WRITE_BYTE( 110 ); // brightness
            // WRITE_BYTE( 100 ); // scroll speed
			// MESSAGE_END();
			
		/////beam ray 2
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(vecSrc.x);
            WRITE_COORD(vecSrc.y);
            WRITE_COORD(vecSrc.z);
            WRITE_COORD( tr.vecEndPos.x ); //tr.vecEndPos.
            WRITE_COORD( tr.vecEndPos.y );
            WRITE_COORD( tr.vecEndPos.z );
            WRITE_SHORT( BSpr ); //sprite
            WRITE_BYTE( 1 ); // Starting frame
            WRITE_BYTE( 0  ); // framerate * 0.1
            WRITE_BYTE( 1 ); // life * 0.1
            WRITE_BYTE( 30 ); // width
            WRITE_BYTE( 9 ); // noise
            WRITE_BYTE( 128 ); // color r,g,b
            WRITE_BYTE( 100 ); // color r,g,b
            WRITE_BYTE( 255 ); // color r,g,b
            WRITE_BYTE( 120 ); // brightness
            WRITE_BYTE( 100 ); // scroll speed
			MESSAGE_END();
			
		/////beam ray 3
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(vecSrc.x);
            WRITE_COORD(vecSrc.y);
            WRITE_COORD(vecSrc.z);
            WRITE_COORD( tr.vecEndPos.x ); //tr.vecEndPos.
            WRITE_COORD( tr.vecEndPos.y );
            WRITE_COORD( tr.vecEndPos.z );
            WRITE_SHORT( BSpr ); //sprite
            WRITE_BYTE( 1 ); // Starting frame
            WRITE_BYTE( 0  ); // framerate * 0.1
            WRITE_BYTE( 1 ); // life * 0.1
            WRITE_BYTE( 30 ); // width
            WRITE_BYTE( 10 ); // noise
            WRITE_BYTE( 100 ); // color r,g,b
            WRITE_BYTE( 128 ); // color r,g,b
            WRITE_BYTE( 255 ); // color r,g,b
            WRITE_BYTE( 130 ); // brightness
            WRITE_BYTE( 100 ); // scroll speed
			MESSAGE_END();
	
	m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecDir, Vector( 0, 0, 0 ), 4096, BULLET_PLAYER_MP5, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	UTIL_DecalTrace( &tr, DECAL_BIGSHOT1 + RANDOM_LONG(1,4) ); // + RANDOM_LONG(1,4)
	UTIL_Sparks( tr.vecEndPos );
			
		
	//UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	m_iClip--;
	EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart11.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,100) );
	CBaseEntity *pZap = Create( "blaster_bolt", tr.vecEndPos, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	
			SendWeaponAnim( 5 );
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.12;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.12;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.12;
	}
	

}




void CMP5::Reload( void )
{
	if ( m_pPlayer->ammo_9mm <= 0 )
		return;

	DefaultReload( MP5_MAX_CLIP, MP5_RELOAD, 1.5 );
}


void CMP5::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MP5_LONGIDLE;	
		break;
	
	default:
	case 1:
		iAnim = MP5_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}



class CMP5AmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_9mmARclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_9mmARclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_MP5CLIP_GIVE, "9mm", _9MM_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp5clip, CMP5AmmoClip );
LINK_ENTITY_TO_CLASS( ammo_9mmAR, CMP5AmmoClip );



class CMP5Chainammo : public CBasePlayerAmmo
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
		int bResult = (pOther->GiveAmmo( AMMO_CHAINBOX_GIVE, "9mm", _9MM_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_9mmbox, CMP5Chainammo );


class CMP5AmmoGrenade : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_ARgrenade.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_ARgrenade.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_M203BOX_GIVE, "ARgrenades", M203_GRENADE_MAX_CARRY ) != -1);

		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp5grenades, CMP5AmmoGrenade );
LINK_ENTITY_TO_CLASS( ammo_ARgrenades, CMP5AmmoGrenade );







void CZap::Spawn( )
{
		m_LaserSprite = PRECACHE_MODEL( "sprites/bolt1.spr" );
        UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "weapon_9mmAR" );
		CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 80 )) != NULL)
       		 {
				
		if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict())) //!(pEntity->pev->movetype == MOVETYPE_FLY)
				{
		
		
		/////beam ray
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(pev->origin.x);
            WRITE_COORD(pev->origin.y);
            WRITE_COORD(pev->origin.z);
            WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
            WRITE_COORD( pEntity->pev->origin.y );
            WRITE_COORD( pEntity->pev->origin.z );
            WRITE_SHORT( m_LaserSprite ); //sprite
            WRITE_BYTE( 1 ); // Starting frame
            WRITE_BYTE( 0  ); // framerate * 0.1
            WRITE_BYTE( 1 ); // life * 0.1
            WRITE_BYTE( 25 ); // width
            WRITE_BYTE( 10 ); // noise
            WRITE_BYTE( 20 ); // color r,g,b
            WRITE_BYTE( 200 ); // color r,g,b
            WRITE_BYTE( 255 ); // color r,g,b
            WRITE_BYTE( 255 ); // brightness
            WRITE_BYTE( 0 ); // scroll speed
		MESSAGE_END();
		
		pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(3,14), DMG_GENERIC);		
		 } 
	}
		pev->nextthink = gpGlobals->time + 0.12;
		SetThink( SUB_Remove );
}















