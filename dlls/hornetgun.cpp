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
#include "hornet.h"
#include "gamerules.h"
#include "shake.h"
#include "hornetgun.h"

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
	PRECACHE_SOUND( "ambience/dronemachine2.wav" );
	
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");
	PRECACHE_MODEL("models/bag.mdl");
	
	m_usHornetFire = PRECACHE_EVENT ( 1, "events/firehornet.sc" );

	UTIL_PrecacheOther("hornet");
}

int CHgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = HORNET_MAX_CARRY;

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
	m_pPlayer->m_flPlayAftershock = 0.0;

	// m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 1; //ammo set to 1. 
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 270 );
	return DefaultDeploy( "models/v_hgun.mdl", "models/p_hgun.mdl", HGUN_UP, "hive" );
}

void CHgun::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( HGUN_DOWN );
	m_fInAttack = 0;

	//!!!HACKHACK - can't select hornetgun if it's empty! no way to get ammo for it, either.
	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;
	}
}


void CHgun::PrimaryAttack()
{

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		return;
	}
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );

	CBaseEntity::Create( "hornet", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );

	m_flRechargeTime = gpGlobals->time + 0.5;
	
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=1;
	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;


	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_TRACK, 0, 0, 0 );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	
	Reload( );

}







float CHgun::GetFullChargeTime( void )
{

	return 0.5;
	
}

void CHgun::SecondaryAttack( void )
{
	if (allowmonsters9.value == 0)
		return;

	if ( m_fInAttack == 0 )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.2;
			return;
		}

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();
		
		SendWeaponAnim( 1 );
		
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim( 1 );
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn  && m_pPlayer->m_flNextAmmoBurn != 1500 )
		{
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
			m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.15;
		}

		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
			return;
		}
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
			{
				// don't eat any more ammo after gun is fully charged.
				m_pPlayer->m_flNextAmmoBurn = 1500;
			}


		int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 15 / GetFullChargeTime() );
		if ( pitch > 175 ) 
			 pitch = 175;
			 

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "ambience/dronemachine2.wav", 0.5, ATTN_NORM, 0, pitch);
			
		
		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 5 )
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
		
			m_pPlayer->TakeDamage( VARS(eoNullEntity), VARS(eoNullEntity), 45, DMG_PARALYZE );
			SendWeaponAnim( 1 );
			
			UTIL_ScreenFade( m_pPlayer, Vector(0,0,255), 5.0, 2.0, 128, FFADE_IN );
			m_pPlayer->pev->rendermode = kRenderNormal;
			m_pPlayer->pev->renderfx = kRenderFxGlowShell;
			m_pPlayer->pev->rendercolor.x = 200;  // red
			m_pPlayer->pev->rendercolor.y = 200;  // green
			m_pPlayer->pev->rendercolor.z = 255; // blue
			m_pPlayer->pev->renderamt = 70;
			m_pPlayer->EnableControl(FALSE);
			m_pPlayer->FTime2 = gpGlobals->time + 3.6; 
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
			
		}
	}
}
	
void CHgun::StartFire( void )
{

	float flDamage;
    Vector vecSrc, vecAim, vecOrig;
    TraceResult tr;
	Vector vecDir;
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
    vecOrig = m_pPlayer->GetGunPosition( );
    vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;
    vecAim = gpGlobals->v_forward * 5; 
    UTIL_TraceLine ( vecOrig, vecOrig + vecAim * 2048, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr);

	flDamage = 110.0 * (( gpGlobals->time - m_pPlayer->m_flStartCharge)*0.01 );
	
	if (m_fInAttack != 3)
	{
		float flZVel = m_pPlayer->pev->velocity.z;
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * (flDamage*360);
	}

	Fire( vecSrc, vecAim, flDamage, tr );
}

