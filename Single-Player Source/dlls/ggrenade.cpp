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
#include "game.h"
#include "player.h"
#include "gamerules.h"

//===================grenade

LINK_ENTITY_TO_CLASS(grenade, CGrenade);

// Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges
#define SF_DETONATE 0x0001

//
// Grenade Explode
//
void CGrenade::Explode(Vector vecSrc, Vector vecAim)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0, 0, 32), ignore_monsters, ENT(pev), &tr);

	Explode(&tr, DMG_CRUSH);
}

// UNDONE: temporary scorching for PreAlpha - find a less sleazy permenant solution.
void CGrenade::Explode(TraceResult *pTrace, int bitsDamageType)
{

	pev->model = iStringNull;
	pev->solid = SOLID_NOT;

	pev->takedamage = DAMAGE_NO;

	if (pTrace->flFraction != 1.0)
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * (pev->dmg - 24) * 0.6);

	//int iContents = UTIL_PointContents(pev->origin);

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION);	// This makes a dynamic light and the explosion sprites/sound
	WRITE_COORD(pev->origin.x); // Send to PAS because of the sound
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(g_sModelIndexFireball);
	WRITE_BYTE(pev->dmg * 0.5); // explode scale, fixed 1.30
	WRITE_BYTE(16);				// framerate
	WRITE_BYTE(TE_EXPLFLAG_NONE);
	MESSAGE_END();

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0);
	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	pev->owner = NULL;

	RadiusDamage(pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType);

	UTIL_DecalTrace(pTrace, DECAL_SCORCH2);

	pev->effects |= EF_NODRAW;
	

	// if (iContents != CONTENTS_WATER)
	// {
	// 	int sparkCount = RANDOM_LONG(0, 7);
	// 	for (int i = 0; i < sparkCount; i++)
	// 		Create("spark_shower", pev->origin, pTrace->vecPlaneNormal, NULL);
	// }

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLODEMODEL);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);			 ///
	WRITE_SHORT(g_sModelIndexBloodDrop); // entity
	WRITE_SHORT(24);					 // model
	WRITE_BYTE(16);						 // life
	MESSAGE_END();

	SetThink(Smoke);
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.02;
}

void CGrenade::Smoke(void)
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(pev->dmg); // smoke scale * 10
		WRITE_BYTE(16);		  // framerate
		MESSAGE_END();
	}
	UTIL_Remove(this);
}

void CGrenade::Killed(entvars_t *pevAttacker, int iGib)
{
	Detonate();
}

// Timed grenade, this think is called when time runs out.
void CGrenade::DetonateUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	SetThink(Detonate);
	pev->nextthink = gpGlobals->time;
}

short m_Sprite;

// Timed secondary satchels, exlode after 2 sec with sound
void CGrenade::DetonateUse2(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	pev->movetype = MOVETYPE_NONE;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/warningbell1.wav", 1.0, ATTN_NORM);

	m_Sprite = PRECACHE_MODEL("sprites/xflare1.spr");

	// animated sprite
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITE);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(m_Sprite);
	WRITE_BYTE(30);	 // scale * 10
	WRITE_BYTE(200); // brightness
	MESSAGE_END();

	SetThink(Detonate);
	pev->nextthink = gpGlobals->time + 2;
}

void CGrenade::PreDetonate(void)
{
	CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, 400, 0.3);

	SetThink(Detonate);
	pev->nextthink = gpGlobals->time + 1;
}

void CGrenade::Detonate(void)
{
	TraceResult tr;
	Vector vecSpot; // trace starts here!

	vecSpot = pev->origin + Vector(0, 0, 8);
	UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, 8), ignore_monsters, ENT(pev), &tr);

	Explode(&tr, DMG_CRUSH);
}


void CGrenade::DangerSoundThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * 0.5, pev->velocity.Length(), 0.2);
	pev->nextthink = gpGlobals->time + 0.2;

	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
	}
}

