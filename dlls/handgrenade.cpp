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
#include "shake.h"
#include "gamerules.h"
extern float g_flWeaponCheat;


#define	HANDGRENADE_PRIMARY_VOLUME		450

enum handgrenade_e {
	HANDGRENADE_IDLE = 0,
	HANDGRENADE_FIDGET,
	HANDGRENADE_PINPULL,
	HANDGRENADE_THROW1,	// toss
	HANDGRENADE_THROW2,	// medium
	HANDGRENADE_THROW3,	// hard
	HANDGRENADE_HOLSTER,
	HANDGRENADE_DRAW
};




LINK_ENTITY_TO_CLASS( weapon_handgrenade, CHandGrenade );

void CHandGrenade::Spawn( )
{
	Precache( );
	m_iId = WEAPON_HANDGRENADE;
	SET_MODEL(ENT(pev), "models/w_grenade.mdl");
	m_flNextChatTime6 = gpGlobals->time;
	pev->dmg = RANDOM_LONG(100,110);
	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;
	FallInit();// get ready to fall down.
	m_type = 0; //zero is standart grenade
}

void CHandGrenade::WriteBeamColor ( void )
{

}

void CHandGrenade::Precache( void )
{
	PRECACHE_MODEL("models/w_grenade.mdl");
	PRECACHE_MODEL("models/v_grenade.mdl");
	PRECACHE_MODEL("models/p_grenade.mdl");
	PRECACHE_SOUND( "weapons/gravgren.wav" );
	m_iTrail = PRECACHE_MODEL("sprites/shockwave.spr");
	PRECACHE_MODEL( "sprites/black_smoke4.spr" );
	PRECACHE_MODEL( "sprites/xflare2.spr" );
	PRECACHE_MODEL( "sprites/plasma.spr" );
	//m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	

}


int CHandGrenade::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hand Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_HANDGRENADE;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}


BOOL CHandGrenade::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 280 );
	m_flReleaseThrow = -1;
	return DefaultDeploy( "models/v_grenade.mdl", "models/p_grenade.mdl", HANDGRENADE_DRAW, "crowbar" );
}

BOOL CHandGrenade::CanHolster( void )
{
	// can only holster hand grenades when not primed!
	return ( m_flStartThrow == 0 );
	return ( m_flStartThrow2 == 0 );
}

