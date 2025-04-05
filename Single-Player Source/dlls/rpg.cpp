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
#if !defined( OEM_BUILD )
#include "decals.h"
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include	"schedule.h"



enum rpg_e {
	RPG_IDLE = 0,
	RPG_FIDGET,
	RPG_RELOAD,		// to reload
	RPG_FIRE2,		// to empty
	RPG_HOLSTER1,	// loaded
	RPG_DRAW1,		// loaded
	RPG_HOLSTER2,	// unloaded
	RPG_DRAW_UL,	// unloaded
	RPG_IDLE_UL,	// unloaded idle
	RPG_FIDGET_UL,	// unloaded fidget
};

LINK_ENTITY_TO_CLASS( weapon_rpg, CRpg );
#ifndef CLIENT_DLL

LINK_ENTITY_TO_CLASS( laser_spot, CLaserSpot );


//=========================================================
//=========================================================
CLaserSpot *CLaserSpot::CreateSpot( void )
{
	CLaserSpot *pSpot = GetClassPtr( (CLaserSpot *)NULL );
	pSpot->Spawn();

	pSpot->pev->classname = MAKE_STRING("laser_spot");

	return pSpot;
}

//=========================================================
//=========================================================
void CLaserSpot::Spawn( void )
{
	Precache( );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;

	SET_MODEL(ENT(pev), "sprites/laserdot.spr");
	UTIL_SetOrigin( pev, pev->origin );
};

//=========================================================
// Suspend- make the laser sight invisible. 
//=========================================================
void CLaserSpot::Suspend( float flSuspendTime )
{
	pev->effects |= EF_NODRAW;
	
	SetThink( Revive );
	pev->nextthink = gpGlobals->time + flSuspendTime;
}

//=========================================================
// Revive - bring a suspended laser sight back.
//=========================================================
void CLaserSpot::Revive( void )
{
	pev->effects &= ~EF_NODRAW;

	SetThink( NULL );
}

void CLaserSpot::Precache( void )
{
	PRECACHE_MODEL("sprites/laserdot.spr");
};

LINK_ENTITY_TO_CLASS( rpg_rocket, CRpgRocket );

//=========================================================
//=========================================================
CRpgRocket *CRpgRocket::CreateRpgRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CRpg *pLauncher )
{
	CRpgRocket *pRocket = GetClassPtr( (CRpgRocket *)NULL );
	UTIL_SetOrigin( pRocket->pev, vecOrigin );
	pRocket->pev->angles = vecAngles;
	pRocket->Spawn();
	pRocket->SetTouch( CRpgRocket::RocketTouch );
	pRocket->m_pLauncher = pLauncher;// remember what RPG fired me. 
	pRocket->m_pLauncher->m_cActiveRockets++;// register this missile as active for the launcher
	pRocket->pev->owner = pOwner->edict();
	return pRocket;
	
	
}

//=========================================================
//=========================================================
void CRpgRocket :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/rpgrocket.mdl");
	UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin( pev, pev->origin );

	pev->classname = MAKE_STRING("rpg_rocket");

	SetThink( IgniteThink );
	SetTouch( ExplodeTouch );

	pev->angles.x -= 30;
	UTIL_MakeVectors( pev->angles );
	pev->angles.x = -(pev->angles.x + 30);

	pev->velocity = gpGlobals->v_forward * 250;
	pev->gravity = 0.5;
	pev->ltime = 255;

	pev->nextthink = gpGlobals->time + 0.4;

	pev->dmg = 150;
	

}

//=========================================================
//=========================================================
void CRpgRocket :: RocketTouch ( CBaseEntity *pOther )
{

	if ( m_pLauncher )
	{
		// my launcher is still around, tell it I'm dead.
		m_pLauncher->m_cActiveRockets--;
	}

	STOP_SOUND( edict(), CHAN_VOICE, "weapons/rocket1.wav" );
	ExplodeTouch( pOther );

}

//=========================================================
//=========================================================
void CRpgRocket :: Precache( void )
{
	PRECACHE_MODEL("models/rpgrocket.mdl");
	m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
	PRECACHE_SOUND ("weapons/rocket1.wav");
}


