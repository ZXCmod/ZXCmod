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
#include "hornet.h"
#include "gamerules.h"
#include "shake.h"


enum hgun_e {
	HGUN_IDLE1 = 0,
	HGUN_FIDGETSWAY,
	HGUN_FIDGETSHAKE,
	HGUN_DOWN,
	HGUN_UP,
	HGUN_SHOOT
};

enum firemode_e
{
	FIREMODE_TRACK = 0,
	FIREMODE_FAST
};


LINK_ENTITY_TO_CLASS( weapon_hornetgun, CHgun );

BOOL CHgun::IsUseable( void )
{
	return TRUE;
}

void CHgun::Spawn( )
{
	Precache( );
	m_iId = WEAPON_HORNETGUN;
	SET_MODEL(ENT(pev), "models/w_hgun.mdl");

	m_iDefaultAmmo = HIVEHAND_DEFAULT_GIVE;
	m_iFirePhase = 0;

	FallInit();// get ready to fall down.
	float m_flNextChatTime8 = gpGlobals->time; //delay
}


void CHgun::Precache( void )
{
	PRECACHE_MODEL("models/v_hgun.mdl");
	PRECACHE_MODEL("models/w_hgun.mdl");
	PRECACHE_MODEL("models/p_hgun.mdl");
	PRECACHE_MODEL( "models/can.mdl" );
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );
	PRECACHE_SOUND( "debris/beamstart9.wav" );
	PRECACHE_SOUND( "debris/bustconcrete1.wav" );
	PRECACHE_SOUND( "items/medcharge4.wav" );
	BSpr = PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_MODEL("models/bag.mdl");
	

	m_usHornetFire = PRECACHE_EVENT ( 1, "events/firehornet.sc" );

	UTIL_PrecacheOther("hornet");
}

int CHgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{

#ifndef CLIENT_DLL
		if ( g_pGameRules->IsMultiplayer() )
		{
			// in multiplayer, all hivehands come full. 
			pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = HORNET_MAX_CARRY;
		}
#endif

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

int CHgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hornets";
	p->iMaxAmmo1 = HORNET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_HORNETGUN;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = HORNETGUN_WEIGHT;

	return 1;
}


BOOL CHgun::Deploy( )
{
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 1; //ammo set to 1. 
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 270 );
	return DefaultDeploy( "models/v_hgun.mdl", "models/p_hgun.mdl", HGUN_UP, "hive" );
}

void CHgun::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( HGUN_DOWN );

	//!!!HACKHACK - can't select hornetgun if it's empty! no way to get ammo for it, either.
	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;
	}
}


void CHgun::PrimaryAttack()
{

	Reload( );

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		return;
	}

#ifndef CLIENT_DLL
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );

	CBaseEntity *pHornet = CBaseEntity::Create( "hornet", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	pHornet->pev->velocity = gpGlobals->v_forward * 300;

	m_flRechargeTime = gpGlobals->time + 0.5;
#endif
	
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_TRACK, 0, 0, 0 );

	

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.25;

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
	{
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}









void CHgun::SecondaryAttack( void )
{
	Reload();
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		return;
	}

#ifndef CLIENT_DLL

///////////neew

    Vector vecSrc, vecAim, vecOrig;
    TraceResult tr;
	Vector vecDir;
    CBaseEntity *pEntity;
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	//float speed = m_pPlayer->pev->velocity.Length();
    vecOrig = m_pPlayer->GetGunPosition( );
    vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;
    vecAim = gpGlobals->v_forward * (RANDOM_LONG(1,9) ); //RANDOM_LONG(-10,10)  speed/20
    UTIL_TraceLine ( vecOrig, vecOrig + vecAim * 2048, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr);
	
	
	