void CGrenade::BounceTouch(CBaseEntity *pOther)
{
	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	// only do damage if we're moving fairly fast
	if (m_flNextAttack < gpGlobals->time && pev->velocity.Length() > 100)
	{
		entvars_t *pevOwner = VARS(pev->owner);
		if (pevOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace();
			ClearMultiDamage();
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_CLUB);
			if (pOther->pev->solid != SOLID_BSP)
				ExplodeTouch(pOther);
			ApplyMultiDamage(pev, pevOwner);
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}

	Vector vecTestVelocity;

	vecTestVelocity = pev->velocity;
	vecTestVelocity.z *= 0.45;

	if (!m_fRegisteredSound && vecTestVelocity.Length() <= 60)
	{
		CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, pev->dmg, 0.3);
		m_fRegisteredSound = TRUE;
	}

	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.8;
	else
		BounceSound();

	pev->framerate = pev->velocity.Length();

	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;
}

void CGrenade::SlideTouch(CBaseEntity *pOther)
{
	if (pOther->edict() == pev->owner)
		return;

	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.95;
	else
		BounceSound();
}

void CGrenade ::BounceSound(void)
{
	switch (RANDOM_LONG(0, 2))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM);
		break;
	}
}

void CGrenade ::TumbleThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime - 1 < gpGlobals->time)
		CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1);

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (g_zxc_promode.value == 0)
		{
			CBasePlayer *m_pPlayer = (CBasePlayer *)CBasePlayer::Instance(pev->owner);
			if (m_pPlayer && FBitSet(pev->flags, FL_ONGROUND))
			{
				TraceResult tr2;
				UTIL_TraceHull(pev->origin + Vector( 0, 0, 36 ), pev->origin + Vector(0,0,44), ignore_monsters, human_hull, ENT(pev), &tr2);
				if ( !tr2.fStartSolid )
				{
					if (m_pPlayer->m_flNextTurretsLimit < 3)
					{
						CBaseEntity *pMonstro = CBaseEntity::Create("monster_alien_grunt", pev->origin, g_vecZero, m_pPlayer->edict());
						pMonstro->pev->owner = m_pPlayer->edict();
						pMonstro->Classify2 = CLASS_MACHINE; 
						m_pPlayer->m_flNextTurretsLimit++;
						pMonstro->pev->weapons = 1;
					}
				}
			}
		}

		SetThink(Detonate);
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}

void CGrenade ::TumbleThink2( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	int xBalls = 5;

	CBaseEntity *pEntity = NULL;
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->dmgtime <= gpGlobals->time)
	{

		Vector vecSrc;
		vecSrc.x = RANDOM_LONG(0, 360);
		vecSrc.y = RANDOM_LONG(0, 360);
		vecSrc.z = 0;

		for (int i = 0; i < xBalls; i++) //  + (gpGlobals->v_right*RANDOM_LONG(-1,1)) + (gpGlobals->v_forward*RANDOM_LONG(-1,1))
		{
			Create("env_particleemitter", pev->origin + (gpGlobals->v_up*(1 + i*48)), pev->angles, pev->owner);
		}

		::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, 512, CLASS_NONE, DMG_ENERGYBEAM); // end blast
		

		SetThink(Detonate);
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5;
		pev->framerate = 0.2;
	}
}