void CRpgRocket :: IgniteThink( void  )
{
	// pev->movetype = MOVETYPE_TOSS;

	pev->movetype = MOVETYPE_FLY;
	pev->effects |= EF_LIGHT;

	// make rocket sound
	//EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav", 1, 0.5 );

	// rocket trail
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex());	// entity
		WRITE_SHORT(m_iTrail );	// model
		WRITE_BYTE( 60 ); // life
		WRITE_BYTE( 3 );  // width
		WRITE_BYTE( pev->ltime );   // r, g, b
		WRITE_BYTE( pev->ltime );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 225 );	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	m_flIgniteTime = gpGlobals->time;

	// set to follow laser spot
	SetThink( FollowThink );
	pev->nextthink = gpGlobals->time + 0.1;
}


void CRpgRocket :: FollowThink( void  )
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget;
	Vector vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( pev->angles );

	vecTarget = gpGlobals->v_forward;
	flMax = 4096;
	
	// Examine all entities within a reasonable radius
	while ((pOther = UTIL_FindEntityByClassname( pOther, "laser_spot" )) != NULL)
	{
		UTIL_TraceLine ( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
		// ALERT( at_console, "%f\n", tr.flFraction );
		if (tr.flFraction >= 0.90)
		{
			vecDir = pOther->pev->origin - pev->origin;
			flDist = vecDir.Length( );
			vecDir = vecDir.Normalize( );
			flDot = DotProduct( gpGlobals->v_forward, vecDir );
			if ((flDot > 0) && (flDist * (1 - flDot) < flMax))
			{
				flMax = flDist * (1 - flDot);
				vecTarget = vecDir;
			}
		}
	}

	pev->angles = UTIL_VecToAngles( vecTarget );

	// this acceleration and turning math is totally wrong, but it seems to respond well so don't change it.
	float flSpeed = pev->velocity.Length();
	if (gpGlobals->time - m_flIgniteTime < 1.0)
	{
		pev->velocity = pev->velocity * 0.2 + vecTarget * (flSpeed * 0.8 + 400);
		if (pev->waterlevel == 3)
		{
			// go slow underwater
			if (pev->velocity.Length() > 300)
			{
				pev->velocity = pev->velocity.Normalize() * 300;
			}
			UTIL_BubbleTrail( pev->origin - pev->velocity * 0.1, pev->origin, 4 );
		} 
		else 
		{
			if (pev->velocity.Length() > 2000)
			{
				pev->velocity = pev->velocity.Normalize() * 2000;
			}
		}
	}
	else
	{
		if (pev->effects & EF_LIGHT)
		{
			pev->effects = 0;
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );
		}
		pev->velocity = pev->velocity * 0.2 + vecTarget * flSpeed * 0.798;
		if (pev->waterlevel == 0 && pev->velocity.Length() < 1500)
		{
			Detonate( );
		}
	}
	// ALERT( at_console, "%.0f\n", flSpeed );

	pev->nextthink = gpGlobals->time + 0.1;
}
#endif



void CRpg::Reload( void )
{
	int iResult;

	if ( m_iClip == 1 )
	{
		// don't bother with any of this if don't need to reload.
		return;
	}

	if ( m_pPlayer->ammo_rockets <= 0 )
		return;
	
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;

	if ( m_cActiveRockets && m_fSpotActive )
	{
		// no reloading when there are active missiles tracking the designator.
		// ward off future autoreload attempts by setting next attack time into the future for a bit. 
		return;
	}

#ifndef CLIENT_DLL
	if ( m_pSpot && m_fSpotActive )
	{
		m_pSpot->Suspend( 1.1 );
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.1;
	}
#endif

	if ( m_iClip == 0 )
		iResult = DefaultReload( RPG_MAX_CLIP, RPG_RELOAD, 2 );
	
	if ( iResult )
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	
}

void CRpg::Spawn( )
{
	Precache( );
	m_flNextHeavyTuretsTime = gpGlobals->time;
	m_iId = WEAPON_RPG;
	m_flNextTurretsLimit;

	SET_MODEL(ENT(pev), "models/w_rpg.mdl");
	m_fSpotActive = 1;

#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		// more default ammo in multiplay. 
		m_iDefaultAmmo = RPG_DEFAULT_GIVE * 2;
	}
	else
	{
		m_iDefaultAmmo = RPG_DEFAULT_GIVE;
	}

	FallInit();// get ready to fall down.
}


