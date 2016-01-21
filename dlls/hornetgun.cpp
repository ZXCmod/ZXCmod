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
#include "hornetgun.h"

extern float g_flWeaponCheat;

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
	
	
	m_flNextChatTime14; //limit
	

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
	PRECACHE_MODEL( "models/fungus(small).mdl" );
	PRECACHE_MODEL( "models/fungus(small)t.mdl" );
	
	
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );
	PRECACHE_SOUND( "debris/beamstart9.wav" );
	PRECACHE_SOUND( "debris/bustconcrete1.wav" );
	PRECACHE_SOUND( "items/medcharge4.wav" );
	PRECACHE_SOUND( "debris/beamstart4.wav" );
	PRECACHE_SOUND( "buttons/blip1.wav" );
	PRECACHE_SOUND( "buttons/blip2.wav" );
	PRECACHE_SOUND( "zxc/explode3.wav" );
	

	
	
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");
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
	if (m_pPlayer->m_flNextChatTime14 < 0)
		m_pPlayer->m_flNextChatTime14 = 0;

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
	if ( g_flWeaponCheat != 0) //no hornets anymore, fix server crash
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev) );
			WRITE_STRING( "Attack disabled, while sv_cheats is on." );
		MESSAGE_END();
		EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "buttons/bell1.wav", 0.75, ATTN_NORM, 1.0, 102 );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 5; //delay
		return;
	}

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


	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_TRACK, 0, 0, 0 );

	

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


	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(tr.vecEndPos.x);	// X
		WRITE_COORD(tr.vecEndPos.y);	// Y
		WRITE_COORD(tr.vecEndPos.z);	// Z
		WRITE_BYTE( 16 );		// radius * 0.1
		WRITE_BYTE( 100 );		// r
		WRITE_BYTE( 100 );		// g
		WRITE_BYTE( 200 );		// b
		WRITE_BYTE( 128 );		// time * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );
	

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
	
	m_flRechargeTime = gpGlobals->time + 0.5;
    pEntity = CBaseEntity::Instance(tr.pHit);
	
	#ifndef CLIENT_DLL
	
	//don't hit on transparent objects
    if (pEntity != NULL && (pEntity->IsBSPModel()))
    {
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25; //delay
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_FAST, 0, 0, 0 );
		return; //close
	}
	
    if (pEntity != NULL && !(pEntity->IsPlayer())) //in 1.26 allow to freeze monsters
    {
		//need to be remove pPlayer 
        pEntity->pev->rendermode = kRenderNormal;
        pEntity->pev->renderfx = kRenderFxGlowShell;
        pEntity->pev->rendercolor.x = 100;  // red
        pEntity->pev->rendercolor.y = 100;  // green
        pEntity->pev->rendercolor.z = 255; // blue
        pEntity->pev->renderamt = 70;
        pEntity->FTime2 = gpGlobals->time + 3.1; //freeze timer of m0nters
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25; //delay
/* 		
		//spectator
		SET_VIEW( m_pPlayer->edict(), pEntity->edict() );
		m_pPlayer->m_afPhysicsFlags |= PFLAG_OBSERVER;
		m_pPlayer->pev->effects |= EF_NODRAW;
		m_pPlayer->pev->view_ofs = g_vecZero;
		m_pPlayer->pev->fixangle = TRUE;
		m_pPlayer->pev->solid = SOLID_NOT;
		m_pPlayer->pev->takedamage = DAMAGE_NO;
		m_pPlayer->pev->movetype = MOVETYPE_NOCLIP;
		ClearBits( m_pPlayer->m_afPhysicsFlags, PFLAG_DUCKING );
		ClearBits( m_pPlayer->pev->flags, FL_DUCKING );
		m_pPlayer->pev->deadflag = DEAD_RESPAWNABLE;
		m_pPlayer->pev->health = 1;
		m_pPlayer->m_fInitHUD = TRUE;
		//UTIL_SetOrigin( m_pPlayer->pev, m_pPlayer->vecPosition );
		m_pPlayer->EnableControl(TRUE);
		m_pPlayer->pev->renderamt = 0;
 */


		
    }
	if (pEntity != NULL && pEntity->IsPlayer())
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
		
		if (pPlayer->pev->renderfx != kRenderFxGlowShell)
			pPlayer->FTime2 = gpGlobals->time + 1.75; //1.25 old. 
		else
			pPlayer->FTime2 = gpGlobals->time += 0.95; //added some freeze time to current, dont set
			
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.95; //delay 1.29. No evil activity.

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
	//////////// end freeze
	#endif

	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_FAST, 0, 0, 0 );
	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
}
	

