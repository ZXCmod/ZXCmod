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
#include "handgrenade.h"



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
	m_flNextGravGrenadesTime = gpGlobals->time;
	pev->dmg = 105;
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
	PRECACHE_MODEL( "sprites/steam1.spr" );
	PRECACHE_MODEL("models/zxc_grenade.mdl");
	PRECACHE_MODEL("models/zxc_grenadet.mdl");
	PRECACHE_SOUND( "zxc/snd3m.wav" );
	PRECACHE_SOUND( "ambience/alien_creeper.wav" );
	

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

void CHandGrenade::ThirdAttack()
{
	if ( !m_flStartThrow2 && m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow2 = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( HANDGRENADE_PINPULL );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_type = 2; 
		
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
		m_type = 1; 
		
	}
}

void CHandGrenade::WeaponIdle( void )
{

	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0) 
	{
		if (  m_pPlayer->m_flNextGravGrenadesTime < gpGlobals->time ) // delay
			{

				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.35;
				Reload();
				m_pPlayer->m_flNextGravGrenadesTime = gpGlobals->time + 0.35;
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
			angThrow.x = -10 + angThrow.x * ( ( 90 - 5 ) / 90.0 );
		else
			angThrow.x = -10 + angThrow.x * ( ( 90 + 5 ) / 90.0 );

		float flVel = ( 100 - angThrow.x ) * 6;
		if ( flVel > 2000 )
			flVel = 2000;

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

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
		m_flReleaseThrow = -1;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
		SendWeaponAnim( HANDGRENADE_FIDGET );
	}
	
	// secondary attack
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
		else if (m_type == 1)
			CGrenade::ShootTimed2( m_pPlayer->pev, vecSrc, vecThrow, time );
		else
			CGrenade::ShootTimed3( m_pPlayer->pev, vecSrc, vecThrow, time );

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

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 4.0;
		m_flReleaseThrow = -1;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;
		SendWeaponAnim( HANDGRENADE_IDLE );
	}
	
	
}
// spawn Grav with reload delay 3 sec
void CHandGrenade :: Reload( void )
{
/* 	if ( g_zxc_cheats != 0) //no gravers anymore
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev) );
			WRITE_STRING( "Attack disabled, while sv_cheats is on." );
		MESSAGE_END();
		EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "buttons/bell1.wav", 0.75, ATTN_NORM, 1.0, 102 );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1; //delay
		return;
	} */


	Vector vecSrc = m_pPlayer->pev->origin;
	Vector vecThrow = gpGlobals->v_forward * 640;

	CBaseEntity *hGren = Create( "weapon_saa", vecSrc, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	hGren->pev->velocity = vecThrow;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	SendWeaponAnim( HANDGRENADE_THROW1 );

	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
}

void    CGrav1 :: Spawn( void )
{
	Precache( );
	
	SET_MODEL( ENT(pev), "models/zxc_grenade.mdl" );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-3, -3, 0), Vector(3, 3, 3));
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_grenade" );
	m_flDie = 0;
	pev->dmg = 1;
	pev->health = 100;
	pev->takedamage = DAMAGE_NO;
	pev->gravity 		= 0.5;
	pev->friction 		= 0.5;
	pev->angles = UTIL_VecToAngles (pev->velocity);
	pev->avelocity.y = 128;
	m_Sprite = PRECACHE_MODEL( "sprites/xflare2.spr" );

	
	pev->nextthink = gpGlobals->time + 6.0;
	SetThink( MoveThink );

}

void CGrav1 :: Precache( void )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	PRECACHE_SOUND( "weapons/gravgren.wav" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
}



