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
cvar_t	teamplay	= {"mp_teamplay","0", FCVAR_SERVER };
cvar_t	fraglimit	= {"mp_fraglimit","0", FCVAR_SERVER };
cvar_t	timelimit	= {"mp_timelimit","0", FCVAR_SERVER };
cvar_t	friendlyfire= {"mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage	= {"mp_falldamage","0", FCVAR_SERVER };
cvar_t	forcerespawn= {"mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight	= {"mp_flashlight","1", FCVAR_SERVER };
cvar_t	aimcrosshair= {"mp_autocrosshair","0", FCVAR_SERVER };
cvar_t	decalfrequency = {"decalfrequency","20", FCVAR_SERVER };
cvar_t	teamlist = {"mp_teamlist","hgrunt;zombie", FCVAR_SERVER };
cvar_t	teamoverride = {"mp_teamoverride","1" };
cvar_t	defaultteam = {"mp_defaultteam","0" };
cvar_t	allowmonsters={"mp_allowmonsters","1", FCVAR_SERVER };
cvar_t	g_zxc_mp_fragmonsters={"mp_fragmonsters","1", FCVAR_SERVER };
cvar_t	g_zxc_mp_dmode={"mp_dmode","1", FCVAR_SERVER }; //1.29, regeneration mod
cvar_t	g_zxc_shotgun_flare_radius={"zxc_sfrad","1500", FCVAR_SERVER }; // sflare radius
cvar_t	g_zxc_shotgun_flare_reload={"zxc_sfreload","120", FCVAR_SERVER }; // sflare reload in sec
cvar_t	g_zxc_cheats={"zxc_cheats","1", FCVAR_SERVER }; // same, as sv_cheats
cvar_t	g_zxc_mspawn={"zxc_mspawn","0", FCVAR_SERVER };
cvar_t	g_zxc_promode={"zxc_promode","0", FCVAR_SERVER };
cvar_t	g_zxc_hp={"zxc_hp","100.0", FCVAR_SERVER };

	

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
	CVAR_REGISTER (&g_zxc_mp_fragmonsters);
	CVAR_REGISTER (&g_zxc_mp_dmode);
	CVAR_REGISTER (&g_zxc_shotgun_flare_radius);
	CVAR_REGISTER (&g_zxc_shotgun_flare_reload);
	CVAR_REGISTER (&g_zxc_cheats);
	CVAR_REGISTER (&mp_chattime);
	CVAR_REGISTER (&g_zxc_mspawn);
	CVAR_REGISTER (&g_zxc_promode);
	CVAR_REGISTER (&g_zxc_hp);

	// SERVER_COMMAND( "exec skill.cfg\n" );
}

