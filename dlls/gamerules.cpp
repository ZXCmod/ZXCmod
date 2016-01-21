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
//=========================================================
// GameRules.cpp
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"skill.h"
#include	"game.h"

extern edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );

DLL_GLOBAL CGameRules*	g_pGameRules = NULL;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgMOTD;

int g_teamplay = 0;

//=========================================================
//=========================================================
BOOL CGameRules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	int iAmmoIndex;

	if ( pszAmmoName )
	{
		iAmmoIndex = pPlayer->GetAmmoIndex( pszAmmoName );

		if ( iAmmoIndex > -1 )
		{
			if ( pPlayer->AmmoInventory( iAmmoIndex ) < iMaxCarry )
			{
				// player has room for more of this type of ammo
				return TRUE;
			}
		}
	}

	return FALSE;
}

//=========================================================
//=========================================================
edict_t *CGameRules :: GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = EntSelectSpawnPoint( pPlayer );

	pPlayer->pev->origin = VARS(pentSpawnSpot)->origin + Vector(0,0,1);
	pPlayer->pev->v_angle  = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = VARS(pentSpawnSpot)->angles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = TRUE;
	
	return pentSpawnSpot;
}

//=========================================================
//=========================================================
BOOL CGameRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	// only living players can have items
	if ( pPlayer->pev->deadflag != DEAD_NO )
		return FALSE;

	if ( pWeapon->pszAmmo1() )
	{
		if ( !CanHaveAmmo( pPlayer, pWeapon->pszAmmo1(), pWeapon->iMaxAmmo1() ) )
		{
			// we can't carry anymore ammo for this gun. We can only 
			// have the gun if we aren't already carrying one of this type
			if ( pPlayer->HasPlayerItem( pWeapon ) )
			{
				return FALSE;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if ( pPlayer->HasPlayerItem( pWeapon ) )
		{
			return FALSE;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return TRUE;
}

//=========================================================
// load the SkillData struct with the proper values based on the skill level.
//=========================================================
void CGameRules::RefreshSkillData ( void )
{
	int	iSkill;

	iSkill = (int)CVAR_GET_FLOAT("skill");
	g_iSkillLevel = iSkill;

	if ( iSkill < 1 )
	{
		iSkill = 1;
	}
	else if ( iSkill > 3 )
	{
		iSkill = 3; 
	}

	gSkillData.iSkillLevel = iSkill;

	ALERT ( at_console, "\nGAME SKILL LEVEL:%d\n",iSkill );

	// Barney
	gSkillData.barneyHealth = 100;

	// Big Momma
	gSkillData.bigmommaHealthFactor = 1000;
	gSkillData.bigmommaDmgSlash = 15;
	gSkillData.bigmommaDmgBlast = 15;
	gSkillData.bigmommaRadiusBlast = 15;

	// Bullsquid
	gSkillData.bullsquidHealth = 100;
	gSkillData.bullsquidDmgBite = 15;
	gSkillData.bullsquidDmgWhip = 15;
	gSkillData.bullsquidDmgSpit = 15;

	// Gargantua
	gSkillData.gargantuaHealth = 2000;
	gSkillData.gargantuaDmgSlash = 20;
	gSkillData.gargantuaDmgFire = 1;
	gSkillData.gargantuaDmgStomp = 15;


	// Headcrab
	gSkillData.headcrabHealth = 50;
	gSkillData.headcrabDmgBite = 7;



	// Houndeye
	gSkillData.houndeyeHealth = 100;
	gSkillData.houndeyeDmgBlast = 7;

	// ISlave
	// gSkillData.slaveHealth = 100;
	gSkillData.slaveDmgClaw = 77;
	gSkillData.slaveDmgClawrake = 94;
	// gSkillData.slaveDmgZap = RANDOM_LONG(37,73);

	// Icthyosaur
	gSkillData.ichthyosaurHealth = 4;
	gSkillData.ichthyosaurDmgShake = 4;

	// Leech
	gSkillData.leechHealth = 100;

	gSkillData.leechDmgBite = 4;


	// Nihilanth
	gSkillData.nihilanthHealth = 100;
	gSkillData.nihilanthZap = 7;

	// Zombie
	gSkillData.zombieDmgOneSlash = 7;
	gSkillData.zombieDmgBothSlash = 15;

	//Turret
	gSkillData.turretHealth = 100;

	// MiniTurret
	gSkillData.miniturretHealth = 100;
	
	// Sentry Turret
	gSkillData.sentryHealth = 75;

// PLAYER WEAPONS

	// Crowbar whack
///	gSkillData.plrDmgCrowbar = GetSkillCvar( "sk_plr_crowbar");

	// Glock Round
	gSkillData.plrDmg9MM = 10;

	// 357 Round
	gSkillData.plrDmg357 = 100;

	// MP5 Round
	gSkillData.plrDmgMP5 = 15;

	// M203 grenade
	gSkillData.plrDmgM203Grenade = 120;

	// Shotgun buckshot
	gSkillData.plrDmgBuckshot = 7;

	// Crossbow
///	gSkillData.plrDmgCrossbowClient = GetSkillCvar( "sk_plr_xbow_bolt_client");
	gSkillData.plrDmgCrossbowMonster = 100;

	// RPG
	gSkillData.plrDmgRPG = 120;

	// Gauss gun
	gSkillData.plrDmgGauss = 17;

	// Hand Grendade
	gSkillData.plrDmgHandGrenade = 120;

	// Satchel Charge
	gSkillData.plrDmgSatchel = 120;

	// Tripmine
	gSkillData.plrDmgTripmine = 120;



	// PLAYER HORNET
// Up to this point, player hornet damage and monster hornet damage were both using
// monDmgHornet to determine how much damage to do. In tuning the hivehand, we now need
// to separate player damage and monster hivehand damage. Since it's so late in the project, we've
// added plrDmgHornet to the SKILLDATA struct, but not to the engine CVar list, so it's inaccesible
// via SKILLS.CFG. Any player hivehand tuning must take place in the code. (sjb)
	gSkillData.plrDmgHornet = 9;


	// HEALTH/CHARGE
	gSkillData.suitchargerCapacity = 50;


}

//=========================================================
// instantiate the proper game rules object
//=========================================================

CGameRules *InstallGameRules( void )
{
	SERVER_COMMAND( "exec server.cfg\n" );
	SERVER_EXECUTE( );

	if ( !gpGlobals->deathmatch )
	{
		// generic half-life
		g_teamplay = 0;
		return new CHalfLifeRules;
	}
	else
	{
		if ( teamplay.value > 0 )
		{
			// teamplay

			g_teamplay = 1;
			return new CHalfLifeTeamplay;
		}
		if ((int)gpGlobals->deathmatch == 1)
		{
			// vanilla deathmatch
			g_teamplay = 0;
			return new CHalfLifeMultiplay;
		}
		else
		{
			// vanilla deathmatch??
			g_teamplay = 0;
			return new CHalfLifeMultiplay;
		}
	}
}

/*Chip - We want a teamplay mod
CGameRules *InstallGameRules( void )
{
    SERVER_COMMAND( "exec game.cfg\n" );
    SERVER_EXECUTE( ); 

    if ( !gpGlobals->deathmatch )
    {
        //No deathmatch defined but we want it anyways.
        gpGlobals->deathmatch = TRUE;
        return new CHalfLifeTeamplay;
    }
    else
    {
        //Deathmatch, so let's start Teamplay
        return new CHalfLifeTeamplay;
    }
} 
*/
