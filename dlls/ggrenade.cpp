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
/*

===== generic grenade.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "decals.h"
#include "shake.h"


//===================grenade


LINK_ENTITY_TO_CLASS( grenade, CGrenade );

// Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges
#define SF_DETONATE		0x0001

//
// Grenade Explode
//
void CGrenade::Explode( Vector vecSrc, Vector vecAim )
{
	TraceResult tr;
	UTIL_TraceLine ( pev->origin, pev->origin + Vector ( 0, 0, -32 ),  ignore_monsters, ENT(pev), & tr);

	Explode( &tr, DMG_BLAST );
}

// UNDONE: temporary scorching for PreAlpha - find a less sleazy permenant solution.
void CGrenade::Explode( TraceResult *pTrace, int bitsDamageType )
{
	float		flRndSound;// sound randomizer

	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible

	pev->takedamage = DAMAGE_NO;

	// Pull out of the wall a bit
	if ( pTrace->flFraction != 1.0 )
	{
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * (pev->dmg - 24) * 0.6);
	}

	int iContents = UTIL_PointContents ( pev->origin );
	
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		if (iContents != CONTENTS_WATER)
		{
			WRITE_SHORT( g_sModelIndexFireball );
		}
		else
		{
			WRITE_SHORT( g_sModelIndexWExplosion );
		}
		WRITE_BYTE( pev->dmg * .50  ); // explode scale, fixed 1.30
		WRITE_BYTE( 16  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );
	entvars_t *pevOwner;
	if ( pev->owner )
		pevOwner = VARS( pev->owner );
	else
		pevOwner = NULL;

	pev->owner = NULL; // can't traceline attack owner if this is set

	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType );

	if ( RANDOM_FLOAT( 0 , 1 ) < 0.5 )
	{
		UTIL_DecalTrace( pTrace, DECAL_SCORCH1 );
	}
	else
	{
		UTIL_DecalTrace( pTrace, DECAL_SCORCH2 );
	}

	flRndSound = RANDOM_FLOAT( 0 , 1 );

	switch ( RANDOM_LONG( 0, 2 ) )
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris1.wav", 0.55, ATTN_NORM);	break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris2.wav", 0.55, ATTN_NORM);	break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris3.wav", 0.55, ATTN_NORM);	break;
	}

	pev->effects |= EF_NODRAW;
	SetThink( Smoke );
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;

	if (iContents != CONTENTS_WATER)
	{
		int sparkCount = RANDOM_LONG(0,3);
		for ( int i = 0; i < sparkCount; i++ )
			Create( "spark_shower", pev->origin, pTrace->vecPlaneNormal, NULL );
	}
}


void CGrenade::Smoke( void )
{
	if (UTIL_PointContents ( pev->origin ) == CONTENTS_WATER)
	{
		UTIL_Bubbles( pev->origin - Vector( 64, 64, 64 ), pev->origin + Vector( 64, 64, 64 ), 100 );
	}
	else
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( pev->dmg ); // smoke scale * 10
			WRITE_BYTE( 16  ); // framerate
		MESSAGE_END();
	}
	UTIL_Remove( this );
}

void CGrenade::Killed( entvars_t *pevAttacker, int iGib )
{
	Detonate( );
}


// Timed grenade, this think is called when time runs out.
void CGrenade::DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( Detonate );
	pev->nextthink = gpGlobals->time;
}

short	m_Sprite;

// Timed secondary satchels, exlode after 2 sec with sound
void CGrenade::DetonateUse2( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	pev->movetype = MOVETYPE_NONE;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/warningbell1.wav", 1.0, ATTN_NORM);
	
	m_Sprite = PRECACHE_MODEL( "sprites/xflare1.spr" );
	
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
		
	SetThink( Detonate );
	pev->nextthink = gpGlobals->time + 2;
}

void CGrenade::PreDetonate( void )
{
	CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin, 400, 0.3 );

	SetThink( Detonate );
	pev->nextthink = gpGlobals->time + 1;
}


void CGrenade::Detonate( void )
{
	TraceResult tr;
	Vector		vecSpot;// trace starts here!

	vecSpot = pev->origin + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);

	Explode( &tr, DMG_BLAST );
}


//
// Contact grenade, explode when it touches something
// 
void CGrenade::ExplodeTouch( CBaseEntity *pOther )
{
	TraceResult tr;
	Vector		vecSpot;// trace starts here!

	pev->enemy = pOther->edict();

	vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr );

	Explode( &tr, DMG_BLAST );
}


void CGrenade::DangerSoundThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin + pev->velocity * 0.5, pev->velocity.Length( ), 0.2 );
	pev->nextthink = gpGlobals->time + 0.2;

	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
	}
}


void CGrenade::BounceTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// only do damage if we're moving fairly fast
	if (m_flNextAttack < gpGlobals->time && pev->velocity.Length() > 100)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		if (pevOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace( );
			ClearMultiDamage( );
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_CLUB ); 
			ApplyMultiDamage( pev, pevOwner);
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}

	Vector vecTestVelocity;
	// pev->avelocity = Vector (300, 300, 300);

	// this is my heuristic for modulating the grenade velocity because grenades dropped purely vertical
	// or thrown very far tend to slow down too quickly for me to always catch just by testing velocity. 
	// trimming the Z velocity a bit seems to help quite a bit.
	vecTestVelocity = pev->velocity; 
	vecTestVelocity.z *= 0.45;

	if ( !m_fRegisteredSound && vecTestVelocity.Length() <= 60 )
	{
		//ALERT( at_console, "Grenade Registered!: %f\n", vecTestVelocity.Length() );

		// grenade is moving really slow. It's probably very close to where it will ultimately stop moving. 
		// go ahead and emit the danger sound.
		
		// register a radius louder than the explosion, so we make sure everyone gets out of the way
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin, pev->dmg / 0.4, 0.3 );
		m_fRegisteredSound = TRUE;
	}

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.8;

		pev->sequence = RANDOM_LONG( 1, 1 );
	}
	else
	{
		// play bounce sound
		BounceSound();
	}
	pev->framerate = pev->velocity.Length() / 200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;

}



void CGrenade::SlideTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// pev->avelocity = Vector (300, 300, 300);

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;

		if (pev->velocity.x != 0 || pev->velocity.y != 0)
		{
			// maintain sliding sound
		}
	}
	else
	{
		BounceSound();
	}
}

void CGrenade :: BounceSound( void )
{
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM);	break;
	case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM);	break;
	case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM);	break;
	}
}

void CGrenade :: TumbleThink( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime - 1 < gpGlobals->time)
	{
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1 );
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink( Detonate );
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}


void CGrenade :: TumbleThink2( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}
	
	CBaseEntity *pEntity = NULL;
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime - 1 < gpGlobals->time)
	{
		CSoundEnt::InsertSound ( bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1 );
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
	
		Vector vecSrc;
		vecSrc.x = RANDOM_LONG(-360,360);
		vecSrc.y = RANDOM_LONG(-360,360);
		vecSrc.z = 0;


		//FireBullets( 64, pev->origin, vecSrc, Vector( RANDOM_LONG(-360,360), RANDOM_LONG(-360,360), RANDOM_LONG(-180,360) ), 768, 35, 1, 70, VARS( pev->owner ) );
		FireBullets( 8, pev->origin, vecSrc, Vector( 2000,2000,2000 ), 128, 35, 1, 50, VARS( pev->owner ) );
		
		Create( "env_particleemitter", pev->origin + gpGlobals->v_up * 32, pev->angles, pev->owner ); 
		Create( "env_particleemitter", pev->origin + gpGlobals->v_up * 64, pev->angles, pev->owner ); 
		Create( "env_particleemitter", pev->origin + gpGlobals->v_up * 96, pev->angles, pev->owner ); 

		SetThink( Detonate );
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}



//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//grenade beams, copied and changed from egon.cpp
class   CGrenadeBeam : public CBaseEntity
{
		void    Spawn           ( );
		void    EXPORT Update   ( void );
		short	m_LaserSprite;
		short	m_LaserSprite2;
		int		m_iBalls;
}; 
LINK_ENTITY_TO_CLASS( env_particleemitter, CGrenadeBeam ); //storm beam entity

void CGrenadeBeam::Spawn( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 1.0;
	pev->friction = 0.0;
	
	
	pev->solid = SOLID_NOT;
	UTIL_SetSize( pev, Vector( 0, 0, 0), Vector(0, 0, 0) );
	UTIL_SetOrigin( pev, pev->origin );
	
	pev->classname = MAKE_STRING( "weapon_grenade" );
	
	//sprite 
	SET_MODEL( ENT(pev), "sprites/blueflare2.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/gradbeam.spr" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	m_LaserSprite2 = PRECACHE_MODEL( "sprites/plasma.spr" );
	//effects
	pev->rendermode = kRenderTransAdd;
	pev->rendercolor.x = 255;  // red
	pev->rendercolor.y = 255;  // green
	pev->rendercolor.z = 255; // blue
	pev->renderamt = 250;
	pev->scale = 1.25;
	
	pev->velocity.z = 370; //jump strong
	pev->velocity.y = RANDOM_LONG(-160,160); //jump strong 2
	pev->velocity.x = RANDOM_LONG(-160,160); //jump strong 3
	
	switch(RANDOM_LONG(0,1))
	{
		case 0: 
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMFOLLOW );
			WRITE_SHORT( entindex() );	// entity
			WRITE_SHORT( m_LaserSprite2 );	// model
			WRITE_BYTE( 16 ); // life
			WRITE_BYTE( 3 );  // width
			WRITE_BYTE( 80 );   // r, g, b
			WRITE_BYTE( 200 );   // r, g, b
			WRITE_BYTE( 200 );   // r, g, b
			WRITE_BYTE( 160 );	// brightness
		MESSAGE_END();
		break;
	}

	SetThink(Update);
	pev->nextthink = gpGlobals->time + 1.7;
}

void CGrenadeBeam::Update( void )
{
	CBaseEntity *pEntity = NULL;
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_LONG(-128,128);	// Pick a random direction
	vecEnd.y = RANDOM_LONG(-128,128);
	vecEnd.z = RANDOM_LONG(-128,128);
	vecEnd = pev->origin + vecEnd.Normalize() * 512;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	
	if (pev->rendercolor.x >= 255)
		{
			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 360 )) != NULL)
			{
				if (pEntity && !(pEntity->pev->health <= 3))
				{
					//::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(30,50), 256, CLASS_NONE, DMG_MORTAR  );
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
						WRITE_BYTE( 3 ); // life * 0.1
						WRITE_BYTE( 8 ); // width
						WRITE_BYTE( 24 ); // noise
						WRITE_BYTE( 200 ); // color r,g,b
						WRITE_BYTE( 200 ); // color r,g,b
						WRITE_BYTE( 255 ); // color r,g,b
						WRITE_BYTE( 160 ); // brightness
						WRITE_BYTE( 256 ); // scroll speed
					MESSAGE_END();
					
					UTIL_ScreenShake( pEntity->pev->origin, 12.0, 90.5, 0.3, 1 );
					UTIL_ScreenFade( pEntity, Vector(255,255,250), 1, 0.84, 128, FFADE_IN ); //flash 
					pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(7,14), DMG_MORTAR);	

				}
			
			}
		// balls
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( pev->origin.x + tr.vecPlaneNormal.x*3 );
			WRITE_COORD( pev->origin.y  + tr.vecPlaneNormal.y*4 );
			WRITE_COORD( pev->origin.z + tr.vecPlaneNormal.z*5  );
			WRITE_SHORT( m_iBalls );		// model
			WRITE_BYTE( 1  );				// count
			WRITE_BYTE( 1 );				// life * 10
			WRITE_BYTE( 1 );				// size * 10
			WRITE_BYTE( 64 );				// amplitude * 0.1
			WRITE_BYTE( 8 );				// speed * 100
		MESSAGE_END();
		
		//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(vecSrc.x);	// X
			WRITE_COORD(vecSrc.y);	// Y
			WRITE_COORD(vecSrc.z);	// Z
			WRITE_BYTE( 16 );		// radius * 0.1
			WRITE_BYTE( RANDOM_LONG(0,200) );		// r
			WRITE_BYTE( 100 );		// g
			WRITE_BYTE( 255 );		// b
			WRITE_BYTE( 128 );		// time * 10
			WRITE_BYTE( 16 );		// decay * 0.1
		MESSAGE_END( );
		
		//play sounds
		switch(RANDOM_LONG(0,6))
			{
			case 0: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark1.wav", 1.0, ATTN_NORM); //play sound
			break;
			case 1: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark2.wav", 1.0, ATTN_NORM); //play sound
			break;
			case 2: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark3.wav", 1.0, ATTN_NORM); //play sound
			break;
			case 3: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark4.wav", 1.0, ATTN_NORM); //play sound
			break;
			case 4: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark5.wav", 1.0, ATTN_NORM); //play sound
			break;
			case 5: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark6.wav", 1.0, ATTN_NORM); //play sound
			break;
			case 6: 
				EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark3.wav", 1.0, ATTN_NORM); //play sound
			break;
			
			
			}
		
		UTIL_Remove( this );
		
		
		}
	
	pev->nextthink = gpGlobals->time + 0.2;

}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


void CGrenade:: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING( "grenade" );
	
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/grenade.mdl");
	UTIL_SetSize(pev, Vector( -1, -1, 0), Vector(1, 1, 0));

	pev->dmg = 100;
	m_fRegisteredSound = FALSE;
	
	
}


CGrenade *CGrenade::ShootContact( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGrenade *pGrenade = GetClassPtr( (CGrenade *)NULL );
	pGrenade->Spawn();
	// contact grenades arc lower
	pGrenade->pev->gravity = 0.5;// lower gravity since grenade is aerodynamic and engine doesn't know it.
	UTIL_SetOrigin( pGrenade->pev, vecStart );
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles (pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);
	
	// make monsters afaid of it while in the air
	pGrenade->SetThink( DangerSoundThink );
	pGrenade->pev->nextthink = gpGlobals->time;
	
	// Tumble in air
	pGrenade->pev->avelocity.x = RANDOM_FLOAT ( -100, -500 );
	
	// Explode on contact
	pGrenade->SetTouch( ExplodeTouch );

	pGrenade->pev->dmg = gSkillData.plrDmgM203Grenade;

	return pGrenade;
}


CGrenade * CGrenade:: ShootTimed( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CGrenade *pGrenade = GetClassPtr( (CGrenade *)NULL );
	pGrenade->Spawn();
	UTIL_SetOrigin( pGrenade->pev, vecStart );
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);
	
	pGrenade->SetTouch( BounceTouch );	// Bounce if touched
	
	// Take one second off of the desired detonation time and set the think to PreDetonate. PreDetonate
	// will insert a DANGER sound into the world sound list and delay detonation for one second so that 
	// the grenade explodes after the exact amount of time specified in the call to ShootTimed(). 

	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink( TumbleThink );
	pGrenade->pev->nextthink = gpGlobals->time + 0.1;
	if (time < 0.1)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector( 0, 0, 0 );
	}
		
	pGrenade->pev->sequence = RANDOM_LONG( 3, 6 );
	pGrenade->pev->framerate = 1.0;

	// Tumble through the air
	// pGrenade->pev->avelocity.x = -400;

	pGrenade->pev->gravity = 0.5;
	pGrenade->pev->friction = 0.8;

	SET_MODEL(ENT(pGrenade->pev), "models/w_grenade.mdl");
	pGrenade->pev->dmg = RANDOM_LONG(100,116);

	return pGrenade;
}

//////
//////1.31 fire gren
//////

CGrenade * CGrenade:: ShootTimed2( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time )
{
	CGrenade *pGrenade = GetClassPtr( (CGrenade *)NULL );
	
	pGrenade->Spawn();
	UTIL_SetOrigin( pGrenade->pev, vecStart );
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);
	
	
	pGrenade->SetTouch( BounceTouch );	// Bounce if touched
	
	// Take one second off of the desired detonation time and set the think to PreDetonate. PreDetonate
	// will insert a DANGER sound into the world sound list and delay detonation for one second so that 
	// the grenade explodes after the exact amount of time specified in the call to ShootTimed(). 

	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink( TumbleThink2 );
	pGrenade->pev->nextthink = gpGlobals->time + 0.3;
	if (time < 0.1)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector( 0, 0, 0 );
	}
		
	pGrenade->pev->sequence = RANDOM_LONG( 3, 6 );
	pGrenade->pev->framerate = 1.0;
	pGrenade->pev->gravity = 0.5;
	pGrenade->pev->friction = 0.8;
	SET_MODEL(ENT(pGrenade->pev), "models/w_grenade.mdl");
	
	pGrenade->pev->dmg = 50;
	
	//effects
	pGrenade->pev->rendermode = kRenderNormal;
	pGrenade->pev->renderfx = kRenderFxGlowShell;
	pGrenade->pev->rendercolor.x = 255;  // red
	pGrenade->pev->rendercolor.y = 128;  // green
	pGrenade->pev->rendercolor.z = 0; // blue
	pGrenade->pev->renderamt = 160;
	
	
	


	return pGrenade;
}


CGrenade * CGrenade :: ShootSatchelCharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity )
{
	CGrenade *pGrenade = GetClassPtr( (CGrenade *)NULL );
	pGrenade->pev->movetype = MOVETYPE_BOUNCE;
	pGrenade->pev->classname = MAKE_STRING( "grenade" );
	
	pGrenade->pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl");	// Change this to satchel charge model

	UTIL_SetSize(pGrenade->pev, Vector( -1, -1, 0), Vector(1, 1, 0));

	pGrenade->pev->dmg = 200;
	UTIL_SetOrigin( pGrenade->pev, vecStart );
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = g_vecZero;
	pGrenade->pev->owner = ENT(pevOwner);
	
	// Detonate in "time" seconds
	pGrenade->SetThink( SUB_DoNothing );
	pGrenade->SetUse( DetonateUse );
	pGrenade->SetUse( DetonateUse2 ); //for second satchels
	pGrenade->SetTouch( SlideTouch );
	pGrenade->pev->spawnflags = SF_DETONATE;

	pGrenade->pev->friction = 0.9;

	return pGrenade;
}



void CGrenade :: UseSatchelCharges( entvars_t *pevOwner, SATCHELCODE code )
{
	edict_t *pentFind;
	edict_t *pentOwner;

	if ( !pevOwner )
		return;

	CBaseEntity	*pOwner = CBaseEntity::Instance( pevOwner );

	pentOwner = pOwner->edict();

	pentFind = FIND_ENTITY_BY_CLASSNAME( NULL, "grenade" );
	while ( !FNullEnt( pentFind ) )
	{
		CBaseEntity *pEnt = Instance( pentFind );
		if ( pEnt )
		{
			if ( FBitSet( pEnt->pev->spawnflags, SF_DETONATE ) && pEnt->pev->owner == pentOwner )
			{
				if ( code == SATCHEL_DETONATE )
					pEnt->Use( pOwner, pOwner, USE_ON, 0 );
				else	// SATCHEL_RELEASE
					pEnt->pev->owner = NULL;
			}
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME( pentFind, "grenade" );
	}
}

//======================end grenade