//freeze 

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(vecSrc.x);
            WRITE_COORD(vecSrc.y);
            WRITE_COORD(vecSrc.z);
            WRITE_COORD( tr.vecEndPos.x );
            WRITE_COORD( tr.vecEndPos.y );
            WRITE_COORD( tr.vecEndPos.z );
            WRITE_SHORT( BSpr ); //sprite
            WRITE_BYTE( 0 ); // Starting frame
            WRITE_BYTE( 0  ); // framerate * 0.1
            WRITE_BYTE( 2 ); // life * 0.1
            WRITE_BYTE( 64 ); // width
            WRITE_BYTE( 5 ); // noise
            WRITE_BYTE( 100 ); // color r,g,b
            WRITE_BYTE( 100 ); // color r,g,b
            WRITE_BYTE( 255 ); // color r,g,b
            WRITE_BYTE( 240 ); // brightness
            WRITE_BYTE( 70 ); // scroll speed
    MESSAGE_END();
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 2; // -2 ammo
	
	if (!(m_pPlayer->pev->button & IN_DUCK))
	{
	m_pPlayer->pev->punchangle.x -= 7;
	m_pPlayer->pev->punchangle.y += RANDOM_LONG(-5,4);
	}
	else
	{
	m_pPlayer->pev->punchangle.x -= 4;
	m_pPlayer->pev->punchangle.y += RANDOM_LONG(-4,2);
	}
	
	//Reload( );
	m_flRechargeTime = gpGlobals->time + 0.5;
    pEntity = CBaseEntity::Instance(tr.pHit);
    if (pEntity != NULL && pEntity->pev->takedamage != pEntity->IsPlayer()) //in 1.26 allow to freeze monsters
    {
//need to be remove pPlayer 
	
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
        pPlayer->pev->rendermode = kRenderNormal;
        pPlayer->pev->renderfx = kRenderFxGlowShell;
        pPlayer->pev->rendercolor.x = 200;  // red
        pPlayer->pev->rendercolor.y = 200;  // green
        pPlayer->pev->rendercolor.z = 255; // blue
        pPlayer->pev->renderamt = 70;
        pPlayer->FTime2 = gpGlobals->time + 24.1; //freeze timer of m0nters
		
	    if (pEntity != NULL && pEntity->pev->takedamage && pEntity->IsPlayer())
		{
		UTIL_ScreenFade( pPlayer, Vector(0,0,255), 5.0, 1.0, 140, FFADE_IN );
        pPlayer->pev->rendermode = kRenderNormal;
        pPlayer->pev->renderfx = kRenderFxGlowShell;
        pPlayer->pev->rendercolor.x = 200;  // red
        pPlayer->pev->rendercolor.y = 200;  // green
        pPlayer->pev->rendercolor.z = 255; // blue
        pPlayer->pev->renderamt = 70;
        pPlayer->EnableControl(FALSE);
		//if (pPlayer->FTime2 <= 5)
        pPlayer->FTime2 = gpGlobals->time + 1.75; //1.25 old. Changed in v1.26 to 2.05 //RANDOM_LONG(1.12,2.25)
		}
    }

//////////// end freeze
	
	
	
	
#endif
	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_FAST, 0, 0, 0 );
	//m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.35; //delay
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	


}
	

	//////////////3

void CHgun::ThirdAttack( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 16)
	{

		//play sounds
			switch(RANDOM_LONG(0,1))
			{
			case 0: 
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.9, ATTN_NORM);
			break;
			case 1: 
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher2.wav", 0.9, ATTN_NORM);
			break;
			}

		
		
		
		
		
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		//dont create another reload void, reload() used by hornet 

		int iAnim;
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 512;

		#ifndef CLIENT_DLL
			CBaseEntity *pSatchel = Create( "player_freeze", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSatchel->pev->velocity = vecThrow;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			iAnim = HGUN_FIDGETSHAKE;
			SendWeaponAnim( iAnim );
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 16;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flRechargeTime = gpGlobals->time + 0.5;
			Reload( );
		#endif

		return;
	}


}
	






void CHgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= HORNET_MAX_CARRY)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < HORNET_MAX_CARRY && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 0.5;
	}
}


void CHgun::WeaponIdle( void )
{

	Reload( );

if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 4) 
	{
	if (  m_pPlayer->m_flNextChatTime8 < gpGlobals->time ) //need delay
		{
		//play sounds
			switch(RANDOM_LONG(0,1))
			{
			case 0: 
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.9, ATTN_NORM);
			break;
			case 1: 
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher2.wav", 0.9, ATTN_NORM);
			break;
			}

		
		
		
		
		
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		//dont create another reload void, reload() used by hornet 

		int iAnim;
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 512;

		#ifndef CLIENT_DLL
			CBaseEntity *pSatchel = Create( "weapon_clip_generic", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSatchel->pev->velocity = vecThrow;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			iAnim = HGUN_FIDGETSHAKE;
			SendWeaponAnim( iAnim );
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 4;
			m_pPlayer->m_flNextChatTime8 = gpGlobals->time + 1.02;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flRechargeTime = gpGlobals->time + 0.5;
			Reload( );
		#endif

		return;
		//}
		}
	}
	
	
	
	
	
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
	if (flRand <= 0.75)
	{
		iAnim = HGUN_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 30.0 / 16 * (2);
	}
	else if (flRand <= 0.875)
	{
		iAnim = HGUN_FIDGETSWAY;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
	}
	else
	{
		iAnim = HGUN_FIDGETSHAKE;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 35.0 / 16.0;
	}
	SendWeaponAnim( iAnim );
	

}