//////////////3

void CHgun::ThirdAttack( void )
{
if (m_pPlayer->m_flNextChatTime14 > 2)
	return;
	
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 10)
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
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 10;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flRechargeTime = gpGlobals->time + 0.5;
			m_pPlayer->m_flNextChatTime14 ++;
			pSatchel->pev->skin = 0;
			Reload( );
		#endif
		return;
	}
}

//////////////4

void CHgun::FourthAttack( void )
{

	if (m_pPlayer->pev->health <= 5)
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 0)
	{

		

		
		CBaseEntity *pEntity;
		TraceResult	tr;	
		Vector vecSrc;
		vecSrc = m_pPlayer->GetGunPosition( );
		Vector vecDir = gpGlobals->v_forward;
		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 4096, dont_ignore_monsters, m_pPlayer->edict(), &tr);
		pEntity = CBaseEntity::Instance(tr.pHit); //trace hit to entity
		
		if (!pEntity->IsAlive())
		{
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5; 
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "buttons/blip1.wav", 0.75, ATTN_NORM);
		}
		if (pEntity->pev->health >= pEntity->pev->max_health )
			return;
		
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		//dont create another reload void, reload() used by hornet 

		int iAnim;

		
		if (pEntity != NULL && (pEntity->pev->flags & (FL_CLIENT | FL_MONSTER)) ) //  && pEntity->IsPlayer()
		{

			#ifndef CLIENT_DLL
				m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
				iAnim = HGUN_FIDGETSHAKE;
				SendWeaponAnim( iAnim );
				
				//hurt a player, but don't kill yourself
				if (m_pPlayer->pev->health >= 6)
				{
					m_pPlayer->TakeDamage(pev, VARS( pev->owner ), 5, DMG_FALL);
					UTIL_ScreenFade( m_pPlayer, Vector(175,25,25), 1, 0.24, 64, FFADE_IN ); //flash 
				}
				
				//heal anyone
				if (pEntity->pev->health <= pEntity->pev->max_health )
				{
					pEntity->TakeHealth(10, DMG_GENERIC);
					UTIL_ScreenFade( pEntity, Vector(0,175,50), 1, 0.24, 64, FFADE_IN ); //flash 
				}
				
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
					WRITE_BYTE( 3 ); // life * 0.1
					WRITE_BYTE( 10 ); // width
					WRITE_BYTE( 15 ); // noise
					WRITE_BYTE( 0 ); // color r,g,b
					WRITE_BYTE( 175 ); // color r,g,b
					WRITE_BYTE( 50 ); // color r,g,b
					WRITE_BYTE( 100 ); // brightness
					WRITE_BYTE( 12 ); // scroll speed
				MESSAGE_END();
				
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "debris/beamstart4.wav", 0.75, ATTN_NORM);
				
					
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5; 
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
				
				
				Reload( );
			#endif
		}
		
		
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
			Vector vecThrow = gpGlobals->v_forward * 768; //512

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


///////////////////////////
///////////////////////////
///////////////////////////


void    CFreeze :: Spawn( )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/can.mdl" );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "hornet" );
	m_flDie = gpGlobals->time + 2.75;
	pev->nextthink = gpGlobals->time + 0.1;
	pev->gravity			= 0.35;
	pev->friction			= 0.01;

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
	
	SetThink( MoveThink );
	SetTouch( MoveTouch );
}