// event secondary attack
void CHgun::Fire( Vector vecOrigSrc, Vector vecDir, float flDamage, TraceResult tr )
{
    CBaseEntity *pEntity;
	
	// freeze ray
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD(vecOrigSrc.x);
		WRITE_COORD(vecOrigSrc.y);
		WRITE_COORD(vecOrigSrc.z);
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_SHORT( BSpr ); //sprite
		WRITE_BYTE( 0 ); // Starting frame
		WRITE_BYTE( 0  ); // framerate * 0.1
		WRITE_BYTE( 2 ); // life * 0.1
		WRITE_BYTE( flDamage*20 ); // width
		WRITE_BYTE( 0 ); // noise
		WRITE_BYTE( 100 ); // color r,g,b
		WRITE_BYTE( 100 ); // color r,g,b
		WRITE_BYTE( 255 ); // color r,g,b
		WRITE_BYTE( 240 ); // brightness
		WRITE_BYTE( 0 ); // scroll speed
	MESSAGE_END();

	// draw light
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecOrigSrc );
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

	m_pPlayer->pev->punchangle.x -= 4;
	m_pPlayer->pev->punchangle.y += RANDOM_LONG(-4,2);
	
	m_flRechargeTime = gpGlobals->time + 0.3;
    pEntity = CBaseEntity::Instance(tr.pHit);

	// don't hit transparent objects
    if (pEntity != NULL && (pEntity->IsBSPModel()))
    {
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.15; //delay
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_FAST, 0, 0, 0 );
		return;
	}
	
	// freeze begin
    if (pEntity != NULL && !(pEntity->IsPlayer())) //in 1.26 allow to freeze monsters
    {
        pEntity->pev->rendermode = kRenderNormal;
        pEntity->pev->renderfx = kRenderFxGlowShell;
        pEntity->pev->rendercolor.x = 100;  // red
        pEntity->pev->rendercolor.y = 100;  // green
        pEntity->pev->rendercolor.z = 255; // blue
        pEntity->pev->renderamt = 70;
        pEntity->FTime2 = gpGlobals->time + flDamage; //freeze timer of monters
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25; //delay
    }
	
	if (pEntity != NULL && pEntity->IsPlayer()) // is player
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
		pPlayer->FTime2 = gpGlobals->time + flDamage; 
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flDamage;
	}
	
	pEntity->TakeDamage(pev, VARS( pev->owner ), flDamage*20, DMG_PARALYZE);
	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usHornetFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, FIREMODE_FAST, 0, 0, 0 );
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	

}

void CHgun::ThirdAttack( void )
{
	if (allowmonsters9.value == 0)
		return;

	if (allowmonsters10.value == 1)
		{
		if (m_pPlayer->m_flNextChatTime14 > 12)
			return;
		}
	else
		{
		if (m_pPlayer->m_flNextChatTime14 > 2)
			return;
		}
	
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 10)
	{
		int iAnim;
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 512;

		CBaseEntity *pFreeze = Create( "player_freeze", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
		pFreeze->pev->velocity = vecThrow;
		pFreeze->pev->skin = 0; // set freeze bag type
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		iAnim = HGUN_FIDGETSHAKE;
		SendWeaponAnim( iAnim );
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 10;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_flRechargeTime = gpGlobals->time + 0.5;
		m_pPlayer->m_flNextChatTime14 ++;

		// play sounds
		switch(RANDOM_LONG(0,1))
		{
			case 0: 
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.9, ATTN_NORM);
			break;
			case 1: 
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher2.wav", 0.9, ATTN_NORM);
			break;
		}
	}
}