#endif









//////////////NEWWW Flash can (i was dont know, how create freeze effect)

class   CFreeze : public CBaseEntity
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
};

//////////////1.28 new Freeze bomb. Need 16 ammo for launch, big freeze effect. 

class   CFreezeBomb : public CBaseEntity
{
        public:

        void    Spawn           ( );
        void    MoveThink       ( );
        void    Explode         ( );
		void EXPORT Touch( CBaseEntity *pOther );
		void  Exp( void );
		int m_flDie;
};

LINK_ENTITY_TO_CLASS( weapon_clip_generic, CFreeze );
LINK_ENTITY_TO_CLASS( player_freeze, CFreezeBomb);







void    CFreeze :: Spawn( )
{
		Precache( );
        SET_MODEL( ENT(pev), "models/can.mdl" );
        pev->movetype = MOVETYPE_BOUNCE;
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "hornet" );
		m_flDie = gpGlobals->time + 2;
		pev->dmg = 0;
		pev->takedamage = DAMAGE_YES;
		pev->nextthink = gpGlobals->time + 3;
		SetThink( MoveThink );
		pev->gravity			= 0.35;
		pev->friction			= 0.35;
		pev->health			= 9999999; //get more eat!

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 50 ); // life
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 0 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // brightness
		MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
}

void CFreeze :: Precache( void )
{
m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
PRECACHE_SOUND( "weapons/gravgren.wav" );
m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
PRECACHE_MODEL( "models/can.mdl" );
}

void    CFreeze:: Explode(int DamageType)
{

pev->nextthink = gpGlobals->time + 0.15; //0.15 old
pev->effects |= EF_LIGHT;
SetThink(MoveThink);
}


void    CFreeze :: MoveThink( )
{
//5.0 4.8

Explode(DMG_FREEZE);
if (gpGlobals->time >= m_flDie) //full explode and self destroy
	{
	

			CBaseEntity *pEntity = NULL;
			Vector	vecDir;
			vecDir = Vector( 0, 0, 0 );
			Vector direction = Vector(0,0,1);
			
			
			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL)
       		 	{
				if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP)
				{
				vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
				pEntity->pev->velocity = pEntity->pev->velocity + vecDir * 2048;
				::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 25, 512, CLASS_NONE, DMG_FREEZE|DMG_MORTAR  );
				UTIL_ScreenShake( pEntity->pev->origin, 12.0, 120, 0.9, 1 );
				#ifndef CLIENT_DLL
				UTIL_ScreenFade( pEntity, Vector(255,255,RANDOM_LONG(128,255)), 8, 0.84, 255, FFADE_IN ); //strong flash 
				#endif


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
	}
}
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 30, 256, CLASS_NONE, DMG_GENERIC  ); //end blast
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 550 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 5 ); // life
			WRITE_BYTE( RANDOM_LONG(64,128) );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( RANDOM_LONG(128,255) );   // r, g, b
			WRITE_BYTE( RANDOM_LONG(128,255) );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();


		//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 24 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 92 );		// g
		WRITE_BYTE( 92 );		// b
		WRITE_BYTE( 64 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
	

	
		// random explosions
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(8,16) + 7  ); // scale * 10
			WRITE_BYTE( RANDOM_LONG(8,10)  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	pev->takedamage = DAMAGE_NO;
	SetThink( SUB_Remove );
}
}











///////NEW WEAPON - TRUE FREEZE BOMB
///////////////////////
///////////////////////


void    CFreezeBomb :: Spawn( )
{
		Precache( );
        SET_MODEL( ENT(pev), "models/bag.mdl" );
        pev->movetype = MOVETYPE_BOUNCE;
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector( 0, 0, 0), Vector(0, 0, 0) );
        UTIL_SetOrigin( pev, pev->origin );
		m_flDie = gpGlobals->time + 180;
		pev->dmg = 0;
		pev->takedamage = DAMAGE_YES;
		pev->gravity			= 0.35;
		pev->friction			= 0.35;
		pev->health			= 100;
		pev->dmg = 10;

		
		pev->nextthink = gpGlobals->time + 3;
		SetTouch( Touch );
		SetThink( MoveThink );
}