void CFreeze :: Precache( void )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	PRECACHE_SOUND( "weapons/gravgren.wav" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	PRECACHE_MODEL( "models/can.mdl" );
}



void    CFreeze :: MoveThink( )
{

if (gpGlobals->time >= m_flDie) //full explode and self destroy
	{

		CBaseEntity *pEntity = NULL;
		Vector	vecDir;
		vecDir = Vector( 0, 0, 0 );
		Vector direction = Vector(0,0,1);
		
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL)
			{
				if ((pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP) && FVisible( pEntity ))
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

		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 30, 96, CLASS_NONE, DMG_GENERIC  ); //end blast
		
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
			WRITE_BYTE( TE_EXPLOSION );		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(8,16) + 7  ); // scale * 10
			WRITE_BYTE( RANDOM_LONG(8,10)  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		SetTouch( NULL );
		UTIL_Remove( this );
	}
	
	pev->nextthink = gpGlobals->time + 0.20; //0.15 old

}


void  CFreeze::MoveTouch( CBaseEntity *pOther )
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
		m_flDie = gpGlobals->time - 20;
		
	pev->velocity = pev->velocity*0.45;



		
}

////////////////////////////////////
///////NEW WEAPON - TRUE FREEZE BOMB
////////////////////////////////////



void    CFreezeBomb :: Spawn( void )
{


	SET_MODEL( ENT(pev), "models/bag.mdl" );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	UTIL_SetOrigin( pev, pev->origin );
	m_flDie = gpGlobals->time + 180;
	pev->takedamage = DAMAGE_YES;
	pev->gravity			= 0.55;
	pev->friction			= 0.35;
	pev->health			= 10100;
	pev->dmg = 10;
	
	pev->nextthink = gpGlobals->time + 4;
	SetTouch( Touch );
	SetThink( MoveThink );
}


void    CFreezeBomb :: Explode( )
{

	if (pev->dmg == 10)
		{
			pev->nextthink = gpGlobals->time + 1.35; //slow update
			
			
			if (pev->skin==0)
			{
				pev->velocity.z = 125; //jump
				pev->angles.y += 30; //rotate
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/bustconcrete1.wav", 0.5, ATTN_NORM);
			}
			else
			{

				EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "buttons/blip2.wav", 0.4, ATTN_NORM, 0, 150);
				pev->movetype = MOVETYPE_TOSS;
			
			}
			
		}
	else if (pev->skin==0)
		{
			pev->nextthink = gpGlobals->time + 0.02; //fast
			EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/medcharge4.wav", 1, ATTN_NORM, 0, pev->dmg*3);
		}
	else
		{
			pev->nextthink = gpGlobals->time + 0.02; //fast
			EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "debris/bustconcrete1.wav", .5, ATTN_NORM, 0, pev->dmg*3);
		}
	SetThink(MoveThink);
	
}


void    CFreezeBomb :: MoveThink( )
{

	CBaseEntity *pEntity = NULL;
	Explode();
	
	pev->movetype = MOVETYPE_TOSS;
	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL && (pev->dmg == 10))
       	{
		if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP)
			{
			if (FVisible( pEntity )) // && (pev->flags & FL_ONGROUND)
				{
					pev->classname = MAKE_STRING( "rpg_rocket" );
					pev->dmg = 11;
					pev->velocity.z = 275; //jump strong
					pev->nextthink = gpGlobals->time + 0.25;
				}
			}
		}
		
	//check for reaction
	if (pev->dmg != 10)
		{
			pev->dmg += 1;
			Exp();
		}
				



	if (gpGlobals->time >= m_flDie || pev->health <= 10000) //full explode and self destroy
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
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/beamstart9.wav", 1.0, ATTN_NORM);
		
			CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );	
			pl->m_flNextChatTime14 --;

		
			pev->nextthink = gpGlobals->time + 0.1;
			pev->takedamage = DAMAGE_NO;
			
			//Last check for explode
			if ( pev->skin == 1 )
			{
				::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 256, 512, CLASS_NONE, DMG_MORTAR  );
				//explode
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
					WRITE_COORD( pev->origin.x);
					WRITE_COORD( pev->origin.y);
					WRITE_COORD( pev->origin.z);
					WRITE_SHORT( g_sModelIndexFireball );
					WRITE_BYTE( 80  ); // scale * 10
					WRITE_BYTE( 16  ); // framerate
					WRITE_BYTE( TE_EXPLFLAG_NONE );
				MESSAGE_END();
				//smoke
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SMOKE );
					WRITE_COORD( pev->origin.x );
					WRITE_COORD( pev->origin.y );
					WRITE_COORD( pev->origin.z );
					WRITE_SHORT( g_sModelIndexSmoke );
					WRITE_BYTE( 72 ); // smoke scale * 10
					WRITE_BYTE( 24  ); // framerate
				MESSAGE_END();
				
			}
		
			SUB_Remove ();
		} 
}