void CHgun::FourthAttack( void )
{

	if (m_pPlayer->pev->health <= 2 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	int Radius = 800;

	CBaseEntity *pEntity;
	TraceResult	tr;	
	TraceResult	trVis;	
	
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	
	Vector vecSrcVis;
	vecSrcVis = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;
	Vector vecSrc;
	vecSrc = m_pPlayer->GetGunPosition( );
	
	Vector vecDir = gpGlobals->v_forward;
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * Radius, dont_ignore_monsters, ENT(pev), &tr);
	
	pEntity = CBaseEntity::Instance(tr.pHit); // trace hit to entity
	
	if (pEntity != NULL && !pEntity->IsAlive())
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5; 
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "buttons/blip1.wav", 0.75, ATTN_NORM);
	}
	
	if (pEntity->pev->health >= pEntity->pev->max_health )
		return;

	int iAnim;

	if (pEntity != NULL && (pEntity->pev->flags & (FL_CLIENT | FL_MONSTER)) ) //  && pEntity->IsPlayer()
	{
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		iAnim = HGUN_FIDGETSHAKE;
		SendWeaponAnim( iAnim );
		 
		// recover limit health
		if (pEntity->pev->max_health < 100)
			pEntity->pev->max_health += 5;
		
		// heal anyone
		if (pEntity->pev->health <= pEntity->pev->max_health )
		{
			pEntity->TakeHealth(2, DMG_PARALYZE);
			UTIL_ScreenFade( pEntity, Vector(0,175,50), 1, 0.16, 16, FFADE_IN ); //to target
		}
		
		// ray
		UTIL_TraceLine(vecSrcVis, vecSrcVis + vecDir * Radius, dont_ignore_monsters, ENT(pev), &trVis);
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMPOINTS );
			WRITE_COORD(vecSrcVis.x);
			WRITE_COORD(vecSrcVis.y);
			WRITE_COORD(vecSrcVis.z);
			WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
			WRITE_COORD( pEntity->pev->origin.y );
			WRITE_COORD( pEntity->Center().z );
			WRITE_SHORT( BSpr ); //sprite
			WRITE_BYTE( 0 ); // Starting frame
			WRITE_BYTE( 0  ); // framerate * 0.1
			WRITE_BYTE( 1 ); // life * 0.1
			WRITE_BYTE( 10 ); // width
			WRITE_BYTE( 0 ); // noise
			WRITE_BYTE( 0 ); // color r,g,b
			WRITE_BYTE( 175 ); // color r,g,b
			WRITE_BYTE( 50 ); // color r,g,b
			WRITE_BYTE( 100 ); // brightness
			WRITE_BYTE( 0 ); // scroll speed
		MESSAGE_END();
		
		// lights target
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(pEntity->pev->origin.x);	// X
			WRITE_COORD(pEntity->pev->origin.y);	// Y
			WRITE_COORD(pEntity->pev->origin.z);	// Z
			WRITE_BYTE( 16 );		// radius * 0.1
			WRITE_BYTE( 0 );		// r
			WRITE_BYTE( 200 );		// g
			WRITE_BYTE( 100 );		// b
			WRITE_BYTE( 8 );		// life * 10
			WRITE_BYTE( 32 );		// decay * 0.1
		MESSAGE_END( );
		
		// lights healer
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(vecSrc.x);	// X
			WRITE_COORD(vecSrc.y);	// Y
			WRITE_COORD(vecSrc.z);	// Z
			WRITE_BYTE( 16 );		// radius * 0.1
			WRITE_BYTE( 0 );		// r
			WRITE_BYTE( 200 );		// g
			WRITE_BYTE( 200 );		// b
			WRITE_BYTE( 8 );		// life * 10
			WRITE_BYTE( 32 );		// decay * 0.1
		MESSAGE_END( );
		
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "debris/beamstart4.wav", 0.75, ATTN_NORM);

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1; 
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;

	}
}


void CHgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= HORNET_MAX_CARRY)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < HORNET_MAX_CARRY && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 0.4;
		STOP_SOUND( ENT(m_pPlayer->pev), CHAN_ITEM, "ambience/dronemachine2.wav" );
	}
}


void CHgun::WeaponIdle( void )
{
	Reload( );

	// this block is reload key
	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 4) 
		{
		if (  m_pPlayer->m_flNextChatTime8 < gpGlobals->time ) //need delay
			{
			if (allowmonsters9.value == 0)
				return;

			m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
			m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

			Vector vecSrc = m_pPlayer->pev->origin;
			Vector vecThrow = gpGlobals->v_forward * 768;

			CBaseEntity *pSatchel = Create( "weapon_clip_generic", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSatchel->pev->velocity = vecThrow;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 4;
			m_pPlayer->m_flNextChatTime8 = gpGlobals->time + 1.0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flRechargeTime = gpGlobals->time + 0.5;
			
			switch(RANDOM_LONG(0,1))
				{
				case 0: 
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.9, ATTN_NORM);
				break;
				case 1: 
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher2.wav", 0.9, ATTN_NORM);
				break;
				}
			}
		}
	
	// for secondary fire
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;
	
	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	}

}