void CGrenade ::TumbleThink3(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	CBaseEntity *pEntity = NULL;
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	// if (pev->dmgtime - 1 < gpGlobals->time)
	// 	CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1);

	if (pev->dmgtime <= gpGlobals->time)
	{
		CBaseEntity *pBullGrena;
		Vector vecSrc = pev->origin + Vector(0,0,15);

		// vecSrc.x = RANDOM_LONG(0, 360);
		// vecSrc.y = RANDOM_LONG(0, 360);
		// vecSrc.z = 0;
		for (size_t i = 0; i < 21; i++)
		{
			if (CGameRules::EntCounter < 400) 
			{
				pBullGrena = Create("squidspit", vecSrc, pev->angles, pev->owner);
				UTIL_SetSize(pBullGrena->pev, Vector(0, 0, 0), Vector(0, 0, 0));
				pBullGrena->pev->velocity = (gpGlobals->v_forward * RANDOM_LONG(-360, 360) ) + (gpGlobals->v_right * RANDOM_LONG(-360, 360) ) + (gpGlobals->v_up * RANDOM_LONG(-0, 90));
				pBullGrena->pev->velocity = pBullGrena->pev->velocity * 7;
				pBullGrena->pev->framerate = 1.0; 
				pBullGrena->pev->scale = 1.5;
			}
		}
		
		
		

		SetThink(Detonate);
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
// grenade beams, copied and changed from egon.cpp
class CGrenadeBeam : public CBaseEntity
{
	void Spawn();
	void EXPORT Update(void);

private:
	short m_LaserSprite;
	short m_LaserSprite2;
	int m_iBalls;
};
LINK_ENTITY_TO_CLASS(env_particleemitter, CGrenadeBeam); // storm beam entity

void CGrenadeBeam::Spawn(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 1.0;
	pev->friction = 0.0;

	pev->health = 5000.0;

	pev->solid = SOLID_NOT;
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin(pev, pev->origin);

	pev->classname = MAKE_STRING("weapon_grenade");

	// sprite
	SET_MODEL(ENT(pev), "sprites/blueflare2.spr");
	m_iBalls = PRECACHE_MODEL("sprites/gradbeam.spr");
	m_LaserSprite = PRECACHE_MODEL("sprites/laserbeam.spr");
	m_LaserSprite2 = PRECACHE_MODEL("sprites/plasma.spr");

	// effects
	pev->rendermode = kRenderTransAdd;
	pev->rendercolor.x = 255; // red
	pev->rendercolor.y = 255; // green
	pev->rendercolor.z = 255; // blue
	pev->renderamt = 250;
	pev->scale = 1.25;

	pev->dmg = 12.0;

	pev->velocity.z = 370;					  // jump strong
	pev->velocity.y = RANDOM_LONG(-160, 160); // jump strong 2
	pev->velocity.x = RANDOM_LONG(-160, 160); // jump strong 3

	switch (RANDOM_LONG(0, 1))
	{
	case 0:
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMFOLLOW);
		WRITE_SHORT(entindex());	 // entity
		WRITE_SHORT(m_LaserSprite2); // model
		WRITE_BYTE(16);				 // life
		WRITE_BYTE(3);				 // width
		WRITE_BYTE(80);				 // r, g, b
		WRITE_BYTE(200);			 // r, g, b
		WRITE_BYTE(200);			 // r, g, b
		WRITE_BYTE(160);			 // brightness
		MESSAGE_END();
		break;
	}

	SetThink(Update);
	pev->nextthink = gpGlobals->time + 1.7;
}

void CGrenadeBeam::Update(void)
{
	CBaseEntity *pEntity = NULL;
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_LONG(-128, 128); // Pick a random direction
	vecEnd.y = RANDOM_LONG(-128, 128);
	vecEnd.z = RANDOM_LONG(-128, 128);
	vecEnd = pev->origin + vecEnd.Normalize() * 512;
	UTIL_TraceLine(pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);

	if (pev->rendercolor.x >= 255)
	{
		while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 360)) != NULL)
		{
			if (pEntity && FVisible(pEntity))
			{
				// ::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(30,50), 256, CLASS_NONE, DMG_MORTAR  );
				// shock ray
				MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
				WRITE_BYTE(TE_BEAMPOINTS);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
				WRITE_COORD(pEntity->pev->origin.y);
				WRITE_COORD(pEntity->pev->origin.z);
				WRITE_SHORT(m_LaserSprite); // sprite
				WRITE_BYTE(1);				// Starting frame
				WRITE_BYTE(0);				// framerate * 0.1
				WRITE_BYTE(3);				// life * 0.1
				WRITE_BYTE(8);				// width
				WRITE_BYTE(24);				// noise
				WRITE_BYTE(200);			// color r,g,b
				WRITE_BYTE(200);			// color r,g,b
				WRITE_BYTE(255);			// color r,g,b
				WRITE_BYTE(160);			// brightness
				WRITE_BYTE(256);			// scroll speed
				MESSAGE_END();

				UTIL_ScreenShake(pEntity->pev->origin, 12.0, 90.5, 0.3, 1);
				UTIL_ScreenFade(pEntity, Vector(205, 205, 250), 1, 0.84, 200, FFADE_IN); // flash
				pEntity->TakeDamage(pev, VARS(pev->owner), pev->dmg, DMG_SHOCK);
			}
		}
		// balls
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SPRITETRAIL); // TE_RAILTRAIL);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x + tr.vecPlaneNormal.x * 3);
		WRITE_COORD(pev->origin.y + tr.vecPlaneNormal.y * 4);
		WRITE_COORD(pev->origin.z + tr.vecPlaneNormal.z * 5);
		WRITE_SHORT(m_iBalls); // model
		WRITE_BYTE(8);		   // count
		WRITE_BYTE(8);		   // life * 10
		WRITE_BYTE(1);		   // size * 10
		WRITE_BYTE(64);		   // amplitude * 0.1
		WRITE_BYTE(8);		   // speed * 100
		MESSAGE_END();

		// lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x); // X
		WRITE_COORD(vecSrc.y); // Y
		WRITE_COORD(vecSrc.z); // Z
		WRITE_BYTE(16);		   // radius * 0.1
		WRITE_BYTE(200);	   // r
		WRITE_BYTE(100);	   // g
		WRITE_BYTE(255);	   // b
		WRITE_BYTE(128);	   // time * 10
		WRITE_BYTE(16);		   // decay * 0.1
		MESSAGE_END();

		// play sounds
		switch (RANDOM_LONG(0, 6))
		{
		case 0:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark1.wav", 1.0, ATTN_NORM); // play sound
			break;
		case 1:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark2.wav", 1.0, ATTN_NORM); // play sound
			break;
		case 2:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark3.wav", 1.0, ATTN_NORM); // play sound
			break;
		case 3:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark4.wav", 1.0, ATTN_NORM); // play sound
			break;
		case 4:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark5.wav", 1.0, ATTN_NORM); // play sound
			break;
		case 5:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark6.wav", 1.0, ATTN_NORM); // play sound
			break;
		case 6:
			EMIT_SOUND(ENT(pev), CHAN_BODY, "buttons/spark3.wav", 1.0, ATTN_NORM); // play sound
			break;
		}

		UTIL_Remove(this);
	}

	pev->nextthink = gpGlobals->time + 0.2;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