void CHandGrenade::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		SendWeaponAnim( HANDGRENADE_HOLSTER );
	}
	else
	{
		// no more grenades!
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_HANDGRENADE);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CHandGrenade::PrimaryAttack()
{


	if ( !m_flStartThrow && m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( HANDGRENADE_PINPULL );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
}
//edit
void CHandGrenade::SecondaryAttack()
{
	if ( !m_flStartThrow2 && m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow2 = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( HANDGRENADE_PINPULL );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		m_type = 0;
		
	}
}
//edit
//1.28 smoke grenade

void CHandGrenade::ThirdAttack()
{
if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 10) 
	{
	int iAnim;
	Vector vecSrc = m_pPlayer->pev->origin;
	Vector vecThrow = gpGlobals->v_forward * 650 + m_pPlayer->pev->velocity*2;

	#ifndef CLIENT_DLL
	CBaseEntity *hGren = Create( "weapon_canister", vecSrc, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	hGren->pev->velocity = vecThrow;
	//hGren->pev->avelocity.y = 500;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	iAnim = HANDGRENADE_THROW1;
	SendWeaponAnim( iAnim );
	#endif

	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 10;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.25;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.95;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
	}
}

void CHandGrenade::FourthAttack()
{
	if ( !m_flStartThrow2 && m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow2 = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( HANDGRENADE_PINPULL );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_type = 1; //fire grenade
		
	}
}

void CHandGrenade::WeaponIdle( void )
{
	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 10) 
	{
		if (  m_pPlayer->m_flNextChatTime6 < gpGlobals->time ) //need delay
			{
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
				Reload();
				m_pPlayer->m_flNextChatTime6 = gpGlobals->time + 3;
				return;
			}
	}

	if ( m_flReleaseThrow == 0 && m_flStartThrow )
		 m_flReleaseThrow = gpGlobals->time;

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if ( m_flStartThrow )
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if ( angThrow.x < 0 )
			angThrow.x = -20 + angThrow.x * ( ( 90 - 10 ) / 90.0 );
		else
			angThrow.x = -20 + angThrow.x * ( ( 90 + 10 ) / 90.0 );

		float flVel = ( 270 - angThrow.x ) * 2;
		if ( flVel > 1500 )
			flVel = 1500;

		UTIL_MakeVectors( angThrow );

		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		// alway explode 3 seconds after the pin was pulled
		float time = m_flStartThrow - gpGlobals->time + 3.0;

		CGrenade::ShootTimed( m_pPlayer->pev, vecSrc, vecThrow, time );

		
		if ( flVel < 1500 )
		{
			SendWeaponAnim( HANDGRENADE_THROW1 );
		}
		else if ( flVel < 2000 )
		{
			SendWeaponAnim( HANDGRENADE_THROW2 );
		}
		else
		{
			SendWeaponAnim( HANDGRENADE_THROW3 );
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flReleaseThrow = 0;
		m_flStartThrow = 0;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;

		m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;

		if ( !m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			// just threw last grenade
			// set attack times in the future, and weapon idle in the future so we can see the whole throw
			// animation, weapon idle will automatically retire the weapon for us.
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;// ensure that the animation can finish playing
		}
		return;
	}
	else if ( m_flReleaseThrow > 0 )
	{
		// we've finished the throw, restart.
		m_flStartThrow = 0;

		if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			SendWeaponAnim( HANDGRENADE_DRAW );
		}
		else
		{
			RetireWeapon();
			return;
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_flReleaseThrow = -1;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
		if (flRand <= 0.75)
		{
			iAnim = HANDGRENADE_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
		}
		else 
		{
			iAnim = HANDGRENADE_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 75.0 / 30.0;
		}

		SendWeaponAnim( iAnim );
	}
	
	//secondary attack
	if ( m_flStartThrow2 )
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if ( angThrow.x < 0 )
			angThrow.x = -10 + angThrow.x * ( ( 90 - 10 ) / 90.0 );
		else
			angThrow.x = -10 + angThrow.x * ( ( 90 + 10 ) / 90.0 );

		float flVel = ( 90 - angThrow.x ) * 4;
		if ( flVel > 500 )
			flVel = 500;

		UTIL_MakeVectors( angThrow );

		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		// alway explode 3 seconds after the pin was pulled
		float time = m_flStartThrow2 - gpGlobals->time + 3.0;
		if (time < 0)		
			time = 0;

		if (m_type == 0)
			CGrenade::ShootTimed( m_pPlayer->pev, vecSrc, vecThrow, time );
		else
			CGrenade::ShootTimed2( m_pPlayer->pev, vecSrc, vecThrow, time );

		if ( flVel < 500 )
		{
			SendWeaponAnim( HANDGRENADE_THROW1 );
		}
		else if ( flVel < 1000 )
		{
			SendWeaponAnim( HANDGRENADE_THROW2 );
		}
		else
		{
			SendWeaponAnim( HANDGRENADE_THROW3 );
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flReleaseThrow = 0;
		m_flStartThrow2 = 0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;

		m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;

		if ( !m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			// just threw last grenade
			// set attack times in the future, and weapon idle in the future so we can see the whole throw
			// animation, weapon idle will automatically retire the weapon for us.
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;// ensure that the animation can finish playing
		}
		return;
	}
	else if ( m_flReleaseThrow > 0 )
	{
		// we've finished the throw, restart.
		m_flStartThrow2 = 0;

		if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			SendWeaponAnim( HANDGRENADE_DRAW );
		}
		else
		{
			RetireWeapon();
			return;
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_flReleaseThrow = -1;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
		
		if (flRand <= 0.25)
		{
			iAnim = HANDGRENADE_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
		}
		else 
		{
			iAnim = HANDGRENADE_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 75.0 / 30.0;
		}

		SendWeaponAnim( iAnim );
	}
	
	
}
///////spawn Grav with reload delay 3 sec
void CHandGrenade :: Reload( void )
{
/* 	if ( g_flWeaponCheat != 0) //no gravers anymore
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev) );
			WRITE_STRING( "Attack disabled, while sv_cheats is on." );
		MESSAGE_END();
		EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "buttons/bell1.wav", 0.75, ATTN_NORM, 1.0, 102 );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1; //delay
		return;
	} */

	int iAnim;
	Vector vecSrc = m_pPlayer->pev->origin;
	Vector vecThrow = gpGlobals->v_forward * 650 + m_pPlayer->pev->velocity*2;

	CBaseEntity *hGren = Create( "weapon_saa", vecSrc, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	hGren->pev->velocity = vecThrow;
	//hGren->pev->avelocity.y = 500;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	iAnim = HANDGRENADE_THROW1;
	SendWeaponAnim( iAnim );

	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 10; //(<1.26)
}

class   CGrav1 : public CBaseEntity
{
        private:

        void    Spawn           ( void );
		void 	Precache 		( void );
        void    MoveThink       ( void );
        void    Explode         ( void );
		int 	m_flDie;
		int 	m_flDie2;
		int     BeamSprite;
		int 	m_iSpriteTexture;
		short	m_LaserSprite;
		short	m_Sprite;
};

class   CSmoke : public CBaseEntity
{
        private:

        void    Spawn           ( void );
		void 	Precache 		( void );
        void    MoveThink       ( void );
		int 	m_flDie;
		short	m_Sprite;
};


LINK_ENTITY_TO_CLASS( weapon_saa, CGrav1 );
LINK_ENTITY_TO_CLASS( weapon_canister, CSmoke );






void    CGrav1 :: Spawn( void )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_grenade.mdl" );
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_grenade" ); // GravGrenade
	m_flDie = gpGlobals->time + RANDOM_LONG(12,16);
	pev->dmg = 0;
	pev->takedamage = DAMAGE_YES;
	pev->nextthink = gpGlobals->time + 5.1;//10 times a second
	SetThink( MoveThink );
	pev->health			= 9999999; //get more eat!
	m_Sprite = PRECACHE_MODEL( "sprites/xflare2.spr" );
}