///////////////////////////
///// FLASH CANS
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
	pev->dmg				= 25;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT( m_LaserSprite ); // model
		WRITE_BYTE( 16 ); // life
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 64 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 64 ); // brightness
	MESSAGE_END(); 
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT( g_sModelIndexLaser ); // model
		WRITE_BYTE( 5 ); // life
		WRITE_BYTE( 1 ); // width
		WRITE_BYTE( 100 ); // r, g, b
		WRITE_BYTE( 100 ); // r, g, b
		WRITE_BYTE( 200 ); // r, g, b
		WRITE_BYTE( 220 ); // brightness
	MESSAGE_END(); 
	
	SetThink( MoveThink );
	SetTouch( MoveTouch );
}

void CFreeze :: Precache( void )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
}



void    CFreeze :: MoveThink( )
{

// full explode and self destroy
if (gpGlobals->time >= m_flDie) 
	{

		CBaseEntity *pEntity = NULL;
		Vector	vecDir;
		vecDir = Vector( 0, 0, 0 );
		Vector direction = Vector(0,0,1);
		
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL)
			{
				if ((pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP) && FVisible( pEntity ))
				{
					CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner); // init pOwner
					
					vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
					pEntity->pev->velocity = pEntity->pev->velocity + vecDir * 2048;
					::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 512, CLASS_NONE, DMG_PARALYZE  );
					
					UTIL_ScreenShake( pEntity->pev->origin, 12.0, 120, 0.9, 1 );
					if ( (g_pGameRules->PlayerRelationship( pOwner, pEntity ) != GR_TEAMMATE)) // not ally
						UTIL_ScreenFade( pEntity, Vector(255,255,200), 8, 0.84, 254, FFADE_IN ); // strong flash 
					else
						UTIL_ScreenFade( pEntity, Vector(255,255,200), 8, 0.84, 100, FFADE_IN ); // weak flash 
				
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

		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 96, CLASS_NONE, DMG_PARALYZE  ); //end blast
		
		// 1st disk
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMDISK );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 16);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 880 ); // reach damage radius over .2 seconds
			WRITE_SHORT( g_sModelIndexBubbles );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 3 ); // life
			WRITE_BYTE( 12 );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 100 );   // r, g, b
			WRITE_BYTE( 100 );   // r, g, b
			WRITE_BYTE( 150 );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 2 );		// speed
		MESSAGE_END();
		
		// 2nd
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMDISK );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 24);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z - 800 ); // reach damage radius over .2 seconds
			WRITE_SHORT( g_sModelIndexLaserDot );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 2 ); // life
			WRITE_BYTE( 92 );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 100 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();

		// balls
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z + 20 );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z + 16  );
			WRITE_SHORT( g_sModelIndexLaserDot );		// model
			WRITE_BYTE( 6  );				// count
			WRITE_BYTE( 2 );				// life * 10
			WRITE_BYTE( 2 );				// size * 10
			WRITE_BYTE( 170 );				// amplitude * 0.1
			WRITE_BYTE( 16 );				// speed * 100
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
			WRITE_BYTE( 17  ); // scale * 10
			WRITE_BYTE( 9  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		SetTouch( NULL );
		UTIL_Remove( this );
	}
	
	pev->nextthink = gpGlobals->time + 0.20; //0.15 old

}


void  CFreeze::MoveTouch( CBaseEntity *pOther )
{

	pev->velocity.z += 24; //jump

	if ( pOther->pev->solid != SOLID_BSP) 
		m_flDie = gpGlobals->time - 20;
		
	pev->velocity = pev->velocity*0.45;

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
}

////////////////////////////////////
/////// FREEZE BOMB
////////////////////////////////////

void    CFreezeBomb :: Spawn( void )
{

	SET_MODEL( ENT(pev), "models/bag.mdl" );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	UTIL_SetOrigin( pev, pev->origin );
	m_flDie = gpGlobals->time + 180;
	
	pev->takedamage 		= DAMAGE_YES;
	pev->gravity			= 0.55;
	pev->friction			= 0.35;
	pev->health				= 10100;
	pev->dmg 				= 256;
	pev->ltime 				= 10;
	
	pev->nextthink = gpGlobals->time + 4;
	SetTouch( Touch );
	SetThink( MoveThink );
}


void    CFreezeBomb :: Explode( )
{

	if (pev->ltime == 10)
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
			pev->nextthink = gpGlobals->time + 0.02; 
			EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/medcharge4.wav", 1, ATTN_NORM, 0, pev->ltime*3);
		}
	else
		{
			pev->nextthink = gpGlobals->time + 0.02; 
			EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "debris/bustconcrete1.wav", .5, ATTN_NORM, 0, pev->ltime*3);
		}
	SetThink(MoveThink);
	
}