void CRpg::Precache( void )
{
	PRECACHE_MODEL("models/w_rpg.mdl");
	PRECACHE_MODEL("models/v_rpg.mdl");
	PRECACHE_MODEL("models/p_rpg.mdl");
	//PRECACHE_MODEL("models/conveyor1.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	UTIL_PrecacheOther( "laser_spot" );
	UTIL_PrecacheOther( "rpg_rocket" );

	PRECACHE_SOUND("weapons/rocketfire1.wav");
	//PRECACHE_SOUND("weapons/pinpull.wav");
	
	PRECACHE_SOUND("weapons/glauncher.wav"); // alternative fire sound

	m_usRpg = PRECACHE_EVENT ( 1, "events/rpg.sc" );
}


int CRpg::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "rockets";
	p->iMaxAmmo1 = ROCKET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = RPG_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_RPG;
	p->iFlags = 0;
	p->iWeight = RPG_WEIGHT;

	return 1;
}

int CRpg::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL CRpg::Deploy( )
{
	if (m_pPlayer->m_flNextTurretsLimit < 0)
		m_pPlayer->m_flNextTurretsLimit = 0;
	
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 320 );
	if ( m_iClip == 0 )
	{
		return DefaultDeploy( "models/v_rpg.mdl", "models/p_rpg.mdl", RPG_DRAW_UL, "rpg" );
	}

	return DefaultDeploy( "models/v_rpg.mdl", "models/p_rpg.mdl", RPG_DRAW1, "rpg" );
}


BOOL CRpg::CanHolster( void )
{
	if ( m_fSpotActive && m_cActiveRockets )
	{
		// can't put away while guiding a missile.
		return FALSE;
	}

	return TRUE;
}

void CRpg::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	SendWeaponAnim( RPG_HOLSTER1 );

#ifndef CLIENT_DLL
	if (m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
	}
#endif

}



void CRpg::PrimaryAttack()
{

	if ( m_iClip )
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		#ifndef CLIENT_DLL
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

			UTIL_MakeVectors( m_pPlayer->pev->v_angle );
			Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8;
			
			CRpgRocket *pRocket = CRpgRocket::CreateRpgRocket( vecSrc, m_pPlayer->pev->v_angle, m_pPlayer, this );

			UTIL_MakeVectors( m_pPlayer->pev->v_angle );// RpgRocket::Create stomps on globals, so remake.
			pRocket->pev->velocity = pRocket->pev->velocity + gpGlobals->v_forward * DotProduct( m_pPlayer->pev->velocity, gpGlobals->v_forward );
		#endif


		PLAYBACK_EVENT( FEV_GLOBAL, m_pPlayer->edict(), m_usRpg );

		m_iClip--; 
				
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
	UpdateSpot( );

}


void CRpg::SecondaryAttack()
{
//do not create the sentry in wall (< 1.26)

if ( m_iClip ) //if has 1 ammo after reloading, shot it
	{
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	TraceResult tr;
	Vector trace_origin;
	trace_origin = m_pPlayer->pev->origin;
	if ( m_pPlayer->pev->flags & FL_DUCKING )
		{
		trace_origin = trace_origin - ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
		}
	 

	 
	//experiment with trace hull 
	if (m_pPlayer->m_flNextTurretsLimit < m_limit) //limit turrets (5 normal, 3 sv_cheats)
	{
	UTIL_TraceHull( trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, ignore_monsters, head_hull, edict(), &tr );
	if ( !tr.fStartSolid ) //if ( tr.fStartSolid ) - sentry be created only in walls, use negative '!'
	{
	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	#ifndef CLIENT_DLL
	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecThrow = gpGlobals->v_forward;
	CBaseEntity *pHornet = CBaseEntity::Create( "monster_sentry", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, vecThrow, m_pPlayer->edict() );
	#endif
	//multiply hp x2
	if (g_zxc_cheats.value != 0)
		pHornet->pev->health = pHornet->pev->health * 2; 
	
	//simple test for future Tesla-gun
	// pHornet->SetAttachment2( m_pPlayer->edict(), 1 ); ///declared \ linked with cbase.h
/* 	
	m_pEyeGlow = CSprite::SpriteCreate( GARG_EYE_SPRITE_NAME, pev->origin, FALSE );
	m_pEyeGlow->SetTransparency( kRenderGlow, 255, 255, 255, 0, kRenderFxNoDissipation );
	m_pEyeGlow->SetAttachment( edict(), 1 );
	EyeOff();
	m_seeTime = gpGlobals->time + 5;
	m_flameTime = gpGlobals->time + 2;
*/

	
	UTIL_SetSize( pHornet->pev, Vector(0,0,0), Vector(0,0,0) ); // set size for created sentry, unused
	


	
	PLAYBACK_EVENT( FEV_GLOBAL, m_pPlayer->edict(), m_usRpg );
	m_iClip--; 
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_pPlayer->m_flNextTurretsLimit ++;
	}
	}
else
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
	UpdateSpot( );
	}
}


