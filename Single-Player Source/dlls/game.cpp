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
#include "eiface.h"
#include "util.h"
#include "game.h"

cvar_t	displaysoundlist = {"displaysoundlist","0"};

// multiplayer server rules
cvar_t	fragsleft	= {"mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	timeleft	= {"mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "

// multiplayer server rules
cvar_t	teamplay	= {"mp_teamplay","1", FCVAR_SERVER };
cvar_t	fraglimit	= {"mp_fraglimit","50", FCVAR_SERVER };
cvar_t	timelimit	= {"mp_timelimit","30", FCVAR_SERVER };
cvar_t	friendlyfire= {"mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage	= {"mp_falldamage","0", FCVAR_SERVER };
//cvar_t	weaponstay	= {"mp_weaponstay","0", FCVAR_SERVER };
cvar_t	forcerespawn= {"mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight	= {"mp_flashlight","1", FCVAR_SERVER };
cvar_t	aimcrosshair= {"mp_autocrosshair","0", FCVAR_SERVER };
cvar_t	decalfrequency = {"decalfrequency","20", FCVAR_SERVER };
cvar_t	teamlist = {"mp_teamlist","hgrunt;zombie", FCVAR_SERVER };
cvar_t	teamoverride = {"mp_teamoverride","1" };
cvar_t	defaultteam = {"mp_defaultteam","0" };
cvar_t	allowmonsters={"mp_allowmonsters","1", FCVAR_SERVER };
cvar_t	allowmonsters2={"mp_fragmonsters","0", FCVAR_SERVER };
cvar_t	allowmonsters3={"zxc_crowbar","1", FCVAR_SERVER };
cvar_t	allowmonsters4={"mp_dmode","0", FCVAR_SERVER }; //1.29, regeneration mod
cvar_t	allowmonsters5={"zxc_python","1", FCVAR_SERVER }; //1.30a, python
cvar_t	allowmonsters6={"zxc_gauss","1", FCVAR_SERVER }; //1.30a, gauss
cvar_t	allowmonsters7={"zxc_tank","1", FCVAR_SERVER }; //1.31, func_tank
cvar_t	allowmonsters8={"zxc_teleport","1", FCVAR_SERVER }; //1.31, cwb teleportation
cvar_t	allowmonsters9={"zxc_allwep","1", FCVAR_SERVER }; //1.33, wpn limit
cvar_t	allowmonsters10={"zxc_megaweps","0", FCVAR_SERVER }; //1.34, very Hi-explosive and no some weapons
cvar_t	allowmonsters11={"zxc_sfrad","1200", FCVAR_SERVER }; // sflare radius
cvar_t	allowmonsters12={"zxc_sflife","0", FCVAR_SERVER }; // sflare time life
cvar_t	allowmonsters13={"zxc_sfshake","1", FCVAR_SERVER }; // sflare shake toggle
cvar_t	allowmonsters14={"zxc_sfreload","180", FCVAR_SERVER }; // sflare reload in sec
cvar_t	allowmonsters15={"zxc_triple","0", FCVAR_SERVER }; // triple rocket for crowbar
cvar_t	allowmonsters16={"zxc_ammo","1", FCVAR_SERVER }; // full ammo with sv_cheats 1
cvar_t	allowmonsters17={"zxc_timer","60", FCVAR_SERVER }; // teamplay round restarter
cvar_t	allowmonsters18={"zxc_csmode","0", FCVAR_SERVER }; // team only 
cvar_t	allowmonsters19={"zxc_nomodels","0", FCVAR_SERVER }; // Prevent another models for player
cvar_t	allowmonsters20={"zxc_pain","0", FCVAR_SERVER }; // player slow moving (combat.cpp - CBaseMonster :: TakeDamage)
cvar_t	g_flWeaponCheat={"zxc_cheats","0", FCVAR_SERVER }; // same, as sv_cheats
cvar_t	g_bestfrags={"zxc_bestfrags","10000", FCVAR_SERVER }; // same, as mp_fraglimit for zxc_cheats 3


	// CVAR_SET_STRING("sv_gravity", "800"); // 65ft/sec
	// CVAR_SET_STRING("sv_stepsize", "18");
	// CVAR_SET_STRING("sv_maxspeed", "350");
	// CVAR_SET_STRING("pausable", "0");
	
	

cvar_t  mp_chattime = {"mp_chattime","5", FCVAR_SERVER };

// Engine Cvars
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_psv_aim = NULL;
cvar_t	*g_footsteps = NULL;

// END Cvars for Skill Level settings

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	// Register cvars here:

	g_psv_gravity = CVAR_GET_POINTER( "sv_gravity" );
	g_psv_aim = CVAR_GET_POINTER( "sv_aim" );
	g_footsteps = CVAR_GET_POINTER( "mp_footsteps" );

	CVAR_REGISTER (&displaysoundlist);

	CVAR_REGISTER (&teamplay);
	CVAR_REGISTER (&fraglimit);
	CVAR_REGISTER (&timelimit);

	CVAR_REGISTER (&fragsleft);
	CVAR_REGISTER (&timeleft);

	CVAR_REGISTER (&friendlyfire);
	CVAR_REGISTER (&falldamage);
	//CVAR_REGISTER (&weaponstay);
	CVAR_REGISTER (&forcerespawn);
	CVAR_REGISTER (&flashlight);
	CVAR_REGISTER (&aimcrosshair);
	CVAR_REGISTER (&decalfrequency);
	CVAR_REGISTER (&teamlist);
	CVAR_REGISTER (&teamoverride);
	CVAR_REGISTER (&defaultteam);
	CVAR_REGISTER (&allowmonsters);
	CVAR_REGISTER (&allowmonsters2); //1.27
	CVAR_REGISTER (&allowmonsters3); //1.28
	CVAR_REGISTER (&allowmonsters4); //1.29
	CVAR_REGISTER (&allowmonsters5); //python
	CVAR_REGISTER (&allowmonsters6); //gauss
	CVAR_REGISTER (&allowmonsters7); //tnk
	CVAR_REGISTER (&allowmonsters8); //tlp
	CVAR_REGISTER (&allowmonsters9); //
	CVAR_REGISTER (&allowmonsters10); //
	CVAR_REGISTER (&allowmonsters11); //
	CVAR_REGISTER (&allowmonsters12); //
	CVAR_REGISTER (&allowmonsters13); //
	CVAR_REGISTER (&allowmonsters14); //
	CVAR_REGISTER (&allowmonsters15); //
	CVAR_REGISTER (&allowmonsters16); //
	CVAR_REGISTER (&allowmonsters17); //
	CVAR_REGISTER (&allowmonsters18); //
	CVAR_REGISTER (&allowmonsters19); //
	CVAR_REGISTER (&allowmonsters20); //
	CVAR_REGISTER (&g_flWeaponCheat); //
	CVAR_REGISTER (&g_bestfrags); //

	CVAR_REGISTER (&mp_chattime);



	// SERVER_COMMAND( "exec skill.cfg\n" );
}