void CGrenade::Spawn(void)
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->classname = MAKE_STRING("grenade");

	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/grenade.mdl");
	UTIL_SetSize(pev, Vector(-3, -3, 0), Vector(3, 3, 3));

	pev->dmg = 100.0;
	pev->health = 5000.0;

	m_fRegisteredSound = FALSE;
}

CGrenade *CGrenade::ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL);
	pGrenade->Spawn();
	// contact grenades arc lower
	pGrenade->pev->gravity = 0.5; // lower gravity since grenade is aerodynamic and engine doesn't know it.
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);

	// make monsters afaid of it while in the air
	pGrenade->SetThink(DangerSoundThink);
	pGrenade->pev->nextthink = gpGlobals->time;

	// Tumble in air
	pGrenade->pev->avelocity.x = RANDOM_LONG(-100, -500);

	// Explode on contact
	pGrenade->SetTouch(ExplodeTouch);

	pGrenade->pev->dmg = 120.0;

	return pGrenade;
}


//
// Contact grenade, explode when it touches something
//
void CGrenade::ExplodeTouch(CBaseEntity *pOther)
{
	TraceResult tr;
	Vector vecSpot; // trace starts here!

	pev->enemy = pOther->edict();

	vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine(vecSpot, vecSpot + pev->velocity.Normalize() * 64, ignore_monsters, ENT(pev), &tr);




	// CBaseEntity *pEntity = NULL;
	// Vector vecDir;
	// Vector vecDirToEnemy;

	// while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, pev->dmg*3)) != NULL)
	// {
	// 	Vector vecMid = pev->origin;					  // get self
	// 	Vector vecMidEnemy = pEntity->BodyTarget(vecMid); // get target
	// 	vecDirToEnemy = vecMidEnemy - vecMid;			  // calculate dir and dist to enemy

	// 	if (pEntity->pev->takedamage>0)
	// 	{
	// 		vecDir = (pEntity->Center() - Center()).Normalize();
	// 		pEntity->pev->velocity =  -(vecDir *  vecDirToEnemy.Length() * 5);
	// 	}
	// }

	


	Explode(&tr, DMG_CRUSH);
}

CGrenade *CGrenade::ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL);
	pGrenade->Spawn();
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);

	pGrenade->SetTouch(BounceTouch); // Bounce if touched

	// Take one second off of the desired detonation time and set the think to PreDetonate. PreDetonate
	// will insert a DANGER sound into the world sound list and delay detonation for one second so that
	// the grenade explodes after the exact amount of time specified in the call to ShootTimed().

	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink(TumbleThink);
	pGrenade->pev->nextthink = gpGlobals->time + 0.1;
	if (time < 0.1)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector(0, 0, 0);
	}

	pGrenade->pev->sequence = RANDOM_LONG(3, 6);
	pGrenade->pev->framerate = 1.0;

	pGrenade->pev->gravity = 0.5;
	pGrenade->pev->friction = 0.8;

	SET_MODEL(ENT(pGrenade->pev), "models/w_grenade.mdl");
	pGrenade->pev->dmg = 100;

	return pGrenade;
}