void  CFreezeBomb::Touch( CBaseEntity *pOther )
{

	//stop moving
	//pev->velocity = pev->velocity * 0.25;
	//pev->friction = 1;
	//pev->avelocity = pev->avelocity * 0.9;
	pev->movetype = MOVETYPE_TOSS;
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 0.35;
	if (pev->solid = SOLID_BBOX && pev->skin == 0) //no stuck
		pev->solid = SOLID_NOT;
		
}

void  CFreezeBomb::Exp( )
{

	if (pev->dmg > 75 && pev->skin == 0) //delete object time, if freeze bag
		{
		//effects
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
		
		
		//freeze action
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL)
			{
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
					pEntity->FTime2 = gpGlobals->time + 10; //big freeze
					
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
		
		//
		SetTouch( NULL );
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/beamstart9.wav", 1.0, ATTN_NORM);
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "debris/beamstart9.wav", 1.0, ATTN_NORM );
		
		//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(vecSrc.x);	// X
			WRITE_COORD(vecSrc.y);	// Y
			WRITE_COORD(vecSrc.z);	// Z
			WRITE_BYTE( 16 );		// radius * 0.1
			WRITE_BYTE( 0 );		// r
			WRITE_BYTE( 92 );		// g
			WRITE_BYTE( 92 );		// b
			WRITE_BYTE( 64 );		// life * 10
			WRITE_BYTE( 0 );		// decay * 0.1
		MESSAGE_END( );
		
		pev->nextthink = gpGlobals->time + 0.1;
		pev->takedamage = DAMAGE_NO;
		
		//1.29 limit reset
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );	
		pl->m_flNextChatTime14 --;

		//delete
		SetThink( SUB_Remove );
	}
	if (pev->dmg > 75 && pev->skin == 1) //delete object time, if explode
	{
		//effects
		Vector direction = Vector(0,0,1);



		
		//others
		SetTouch( NULL );
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "zxc/explode3.wav", 1.0, ATTN_NORM);
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "zxc/explode3.wav", 1.0, ATTN_NORM );
		
		//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(vecSrc.x);	// X
			WRITE_COORD(vecSrc.y);	// Y
			WRITE_COORD(vecSrc.z);	// Z
			WRITE_BYTE( 16 );		// radius * 0.1
			WRITE_BYTE( 100 );		// r
			WRITE_BYTE( 12 );		// g
			WRITE_BYTE( 4 );		// b
			WRITE_BYTE( 32 );		// time * 10
			WRITE_BYTE( 16 );		// decay * 0.1
		MESSAGE_END( );
		
		pev->nextthink = gpGlobals->time + 0.1;
		pev->takedamage = DAMAGE_NO;
		
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 325, 475, CLASS_NONE, DMG_MORTAR  );
		
		//explode
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 80  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		//smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 72 ); // smoke scale * 10
			WRITE_BYTE( 24  ); // framerate
		MESSAGE_END();
			
		
		//1.29 limit reset
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );	
		pl->m_flNextChatTime14 --;

		//delete
		SetThink( SUB_Remove );
	}
		
	pev->angles.y += 30; //rotate

}