void    CGrav1::Touch( CBaseEntity *pOther )
{
	// destroy this, if oths not in radius
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 128 )) != NULL)
		{
			if (FClassnameIs( pEntity->pev, "weapon_grenade") && (pEntity->edict() != edict()) && (FVisible( pEntity )) && (pEntity->pev->velocity == g_vecZero) && (pEntity->pev->effects != EF_LIGHT) && (pev->effects != EF_LIGHT))
				{
					// spark effects
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_STREAK_SPLASH );
						WRITE_COORD( pev->origin.x );		// origin
						WRITE_COORD( pev->origin.y );
						WRITE_COORD( pev->origin.z );
						WRITE_COORD( 0 );	// direction
						WRITE_COORD( 0 );
						WRITE_COORD( 128 );
						WRITE_BYTE( 64 );	// Streak color 6
						WRITE_SHORT( 24 );	// count
						WRITE_SHORT( 512 );
						WRITE_SHORT( 512 );	// Random velocity modifier
					MESSAGE_END();
					// torus
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_BEAMTORUS );
						WRITE_COORD( pev->origin.x);
						WRITE_COORD( pev->origin.y);
						WRITE_COORD( pev->origin.z); 
						WRITE_COORD( pev->origin.x);
						WRITE_COORD( pev->origin.y);
						WRITE_COORD( pev->origin.z - 400 ); // reach damage radius over .2 seconds
						WRITE_SHORT( m_iSpriteTexture );
						WRITE_BYTE( 0 ); // startframe
						WRITE_BYTE( 0 ); // framerate
						WRITE_BYTE( 5 ); // life
						WRITE_BYTE( 3 );  // width
						WRITE_BYTE( 0 );   // noise
						WRITE_BYTE( 255 );   // r, g, b
						WRITE_BYTE( 100 );   // r, g, b
						WRITE_BYTE( 100 );   // r, g, b
						WRITE_BYTE( 170 ); // brightness
						WRITE_BYTE( 0 );		// speed
					MESSAGE_END();
					
					MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
						WRITE_BYTE( TE_IMPLOSION );
						WRITE_COORD( pev->origin.x );
						WRITE_COORD( pev->origin.y );
						WRITE_COORD( pev->origin.z );
						WRITE_BYTE( 64 );	// rad
						WRITE_BYTE( 64 );		// count
						WRITE_BYTE( 12 );		// life
					MESSAGE_END();
					
					// shock ray
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
						WRITE_BYTE( 24 ); // life * 0.1
						WRITE_BYTE( 12 ); // width
						WRITE_BYTE( 0 ); // noise
						WRITE_BYTE( 100 ); // color r,g,b
						WRITE_BYTE( 100 ); // color r,g,b
						WRITE_BYTE( 200 ); // color r,g,b
						WRITE_BYTE( 220 ); // brightness
						WRITE_BYTE( 0 ); // scroll speed
					MESSAGE_END();
					
					// lights
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE(TE_DLIGHT);
						WRITE_COORD(pev->origin.x);	// X
						WRITE_COORD(pev->origin.y);	// Y
						WRITE_COORD(pev->origin.z);	// Z
						WRITE_BYTE( 16 );		// radius * 0.1
						WRITE_BYTE( 64 );		// r
						WRITE_BYTE( 64 );		// g
						WRITE_BYTE( 256 );		// b
						WRITE_BYTE( 128 );		// life * 10
						WRITE_BYTE( 24 );		// decay * 0.1
					MESSAGE_END( );
					
					UTIL_Sparks( pev->origin );
					
					EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/snd3m.wav", 1.0, ATTN_NORM);
					EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/snd3m.wav", 1.0, ATTN_NORM);
					EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/snd3m.wav", 1.0, ATTN_NORM);

					pEntity->pev->dmg += 1; // add more dmg
					pEntity->m_flDie -= 2; // increace timer delay
					pEntity->pev->nextthink = gpGlobals->time + 5.0;
					UTIL_Remove( this );
				}
		}
		
	int iContents = UTIL_PointContents ( pev->origin );

	if (pev->velocity != g_vecZero && (iContents != CONTENTS_WATER))
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
			pev->velocity = pev->velocity*0.75;
			pev->angles = UTIL_VecToAngles (pev->velocity); //dynamic angles
		}
}


