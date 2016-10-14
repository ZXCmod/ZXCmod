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
// Hornets
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"weapons.h"
#include	"soundent.h"
#include	"hornet.h"
#include	"gamerules.h"
#include "game.h"


int iHornetTrail;
int iHornetPuff;

LINK_ENTITY_TO_CLASS( hornet, CHornet );


//=========================================================
// don't let hornets gib, ever.
//=========================================================
int CHornet :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	// filter these bits a little.
	bitsDamageType &= ~ ( DMG_ALWAYSGIB );
	bitsDamageType |= DMG_NEVERGIB;

	return CBaseMonster :: TakeDamage ( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

//=========================================================
//=========================================================
void CHornet :: Spawn( void )
{
	Precache();
	pev->movetype	= MOVETYPE_FLY;
	pev->solid		= SOLID_BBOX;
	pev->flags		|= FL_MONSTER;

	m_flFlySpeed = 1024;
	
	pev->velocity = gpGlobals->v_forward * m_flFlySpeed;

	m_flFieldOfView = 0.4; // +- x degrees

	SET_MODEL(ENT( pev ), "models/hornet.mdl");
	UTIL_SetSize( pev, Vector( -3, -3, -3 ), Vector( 3, 3, 3 ) );
	SetTouch( DieTouch );
	SetThink( StartTrack );
	edict_t *pSoundEnt = pev->owner;
	if ( !pSoundEnt )
		pSoundEnt = edict();
	pev->dmg = 10;
	pev->nextthink = 0.5;
	ResetSequenceInfo( );
	
	Classify2 = Classify();
	
	m_flDie = gpGlobals->time + 4;
}


void CHornet :: Precache()
{
	PRECACHE_MODEL("models/hornet.mdl");

	PRECACHE_SOUND( "agrunt/ag_fire1.wav" );
	PRECACHE_SOUND( "agrunt/ag_fire2.wav" );
	PRECACHE_SOUND( "agrunt/ag_fire3.wav" );

	PRECACHE_SOUND( "hornet/ag_buzz1.wav" );
	PRECACHE_SOUND( "hornet/ag_buzz2.wav" );
	PRECACHE_SOUND( "hornet/ag_buzz3.wav" );

	PRECACHE_SOUND( "hornet/ag_hornethit1.wav" );
	PRECACHE_SOUND( "hornet/ag_hornethit2.wav" );
	PRECACHE_SOUND( "hornet/ag_hornethit3.wav" );

	iHornetPuff = PRECACHE_MODEL( "sprites/muz1.spr" );
	iHornetTrail = PRECACHE_MODEL("sprites/laserbeam.spr");
}	

//=========================================================
// hornets will never get mad at each other, no matter who the owner is.
//=========================================================
int CHornet::IRelationship ( CBaseEntity *pTarget )
{
	if ( pTarget->pev->modelindex == pev->modelindex )
	{
		return R_NO;
	}

	return CBaseMonster :: IRelationship( pTarget );
}

//=========================================================
// ID's Hornet as their owner
//=========================================================
int CHornet::Classify ( )
{

	if ( pev->owner && pev->owner->v.flags & FL_CLIENT)
	{
		return CLASS_PLAYER_BIOWEAPON;
	}

	return	CLASS_ALIEN_BIOWEAPON;
}

//=========================================================
// StartTrack - starts a hornet out tracking its target
//=========================================================
void CHornet :: StartTrack ( void )
{
	IgniteTrail();

	SetTouch( TrackTouch );
	SetThink( TrackTarget );

	pev->nextthink = gpGlobals->time + 0.1;
}

//=========================================================
// StartDart - starts a hornet out just flying straight.
//=========================================================
void CHornet :: StartDart ( void )
{
	IgniteTrail();
	SetTouch( DartTouch );
}

void CHornet::IgniteTrail( void )
{

	// trail
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE(  TE_BEAMFOLLOW );
		WRITE_SHORT( entindex() );	// entity
		WRITE_SHORT( iHornetTrail );	// model
		WRITE_BYTE( 12 ); // life
		WRITE_BYTE( 1 );  // width
		WRITE_BYTE( 161 );   // r, g, b
		WRITE_BYTE( 25 );   // r, g, b
		WRITE_BYTE( 97 );   // r, g, b
		WRITE_BYTE( 100 );	// brightness
	MESSAGE_END();
}

//=========================================================
// Hornet is flying, gently tracking target
//=========================================================
void CHornet :: TrackTarget ( void )
{
	Vector	vecFlightDir;
	Vector	vecDirToEnemy;
	float	flDelta;

	StudioFrameAdvance( );
	
	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{

		// explode
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 12  );
			WRITE_BYTE( 16  );
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 256, CLASS_NONE, DMG_CRUSH  );

		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}

	if ( m_hEnemy == NULL )
	{
		Look( 256 );
		m_hEnemy = BestVisibleEnemy( );
	}
	
	if ( m_hEnemy != NULL && FVisible( m_hEnemy ))
	{
		m_vecEnemyLKP = m_hEnemy->BodyTarget( pev->origin );
	}
	else
	{
		m_vecEnemyLKP = m_vecEnemyLKP + pev->velocity * m_flFlySpeed * 0.1;
	}

	vecDirToEnemy = ( m_vecEnemyLKP - pev->origin ).Normalize();

	if (pev->velocity.Length() < 0.9)
		vecFlightDir = vecDirToEnemy;
	else 
		vecFlightDir = pev->velocity.Normalize();

	// measure how far the turn is, the wider the turn, the slow we'll go this time.
	flDelta = DotProduct ( vecFlightDir, vecDirToEnemy );
	
	if ( flDelta < 0.5 )
	{
		switch (RANDOM_LONG(0,2))
		{
			case 0:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz1.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
			case 1:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz2.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
			case 2:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz3.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
		}
	}

	pev->velocity = ( vecFlightDir*0.5 + vecDirToEnemy).Normalize();

	pev->velocity = pev->velocity * ( m_flFlySpeed * flDelta );// scale the dir by the ( speed * width of turn )

	pev->angles = UTIL_VecToAngles (pev->velocity);
	
	pev->nextthink = gpGlobals->time + 0.1;
}

//=========================================================
// Tracking Hornet hit something
//=========================================================
void CHornet :: TrackTouch ( CBaseEntity *pOther )
{
	// explode
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
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 128, CLASS_NONE, DMG_PARALYZE );
	
	UTIL_BloodDrips( pev->origin, g_vecZero, RANDOM_LONG(25,75), 60 );
	DieTouch( pOther );
}

void CHornet::DartTouch( CBaseEntity *pOther )
{
	DieTouch( pOther );
	
}

void CHornet::DieTouch ( CBaseEntity *pOther )
{
	if ( pOther && pOther->pev->takedamage )
	{
		switch (RANDOM_LONG(0,2))
		{
			case 0:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_hornethit1.wav", 1, ATTN_NORM);	break;
			case 1:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_hornethit2.wav", 1, ATTN_NORM);	break;
			case 2:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_hornethit3.wav", 1, ATTN_NORM);	break;
		}
	}


	pev->modelindex = 0;

	UTIL_Remove( this );
}