void CGrav1 :: Precache( void )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	PRECACHE_SOUND( "weapons/gravgren.wav" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
}



void    CGrav1:: Explode( void )
{
	
}


void    CGrav1 :: MoveThink( void )
{
/* 	//5.0 4.8, indus code xD
	if (gpGlobals->time >= m_flDie-5.4 && gpGlobals->time <= m_flDie-5.1)
		{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/gravgren.wav", 1.0, ATTN_NORM); //play sound SND_STOP //ATTN_NORM
		//Explode();
		return;
		}
	//else
		//Explode();
 */

 	CBaseEntity *pEntity = NULL;
	Vector	vecDir;
	vecDir = Vector( 0, 0, 0 );
	Vector direction = Vector(0,0,1); 
	
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = 0;	// Pick a random direction
	vecEnd.y = 0;
	vecEnd.z = RANDOM_FLOAT(128,255);
	vecEnd = pev->origin + vecEnd.Normalize() * 512;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	pev->velocity.z = 35; //jump
	
	//find and capture nearest objects
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 284 )) != NULL) //256
		{
			if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP)
			{
				vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
				pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -200; //300  + pev->dmg

				//push up
				switch(RANDOM_LONG(0,3))
				{
					case 1: pEntity->pev->velocity.z += pEntity->pev->velocity.z + 128;
					pev->velocity.z = 135; //jump
					break;
				}
				
				//::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 1000, 25, CLASS_NONE, DMG_GENERIC  );
				UTIL_ScreenShake( pEntity->pev->origin, 12.0, 90.5, 0.3, 1 );
				//UTIL_ScreenFade( pEntity, Vector(0,RANDOM_LONG(128,255),RANDOM_LONG(170,255)), 64, 12, 16, FFADE_IN );
				
				//lightings up
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
					WRITE_BYTE( 1 ); // life * 0.1
					WRITE_BYTE( 16 ); // width
					WRITE_BYTE( 24 ); // noise
					WRITE_BYTE( 255 ); // color r,g,b
					WRITE_BYTE( 255 ); // color r,g,b
					WRITE_BYTE( 255 ); // color r,g,b
					WRITE_BYTE( 175 ); // brightness
					WRITE_BYTE( 8 ); // scroll speed
				MESSAGE_END();
				
				//dont hurt through wall
				if (FVisible( pEntity ))
				{
					pEntity->TakeDamage(pev, VARS( pev->owner ), 1, DMG_SHOCK);	
				
					//shock ray
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
						WRITE_BYTE( 8 ); // width
						WRITE_BYTE( 24 ); // noise
						WRITE_BYTE( 250 ); // color r,g,b
						WRITE_BYTE( 250 ); // color r,g,b
						WRITE_BYTE( 255 ); // color r,g,b
						WRITE_BYTE( 160 ); // brightness
						WRITE_BYTE( 256 ); // scroll speed
					MESSAGE_END();
				}


				//::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 16, 128, CLASS_NONE, DMG_MORTAR  ); //end blast
			
			
				//spark effects
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_STREAK_SPLASH );
					WRITE_COORD( pev->origin.x );		// origin
					WRITE_COORD( pev->origin.y );
					WRITE_COORD( pev->origin.z );
					WRITE_COORD( direction.x );	// direction
					WRITE_COORD( direction.y );
					WRITE_COORD( direction.z );
					WRITE_BYTE( 64 );	// Streak color 6
					WRITE_SHORT( 3 );	// count
					WRITE_SHORT( 512 );
					WRITE_SHORT( 512 );	// Random velocity modifier
				MESSAGE_END();
			}
		}

	if (pev->effects != EF_LIGHT)
	{
		pev->velocity.z = 345;
		pev->angles.x = RANDOM_LONG(25,325);
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/gravgren.wav", 1.0, ATTN_NORM);
		
		// animated sprite
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( m_Sprite );
			WRITE_BYTE( 30 ); // scale * 10
			WRITE_BYTE( 200 ); // brightness
		MESSAGE_END();
		
		pev->effects |= EF_LIGHT;
	}
	
	//release a destroy
	if (gpGlobals->time >= m_flDie) //full explode and self destroy
	{		
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 256 )) != NULL) //512
			{
				if (pEntity->pev->movetype == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP) ///NICE!!!
				{
					vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize(); ///NOW WORKED! CONGRATULATIONS!
					pEntity->pev->velocity = pEntity->pev->velocity + vecDir * 512;
					::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 8, 512, CLASS_NONE, DMG_FREEZE|DMG_MORTAR  );
					UTIL_ScreenShake( pEntity->pev->origin, 12.0, 120, 0.9, 1 );

					//spark effects
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_STREAK_SPLASH );
						WRITE_COORD( pev->origin.x );		// origin
						WRITE_COORD( pev->origin.y );
						WRITE_COORD( pev->origin.z );
						WRITE_COORD( direction.x );	// direction
						WRITE_COORD( direction.y );
						WRITE_COORD( direction.z );
						WRITE_BYTE( 255 );	// Streak color 6
						WRITE_SHORT( 64 );	// count
						WRITE_SHORT( 1024 );
						WRITE_SHORT( 1600 );	// Random velocity modifier
					MESSAGE_END();


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
						WRITE_BYTE( RANDOM_LONG(30,70) );  // width
						WRITE_BYTE( 0 );   // noise
						WRITE_BYTE( 0 );   // r, g, b
						WRITE_BYTE( RANDOM_LONG(103,255) );   // r, g, b
						WRITE_BYTE( RANDOM_LONG(103,255) );   // r, g, b
						WRITE_BYTE( 128 ); // brightness
						WRITE_BYTE( 0 );		// speed
					MESSAGE_END();

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
					UTIL_Remove( this );
				}
			}
	}

	if (gpGlobals->time >= m_flDie+5)
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
		UTIL_Remove( this );
		}
		
	pev->nextthink = gpGlobals->time + 0.11;
	
}


//////////////////////////
////////////smoke/////////
//////////////////////////


void    CSmoke :: Spawn( void )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_grenade.mdl" );
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, 0), Vector(4, 4, 8) );
	UTIL_SetOrigin( pev, pev->origin );
	m_flDie = gpGlobals->time + 30;
	pev->nextthink = gpGlobals->time + 1.5;//10 times a second
	SetThink( MoveThink );

}

void CSmoke :: Precache( void )
{
	m_Sprite = PRECACHE_MODEL( "sprites/black_smoke4.spr" );
}



void    CSmoke :: MoveThink( void )
{
	// lots of smoke
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ) );
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ) );
		WRITE_COORD( pev->origin.z - 8 );
		WRITE_SHORT( m_Sprite );
		WRITE_BYTE( 64 ); // scale * 10
		WRITE_BYTE( 6 ); // framerate
	MESSAGE_END( );

	if (gpGlobals->time >= m_flDie) //full explode and self destroy
	{
		// random explosions
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 8  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		UTIL_Remove( this );
	}
	
	pev->nextthink = gpGlobals->time + 0.3; //0.15 old
	
}