void    CGrav1 :: MoveThink( void )
{

	Vector vecDir = Vector( 0, 0, 0 ); 
	Vector direction = Vector(0,0,1); 
	static float i; // resize cube effect 
	radius = 200 + (pev->dmg * 35); // set more radius
	
	// Make a lightning strike
	Vector vecEnd = Vector( 0, 0, 150 ); 
	vecEnd = pev->origin + vecEnd.Normalize() * 512;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMCYLINDER);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x );
	WRITE_COORD(pev->origin.y );
	WRITE_COORD(pev->origin.z + radius*2); 
	WRITE_SHORT(m_iSpriteTexture);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(16);	 // framerate
	WRITE_BYTE(4);	 // life
	WRITE_BYTE(radius*5); // width
	WRITE_BYTE(8);	 // noise
	WRITE_BYTE(55);	 // r, g, b
	WRITE_BYTE(55);	 // r, g, b
	WRITE_BYTE(155);	 // r, g, b
	WRITE_BYTE(155);	 // brightness
	WRITE_BYTE(4);	 // speed
	MESSAGE_END();

	// torus
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMTORUS);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z - radius*2); // reach damage radius over .2 seconds
	WRITE_SHORT(m_iSpriteTexture);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(2);	 // life
	WRITE_BYTE(3);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(155); // r, g, b
	WRITE_BYTE(0); // r, g, b
	WRITE_BYTE(200); // r, g, b
	WRITE_BYTE(164);	 // brightness
	WRITE_BYTE(0);	 // speed
	MESSAGE_END();
	
	// i -= 0.4; // decrease cube size for nice visual effect
	
	// particles
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_IMPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_BYTE( pev->dmg*25 );	// rad
		WRITE_BYTE( 24 );		// count
		WRITE_BYTE( 8 );		// life
	MESSAGE_END();

	//find and capture nearest objects, from 1.35 this rebalanced ! 1.38 is proper gravy
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, radius )) != NULL)
	{
		if ((pEntity->pev->takedamage > 0) && pEntity->IsAlive() && (pEntity->edict() != edict())) // == MOVETYPE_WALK || pEntity->pev->movetype == MOVETYPE_STEP
		{
			Vector vecMid = pev->origin;					  // get self
			Vector vecMidEnemy = pEntity->BodyTarget(vecMid); // get target
			Vector vecDirToEnemy = vecMidEnemy - vecMid;			  // calculate dir and dist to enemy

			vecDir = (pEntity->Center() - Center()).Normalize();
			pEntity->pev->velocity =  -(vecDir *  vecDirToEnemy.Length() * 3);
			
			UTIL_ScreenShake( pEntity->pev->origin, radius, 60, 0.5, 1 );
			
			// check wall
			if (FVisible( pEntity ))
			{					
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMENTS );
					WRITE_SHORT( ENTINDEX( this->edict() ) );
					WRITE_SHORT( ENTINDEX( pEntity->edict() ) );
					WRITE_SHORT( m_LaserSprite );
					WRITE_BYTE( 0 ); // framestart
					WRITE_BYTE( 0 ); // framerate
					WRITE_BYTE( 1 ); // life
					WRITE_BYTE( 10 );  // width
					WRITE_BYTE( 0 );   // noise
					WRITE_BYTE( 140 );   // r, g, b
					WRITE_BYTE( 140 );   // r, g, b
					WRITE_BYTE( 140 );   // r, g, b +pSightEnt->pev->health
					WRITE_BYTE( 200 );	// brightness
					WRITE_BYTE( 0 );		// speed
				MESSAGE_END();	
			}
		}
	}


	if (pev->effects != EF_LIGHT)
	{
		pev->velocity.z = 350;
		pev->angles.x = 256;
		static int pitch = 110 - (pev->dmg * 2);

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
		pev->gravity 		= 1.0;
		i = 20;
		
		SetTouch( NULL );
		
		if (pitch < 50)
			pitch = 50;
		EMIT_SOUND_DYN( ENT(pev), CHAN_ITEM, "weapons/gravgren.wav", 1.0, ATTN_NORM, 1.0, pitch );
		EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "ambience/alien_creeper.wav", 0.5, ATTN_NORM, 1.0, pitch );
		
	}
	
	// release a destroy
	if (m_flDie >= 52) // full explode and self destroy
	{		

		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), radius, radius, CLASS_NONE, DMG_BULLET  );

		while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, radius )) != NULL)
		{
			if ((pEntity->pev->takedamage > 0) && pEntity->IsAlive())
			{				
				if (!FVisible( pEntity )) // hit thr walls
				{
					pEntity->TakeDamage(pev, VARS( pev->owner ), radius*0.115, DMG_BLAST);
					UTIL_ScreenShake( pEntity->pev->origin, radius, 60, 1.5, 1.0 );
				}
			}
		}

		// cylinder shape 
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
			WRITE_BYTE( 50 );  // width
			WRITE_BYTE( 16 );   // noise
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 100 );   // r, g, b
			WRITE_BYTE( 150 );   // r, g, b
			WRITE_BYTE( 100 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();

		// random explosions
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 150  ); // scale * 10
			WRITE_BYTE( 9  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		STOP_SOUND( ENT(pev), CHAN_VOICE, "ambience/alien_creeper.wav" );
		UTIL_Remove( this );

	}

	pev->nextthink = gpGlobals->time + 0.12;
	m_flDie ++;
	
}