//////
////// 1.31 electro gren
//////

CGrenade *CGrenade::ShootTimed2(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL);

	pGrenade->Spawn();
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);

	pGrenade->SetTouch(BounceTouch); // Bounce if touched

	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink(TumbleThink2);
	pGrenade->pev->nextthink = gpGlobals->time + 0.3;
	if (time < 0.1)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector(0, 0, 0);
	}

	pGrenade->pev->gravity = 0.5;
	pGrenade->pev->friction = 0.8;
	SET_MODEL(ENT(pGrenade->pev), "models/w_grenade.mdl");

	pGrenade->pev->dmg = 20.0;

	pGrenade->pev->rendermode = kRenderNormal;
	pGrenade->pev->renderfx = kRenderFxGlowShell;
	pGrenade->pev->rendercolor.x = 10; // red
	pGrenade->pev->rendercolor.y = 128; // green
	pGrenade->pev->rendercolor.z = 255;	// blue
	pGrenade->pev->renderamt = 160;

	return pGrenade;
}

//////
////// 1.38 bullsq gren
//////

CGrenade *CGrenade::ShootTimed3(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL);

	pGrenade->Spawn();
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);

	pGrenade->SetTouch(BounceTouch); // Bounce if touched

	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->SetThink(TumbleThink3);
	pGrenade->pev->nextthink = gpGlobals->time + 0.3;
	if (time < 0.1)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector(0, 0, 0);
	}

	pGrenade->pev->gravity = 0.5;
	pGrenade->pev->friction = 0.8;
	SET_MODEL(ENT(pGrenade->pev), "models/w_grenade.mdl");

	pGrenade->pev->dmg = 128;

	pGrenade->pev->rendermode = kRenderNormal;
	pGrenade->pev->renderfx = kRenderFxGlowShell;
	pGrenade->pev->rendercolor.x = 0; // red
	pGrenade->pev->rendercolor.y = 200; // green
	pGrenade->pev->rendercolor.z = 0;	// blue
	pGrenade->pev->renderamt = 20;

	return pGrenade;
}

CGrenade *CGrenade ::ShootSatchelCharge(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL);
	pGrenade->pev->movetype = MOVETYPE_BOUNCE;
	pGrenade->pev->classname = MAKE_STRING("grenade");

	pGrenade->pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl"); // Change this to satchel charge model

	UTIL_SetSize(pGrenade->pev, Vector(-1, -1, 0), Vector(1, 1, 0));

	pGrenade->pev->dmg = 200;
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = g_vecZero;
	pGrenade->pev->owner = ENT(pevOwner);

	// Detonate in "time" seconds
	pGrenade->SetThink(SUB_DoNothing);
	pGrenade->SetUse(DetonateUse);
	pGrenade->SetUse(DetonateUse2); // for second satchels
	pGrenade->SetTouch(SlideTouch);
	pGrenade->pev->spawnflags = SF_DETONATE;

	pGrenade->pev->friction = 0.9;

	return pGrenade;
}

void CGrenade ::UseSatchelCharges(entvars_t *pevOwner, SATCHELCODE code)
{
	edict_t *pentFind;
	edict_t *pentOwner;

	if (!pevOwner)
		return;

	CBaseEntity *pOwner = CBaseEntity::Instance(pevOwner);

	pentOwner = pOwner->edict();

	pentFind = FIND_ENTITY_BY_CLASSNAME(NULL, "grenade");
	while (!FNullEnt(pentFind))
	{
		CBaseEntity *pEnt = Instance(pentFind);
		if (pEnt)
		{
			if (FBitSet(pEnt->pev->spawnflags, SF_DETONATE) && pEnt->pev->owner == pentOwner)
			{
				if (code == SATCHEL_DETONATE)
					pEnt->Use(pOwner, pOwner, USE_ON, 0);
				else // SATCHEL_RELEASE
					pEnt->pev->owner = NULL;
			}
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME(pentFind, "grenade");
	}
}

//======================end grenade