void    CFreezeBomb :: Explode( )
{
if (pev->dmg == 10)
	{
	pev->nextthink = gpGlobals->time + 1.35; //slow update
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/bustconcrete1.wav", 0.5, ATTN_NORM);
	pev->velocity.z = 125; //jump
	pev->angles.y += 30; //rotate
	}
else
	{
	pev->nextthink = gpGlobals->time + 0.02; //fast
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/medcharge4.wav", 1, ATTN_NORM, 0, pev->dmg*3);
	}
	
	SetThink(MoveThink);
}


void    CFreezeBomb :: MoveThink( )
{

	CBaseEntity *pEntity = NULL;


	Explode();
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL && (pev->dmg == 10))
       	{
		if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP)
			{
			if (FVisible( pEntity ) && (pev->flags & FL_ONGROUND))
				{
				pev->dmg = 11;
				pev->velocity.z = 275; //jump strong
				pev->nextthink = gpGlobals->time + 0.25;
				//UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
				//UTIL_SetOrigin( pev, pev->origin );
				//return;
				}
			}
		}
		
		//check for reaction
		if (pev->dmg != 10)
			{
			pev->dmg += 1;
			Exp();
			}
				









/////////////////
///////////////////////////
	
	
	
	
	
	 
if (gpGlobals->time >= m_flDie) //full explode and self destroy
	{
		// random explosions
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(8,16) + 7  ); // scale * 10
			WRITE_BYTE( RANDOM_LONG(8,10)  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	pev->takedamage = DAMAGE_NO;
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/beamstart9.wav", 1.0, ATTN_NORM);
	SetThink( SUB_Remove );
	} 



}



void  CFreezeBomb::Touch( CBaseEntity *pOther )
{
	//stop moving

		pev->movetype = MOVETYPE_TOSS;
		SetTouch( NULL );
		pev->nextthink = gpGlobals->time + 0.25;
		
}

void  CFreezeBomb::Exp( )
{

	if (pev->dmg > 75) //delete object time
		{
//////////////effects
Vector direction = Vector(0,0,1);

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
		WRITE_SHORT( 512 );	// count
		WRITE_SHORT( 1024 );
		WRITE_SHORT( 2600 );	// Random velocity modifier
	MESSAGE_END();

	
	
	
	
	
	
	
	
	//action
	CBaseEntity *pEntity = NULL;


	Explode();
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 460 )) != NULL)
       	{
		//if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP && pev->movetype == MOVETYPE_TOSS)
			//{
		    if (pEntity != NULL && pEntity->pev->takedamage != pEntity->IsPlayer()) //in 1.26 allow to freeze monsters
			{
			if (FVisible( pEntity )) //anyone entity
				{
				pEntity->pev->rendermode = kRenderNormal;
				pEntity->pev->renderfx = kRenderFxGlowShell;
				pEntity->pev->rendercolor.x = 200;  // red
				pEntity->pev->rendercolor.y = 200;  // green
				pEntity->pev->rendercolor.z = 255; // blue
				pEntity->pev->renderamt = 70;
				pEntity->FTime2 = gpGlobals->time + 60; //big freeze
				
			    if (pEntity != NULL && pEntity->pev->takedamage && pEntity->IsPlayer()) //check only player
					{
					CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
					UTIL_ScreenFade( pPlayer, Vector(0,0,255), 5.0, 1.0, 140, FFADE_IN );
					pPlayer->pev->rendermode = kRenderNormal;
					pPlayer->pev->renderfx = kRenderFxGlowShell;
					pPlayer->pev->rendercolor.x = 200;  // red
					pPlayer->pev->rendercolor.y = 200;  // green
					pPlayer->pev->rendercolor.z = 255; // blue
					pPlayer->pev->renderamt = 70;
					pPlayer->EnableControl(FALSE);
					pPlayer->FTime2 = gpGlobals->time + 3.25;
					}
				}
			}
		
		}
		
		

		
		
		
		
		
		
		//owtheers
		SetTouch( NULL );
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/beamstart9.wav", 1.0, ATTN_NORM);
		//STOP_SOUND( ENT(pev), CHAN_ITEM, "items/medcharge4.wav" );
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "debris/beamstart9.wav", 1.0, ATTN_NORM );
		//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 24 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 92 );		// g
		WRITE_BYTE( 92 );		// b
		WRITE_BYTE( 64 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
	
	pev->nextthink = gpGlobals->time + 0.1;
	pev->takedamage = DAMAGE_NO;
	SetThink( SUB_Remove );
		}
		
		pev->angles.y += 30; //rotate

}