////////////////////////////////////
//Particle gren {smoke to Pg 1.35)//
////////////////////////////////////


void    CSmoke :: Spawn( void )
{
	Precache( );
	SET_MODEL( ENT(pev), "models/w_grenade.mdl" );
	
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-3, -3, 0), Vector(3, 3, 3));
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_grenade" );
	m_flDie = gpGlobals->time + 0; // deprecated
	pev->dmg = 2;
	pev->takedamage = DAMAGE_YES;
	pev->health = 200;
	pev->gravity 		= 1.0;
	pev->friction 		= 1.0;
	
	pev->nextthink = gpGlobals->time + 0.25;
	SetThink( MoveThink );

}

void CSmoke :: Precache( void )
{
	m_Sprite = PRECACHE_MODEL( "sprites/black_smoke4.spr" );
}

// antiarmor gren
void    CSmoke :: MoveThink( void )
{
	CBaseEntity *pEntity = NULL;
	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 100+pev->dmg )))
	{
		if ((pEntity->IsAlive() ) && pEntity->pev->takedamage && (pEntity->edict() != edict()) )
		{
			if (FVisible( pEntity )) 
			{
				pEntity->pev->velocity.x = pEntity->pev->velocity.x * 0.1; 
				pEntity->pev->velocity.y = pEntity->pev->velocity.y * 0.1; 
				pEntity->TakeDamage( pev, VARS( pev->owner ), pev->dmg/2, DMG_BULLET );
				

			}
		}
	}

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_PARTICLEBURST );
		WRITE_COORD( pev->origin.x + RANDOM_LONG(-128,128));
		WRITE_COORD( pev->origin.y + RANDOM_LONG(-128,128));
		WRITE_COORD( pev->origin.z);
		WRITE_SHORT( 128+(pev->dmg) );
		WRITE_BYTE( 1 );
		WRITE_BYTE( 9 );
	MESSAGE_END();
	
	pev->dmg += 2;
	pev->health-=3;
	pev->angles = UTIL_VecToAngles (pev->velocity);
		
	if (pev->health < 0) // full explode and self destroy
	{
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 16  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		UTIL_Remove( this );
	}
	
	pev->nextthink = gpGlobals->time + 0.2;
	
}