void CRpg::ThirdAttack()
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1)

	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		#ifndef CLIENT_DLL
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			UTIL_MakeVectors( m_pPlayer->pev->v_angle );
			Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * -17 + gpGlobals->v_up * -8;
			Vector vecSrc2 = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 17 + gpGlobals->v_up * -8;
			CRpgRocket *pRocket = CRpgRocket::CreateRpgRocket( vecSrc, m_pPlayer->pev->v_angle, m_pPlayer, this );
			CRpgRocket *pRocket2 = CRpgRocket::CreateRpgRocket( vecSrc2, m_pPlayer->pev->v_angle, m_pPlayer, this );
			pRocket->pev->dmg = 88;
			pRocket->pev->ltime = 80;
			pRocket->pev->nextthink = gpGlobals->time + 0.50; //post delay
			pRocket->pev->velocity = pRocket->pev->velocity + gpGlobals->v_forward * DotProduct( m_pPlayer->pev->velocity, gpGlobals->v_forward );

			pRocket2->pev->dmg = 89;
			pRocket2->pev->ltime = 250;
			pRocket2->pev->nextthink = gpGlobals->time + 0.40; //post delay
			pRocket2->pev->velocity = pRocket->pev->velocity + gpGlobals->v_forward * DotProduct( m_pPlayer->pev->velocity, gpGlobals->v_forward );
		
			UTIL_MakeVectors( m_pPlayer->pev->v_angle );// RpgRocket::Create stomps on globals, so remake.
		
		#endif
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--; 
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/rocketfire1.wav", 0.9, ATTN_NORM);
				
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
	}


}


void CRpg::FourthAttack()
{

	if (m_pPlayer->m_flNextHornetgunFreezebagLimit > 2)
		return;
	


	if (m_iClip)
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
		
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 430;

		CBaseEntity *pMine = Create( "player_freeze", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward + gpGlobals->v_right * 4 + gpGlobals->v_up * -8, Vector(0,0,0), m_pPlayer->edict() );

		pMine->pev->velocity = vecThrow;
		pMine->pev->skin = 1;
		pMine->pev->friction = 1.0;
		pMine->pev->movetype = MOVETYPE_TOSS;
		SET_MODEL( ENT(pMine->pev), "models/fungus(small).mdl" );
		UTIL_SetSize( pMine->pev, Vector( -8, -8, 0), Vector( 8, 8, 26 ) );

		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
		PLAYBACK_EVENT( FEV_GLOBAL, m_pPlayer->edict(), m_usRpg );
		m_iClip--; 
		m_pPlayer->m_flNextHornetgunFreezebagLimit ++;

		UpdateSpot( );
	}
}