void    CFreezeBomb :: MoveThink( )
{

	CBaseEntity *pEntity = NULL;
	Explode();
	
	pev->movetype = MOVETYPE_TOSS;
	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL && (pev->ltime == 10))
       	{
		if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP)
			{
			if (FVisible( pEntity )) // && (pev->flags & FL_ONGROUND)
				{
					pev->classname = MAKE_STRING( "rpg_rocket" );
					pev->ltime = 11;
					pev->velocity.z = 275; //jump strong
					pev->nextthink = gpGlobals->time + 0.25;
				}
			}
		}
		
	//check for reaction
	if (pev->ltime != 10)
		{
			pev->ltime += 1;
			Exp();
		}
				
	if (gpGlobals->time >= m_flDie || pev->health <= 10000) //full explode and self destroy
		{
			// explosion
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
				WRITE_COORD( pev->origin.x );
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_SHORT( g_sModelIndexFireball );
				WRITE_BYTE( 15  ); // scale * 10
				WRITE_BYTE( 9  ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/beamstart9.wav", 1.0, ATTN_NORM);
		
			CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );	
			pl->m_flNextChatTime14 --;

			pev->nextthink = gpGlobals->time + 0.1;
			pev->takedamage = DAMAGE_NO;
			
			// Last check for explode
			if ( pev->skin == 1 )
			{
				::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 512, CLASS_NONE, DMG_PARALYZE  );
				
				// explode
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
				// smoke
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
	pev->movetype = MOVETYPE_TOSS;
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 0.35;
	if (pev->solid = SOLID_BBOX && pev->skin == 0) //no stuck
		pev->solid = SOLID_NOT;
		
}

void  CFreezeBomb::Exp( )
{

	if (pev->ltime > 75 && pev->skin == 0) //delete object time, if freeze bag
	{
		Vector direction = Vector(0,0,1);
		// spark effects
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

		//freeze action
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 300 )) != NULL)
			{
				if (pEntity->pev->takedamage != pEntity->IsPlayer()) //in 1.26 allow to freeze monsters
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
					
					if (pEntity->pev->takedamage && pEntity->IsPlayer()) //check only player
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
		
		Explode();
		
		SetTouch( NULL );
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/beamstart9.wav", 1.0, ATTN_NORM);
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "debris/beamstart9.wav", 1.0, ATTN_NORM );
		
		//lights
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(pev->origin.x);	// X
			WRITE_COORD(pev->origin.y);	// Y
			WRITE_COORD(pev->origin.z);	// Z
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
	
	if (pev->ltime > 75 && pev->skin == 1) //delete object time, if explode
	{
		// others
		SetTouch( NULL );
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "zxc/explode3.wav", 1.0, ATTN_NORM);
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "zxc/explode3.wav", 1.0, ATTN_NORM );
		
		// lights
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(pev->origin.x);	// X
			WRITE_COORD(pev->origin.y);	// Y
			WRITE_COORD(pev->origin.z);	// Z
			WRITE_BYTE( 16 );		// radius * 0.1
			WRITE_BYTE( 100 );		// r
			WRITE_BYTE( 12 );		// g
			WRITE_BYTE( 4 );		// b
			WRITE_BYTE( 32 );		// time * 10
			WRITE_BYTE( 16 );		// decay * 0.1
		MESSAGE_END( );
		
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 128+pev->dmg, 475, CLASS_NONE, DMG_CRUSH  );
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_EXPLODEMODEL	 );
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(pev->origin.x);///
			WRITE_SHORT(g_sModelIndexBloodDrop); // entity
			WRITE_SHORT( 30 ); // model
			WRITE_BYTE( 16 ); // life
		MESSAGE_END();
		
		// explode
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
		
		// smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 72 ); // smoke scale * 10
			WRITE_BYTE( 24  ); // framerate
		MESSAGE_END();
			
		
		// 1.29 limit reset
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );	
		pl->m_flNextChatTime14 --;
		
		pev->nextthink = gpGlobals->time + 0.1;
		pev->takedamage = DAMAGE_NO;

		SetThink( SUB_Remove );
	}
		
	pev->angles.y += 30; //rotate

}