void CRpg::WeaponIdle( void )
{
	UpdateSpot( );

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
	
	m_limit = 3;

	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0) 
		if (m_iClip >= 1)
		{
			// do not create in wall
			{
				UTIL_MakeVectors( m_pPlayer->pev->v_angle );
				TraceResult tr;
				Vector trace_origin;

				trace_origin = m_pPlayer->pev->origin;
				if ( m_pPlayer->pev->flags & FL_DUCKING )
				{
					trace_origin = trace_origin - ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
				}

				UTIL_TraceLine( trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr );

				if ( !tr.fAllSolid && !tr.fStartSolid )
				{
				if (m_pPlayer->m_flNextTurretsLimit < m_limit)
					{
						{
						if (  m_pPlayer->m_flNextHeavyTuretsTime < gpGlobals->time ) //need delay
							{
							m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
							m_pPlayer->m_flNextHeavyTuretsTime = gpGlobals->time + 3;
							m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
							m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

							// player "shoot" animation
							m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
							UTIL_MakeVectors( m_pPlayer->pev->v_angle );
							Vector vecThrow = gpGlobals->v_forward;
							m_pPlayer->m_flNextTurretsLimit ++;
							CBaseEntity *pHornet = CBaseEntity::Create( "monster_turret", pev->origin, vecThrow, m_pPlayer->edict() );
							//multiply hp x2
							if (g_zxc_cheats.value != 0)
								pHornet->pev->health = pHornet->pev->health * 1.5; 

							PLAYBACK_EVENT( FEV_GLOBAL, m_pPlayer->edict(), m_usRpg );
							m_iClip--; 
							m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 1;
							m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
							m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
							m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
						}

						else
						{
							PlayEmptySound( );
							m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
							m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
						}
						UpdateSpot( );
						return;
						}
					}
				}
			}
		}
	// reload completed

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;
		
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
		
		
		if (flRand <= 0.75 || m_fSpotActive)
		{
			if ( m_iClip == 0 )
				iAnim = RPG_IDLE_UL;
			else
				iAnim = RPG_IDLE;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		}
		else
		{
			if ( m_iClip == 0 )
				iAnim = RPG_FIDGET_UL;
			else
				iAnim = RPG_FIDGET;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		}

		SendWeaponAnim( iAnim );
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	}
	char  szText[24];
	hudtextparms_t hText;
	sprintf(szText,  "Turrets left: %d", INT(  max(0, m_limit-m_pPlayer->m_flNextTurretsLimit)  ) ); 
	hText.channel = 16;
	hText.x = 0.90;
	hText.y = 0.86;
	hText.effect = 0; // Fade in/out
	hText.r1 = 255;
	hText.g1 = hText.b1 = 150;
	hText.a1 = 20;
	hText.fadeinTime = 0.2;
	hText.fadeoutTime = 0.5;
	hText.holdTime = 2.0;
	hText.fxTime = 2.0;
	if (  m_pPlayer->pevEntity!=NULL ) UTIL_HudMessage(m_pPlayer->pevEntity, hText, szText);
}



void CRpg::UpdateSpot( void )
{

#ifndef CLIENT_DLL
	if (m_fSpotActive)
	{
		if (!m_pSpot)
		{
			m_pSpot = CLaserSpot::CreateSpot();
		}

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecSrc = m_pPlayer->GetGunPosition( );
		Vector vecAiming = gpGlobals->v_forward;
		
		TraceResult tr;
		UTIL_TraceLine ( vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );
		
		UTIL_SetOrigin( m_pSpot->pev, tr.vecEndPos );

		if ( m_flNextSBarUpdateTimeR < gpGlobals->time )
		{
			// follow line 
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_BEAMENTS );
				WRITE_SHORT( ENTINDEX( m_pPlayer->edict() ) );
				WRITE_SHORT( ENTINDEX( m_pSpot->edict() ) );
				WRITE_SHORT( g_sModelIndexLaser );
				WRITE_BYTE( 0 ); // framestart
				WRITE_BYTE( 0 ); // framerate
				WRITE_BYTE( 1 ); // life
				WRITE_BYTE( 5 );  // width
				WRITE_BYTE( 0 );   // noise
				WRITE_BYTE( 34 );   // r, g, b
				WRITE_BYTE( 0 );   // r, g, b
				WRITE_BYTE( 0 );   // r, g, b +pSightEnt->pev->health
				WRITE_BYTE( 200 );	// brightness
				WRITE_BYTE( 0 );		// speed
			MESSAGE_END();	
			::RadiusDamage( tr.vecEndPos, pev, VARS( pev->owner ), 1.2, 16, CLASS_NONE, DMG_BULLET  ); //end blast
			UTIL_DecalTrace( &tr, DECAL_GUNSHOT1 );
			m_flNextSBarUpdateTimeR = gpGlobals->time + 0.1; // 1 frame per sec
		}
	}
#endif

}


class CRpgAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_rpgammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_rpgammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int iGive;

#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
		{
			// hand out more ammo per rocket in multiplayer.
			iGive = 1;
		}
		else
		{
			iGive = AMMO_RPGCLIP_GIVE;
		}

		if (pOther->GiveAmmo( iGive, "rockets", ROCKET_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_rpgclip, CRpgAmmo );

#endif














