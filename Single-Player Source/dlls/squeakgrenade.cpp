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
#include "gamerules.h"

enum w_squeak_e
{
	WSQUEAK_IDLE1 = 0,
	WSQUEAK_FIDGET,
	WSQUEAK_JUMP,
	WSQUEAK_RUN,
};

enum squeak_e
{
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};

class CSqueakGrenade : public CGrenade
{
	void Spawn(void);
	void Precache(void);
	int Classify(void);
	void EXPORT SuperBounceTouch(CBaseEntity *pOther);
	void EXPORT HuntThink(void);
	int BloodColor(void) { return BLOOD_COLOR_YELLOW; }
	void Killed(entvars_t *pevAttacker, int iGib);
	void GibMonster(void);
	int IRelationship(CBaseEntity *pTarget);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	static float m_flNextBounceSoundTime;

	// CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	int m_iMyClass;
};

float CSqueakGrenade::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS(monster_snark, CSqueakGrenade);
TYPEDESCRIPTION CSqueakGrenade::m_SaveData[] =
	{
		DEFINE_FIELD(CSqueakGrenade, m_flDie, FIELD_TIME),
		DEFINE_FIELD(CSqueakGrenade, m_vecTarget, FIELD_VECTOR),
		DEFINE_FIELD(CSqueakGrenade, m_flNextHunt, FIELD_TIME),
		DEFINE_FIELD(CSqueakGrenade, m_flNextHit, FIELD_TIME),
		DEFINE_FIELD(CSqueakGrenade, m_posPrev, FIELD_POSITION_VECTOR),
		DEFINE_FIELD(CSqueakGrenade, m_hOwner, FIELD_EHANDLE),
};

IMPLEMENT_SAVERESTORE(CSqueakGrenade, CGrenade);

#define SQUEEK_DETONATE_DELAY 15.0

int CSqueakGrenade::IRelationship(CBaseEntity *pTarget)
{
	if (pTarget->pev->modelindex == pev->modelindex)
	{
		return R_NO;
	}

	return CBaseMonster ::IRelationship(pTarget);
}

//=========================================================
// ID's Hornet as their owner
//=========================================================
int CSqueakGrenade::Classify()
{

	if (pev->owner && pev->owner->v.flags & FL_CLIENT)
	{
		return CLASS_PLAYER_BIOWEAPON;
	}

	return CLASS_ALIEN_BIOWEAPON;
}

void CSqueakGrenade ::Spawn(void)
{

	Precache();
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_squeak.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, 0), Vector(4, 4, 8));
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(SuperBounceTouch);
	SetThink(HuntThink);

	

	pev->nextthink = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time + 1.5;

	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_AIM;
	pev->health = 3.0;
	pev->gravity = 0.5;
	pev->friction = 0.3;
	pev->dmg = 25.0;
	pev->weapons = 1; // no frags point

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());			  // entity
	WRITE_SHORT(g_sModelIndexSmokeTrail); // model
	WRITE_BYTE(16);						  // life
	WRITE_BYTE(1.5);					  // width
	WRITE_BYTE(255);					  // r, g, b
	WRITE_BYTE(55);						  // r, g, b
	WRITE_BYTE(55);						  // r, g, b
	WRITE_BYTE(126);					  // brightness
	MESSAGE_END();

	Classify2 = Classify();
	m_flFieldOfView = 0.4;

	if (pev->owner)
		m_hOwner = Instance(pev->owner);

	m_flNextBounceSoundTime = gpGlobals->time; // reset each time a snark is spawned.

	pev->sequence = WSQUEAK_RUN;
	ResetSequenceInfo();

	m_flDie = gpGlobals->time + 6.0;
}

void CSqueakGrenade::Precache(void)
{
	PRECACHE_MODEL("models/headcrab.mdl");
	PRECACHE_MODEL("models/w_squeak.mdl");
	PRECACHE_MODEL("models/crystal2.mdl");
	PRECACHE_SOUND("squeek/sqk_blast1.wav");
	PRECACHE_SOUND("common/bodysplat.wav");
	PRECACHE_SOUND("squeek/sqk_die1.wav");
	PRECACHE_SOUND("squeek/sqk_hunt1.wav");
	PRECACHE_SOUND("squeek/sqk_hunt2.wav");
	PRECACHE_SOUND("squeek/sqk_hunt3.wav");
	PRECACHE_SOUND("squeek/sqk_deploy1.wav");
	PRECACHE_SOUND("zxc/crystal_heal.wav");
	PRECACHE_MODEL("sprites/laserbeam.spr");

	PRECACHE_MODEL("models/crystal4.mdl");
	PRECACHE_SOUND("zxc/snd3.wav");
	PRECACHE_SOUND("zxc/electrofreq.wav");
}

void CSqueakGrenade ::Killed(entvars_t *pevAttacker, int iGib)
{
	pev->model = iStringNull; // make invisible
	SetThink(SUB_Remove);
	SetTouch(NULL);
	pev->nextthink = gpGlobals->time + 0.1;

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	pev->takedamage = DAMAGE_NO;

	// play squeek blast
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "squeek/sqk_blast1.wav", 1, 0.5, 0, PITCH_NORM);

	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, SMALL_EXPLOSION_VOLUME, 3.0);

	UTIL_BloodDrips(pev->origin, g_vecZero, BloodColor(), 80);

	if (m_hOwner != NULL)
		RadiusDamage(pev, m_hOwner->pev, pev->dmg, CLASS_NONE, DMG_BLAST);
	else
		RadiusDamage(pev, pev, pev->dmg, CLASS_NONE, DMG_BLAST);

	// reset owner so death message happens
	if (m_hOwner != NULL)
		pev->owner = m_hOwner->edict();

	CBaseMonster ::Killed(pevAttacker, GIB_ALWAYS);
}

void CSqueakGrenade ::GibMonster(void)
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);
}

void CSqueakGrenade::HuntThink(void)
{
	// ALERT( at_console, "think\n" );

	if (!IsInWorld())
	{
		SetTouch(NULL);
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize();
		pev->health = -1;
		Killed(pev, 0);
		return;
	}

	// float
	if (pev->waterlevel != 0)
	{
		if (pev->movetype == MOVETYPE_BOUNCE)
		{
			pev->movetype = MOVETYPE_FLY;
		}
		pev->velocity = pev->velocity * 0.9;
		pev->velocity.z += 8.0;
	}
	else if (pev->movetype = MOVETYPE_FLY)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}

	// return if not time to hunt
	if (m_flNextHunt > gpGlobals->time)
		return;

	m_flNextHunt = gpGlobals->time + 2.0;

	CBaseEntity *pOther = NULL;
	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize();

	UTIL_MakeVectors(pev->angles);

	if (m_hEnemy == NULL || !m_hEnemy->IsAlive())
	{
		// find target, bounce a bit towards it.
		Look(512);
		m_hEnemy = BestVisibleEnemy();
	}

	// squeek if it's about time blow up
	if ((m_flDie - gpGlobals->time <= 0.5) && (m_flDie - gpGlobals->time >= 0.3))
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_die1.wav", 1, ATTN_NORM, 0, 100 + RANDOM_LONG(0, 0x3F));
		CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 256, 0.25);
	}

	// higher pitch as squeeker gets closer to detonation time
	float flpitch = 155.0 - 60.0 * ((m_flDie - gpGlobals->time) / SQUEEK_DETONATE_DELAY);
	if (flpitch < 80)
		flpitch = 80;

	if (m_hEnemy != NULL)
	{
		if (FVisible(m_hEnemy))
		{
			vecDir = m_hEnemy->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize();
		}

		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;

		// ALERT( at_console, "think : enemy\n");

		// ALERT( at_console, "%.0f %.2f %.2f %.2f\n", flVel, m_vecTarget.x, m_vecTarget.y, m_vecTarget.z );

		pev->velocity = pev->velocity * flAdj + m_vecTarget * 300;
	}

	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector(0, 0, 0);
	}
	else
	{
		if (pev->avelocity == Vector(0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT(-100, 100);
			pev->avelocity.z = RANDOM_FLOAT(-100, 100);
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT(-100, 100);
		pev->velocity.y = RANDOM_FLOAT(-100, 100);
	}
	m_posPrev = pev->origin;

	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->angles.z = 0;
	pev->angles.x = 0;
}

void CSqueakGrenade::SuperBounceTouch(CBaseEntity *pOther)
{
	float flpitch;

	TraceResult tr = UTIL_GetGlobalTrace();

	// don't hit the guy that launched this grenade
	// if (pev->owner && pOther->edict() == pev->owner)
	// 	return;

	// at least until we've bounced once
	pev->owner = NULL;

	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	// higher pitch as squeeker gets closer to detonation time
	flpitch = 155.0 - 60.0 * ((m_flDie - gpGlobals->time) / SQUEEK_DETONATE_DELAY);

	if (pOther->pev->takedamage && m_flNextAttack < gpGlobals->time)
	{
		// attack!

		// make sure it's me who has touched them
		if (tr.pHit == pOther->edict())
		{
			// and it's not another squeakgrenade
			if (tr.pHit->v.modelindex != pev->modelindex)
			{
				// ALERT( at_console, "hit enemy\n");
				ClearMultiDamage();
				pOther->TraceAttack(pev, 1.0, gpGlobals->v_forward, &tr, DMG_PARALYZE);
				if (m_hOwner != NULL)
					ApplyMultiDamage(pev, m_hOwner->pev);
				else
					ApplyMultiDamage(pev, pev);

				pev->dmg += 1.0; // add more explosion damage
				// m_flDie += 2.0; // add more life

				pev->health = -1.0;
				Killed(pev, 0);

				// make bite sound
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "squeek/sqk_deploy1.wav", 1.0, ATTN_NORM, 0, (int)flpitch);
				m_flNextAttack = gpGlobals->time + 0.5;
			}
		}
		else
		{
			// ALERT( at_console, "been hit\n");
		}
	}

	m_flNextHit = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time;

	if (g_pGameRules->IsMultiplayer())
	{
		// in multiplayer, we limit how often snarks can make their bounce sounds to prevent overflows.
		if (gpGlobals->time < m_flNextBounceSoundTime)
		{
			// too soon!
			return;
		}
	}

	if (!(pev->flags & FL_ONGROUND))
	{
		// play bounce sound
		float flRndSound = RANDOM_FLOAT(0, 1);

		if (flRndSound <= 0.33)
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt1.wav", 1, ATTN_NORM, 0, (int)flpitch);
		else if (flRndSound <= 0.66)
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, (int)flpitch);
		else
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, (int)flpitch);
		CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 256, 0.25);
	}
	else
	{
		// skittering sound
		CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 100, 0.1);
	}

	m_flNextBounceSoundTime = gpGlobals->time + 0.5; // half second.
}

LINK_ENTITY_TO_CLASS(weapon_snark, CSqueak);

void CSqueak::Spawn()
{
	Precache();
	m_iId = WEAPON_SNARK;
	SET_MODEL(ENT(pev), "models/w_sqknest.mdl");

	FallInit(); // get ready to fall down.

	m_iDefaultAmmo = SNARK_DEFAULT_GIVE;

	pev->sequence = 1;
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0;
	m_flNexTripleCrystalTime = gpGlobals->time; // start timer
}

void CSqueak::Precache(void)
{
	PRECACHE_MODEL("models/w_sqknest.mdl");
	PRECACHE_MODEL("models/v_squeak.mdl");
	PRECACHE_MODEL("models/p_squeak.mdl");
	PRECACHE_SOUND("squeek/sqk_hunt2.wav");
	PRECACHE_SOUND("squeek/sqk_hunt3.wav");
	PRECACHE_SOUND("garg/gar_stomp1.wav");
	UTIL_PrecacheOther("monster_snark");
	m_iSpriteTexture = PRECACHE_MODEL("sprites/shockwave.spr");

	m_usSnarkFire = PRECACHE_EVENT(1, "events/snarkfire.sc");
}

int CSqueak::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Snarks";
	p->iMaxAmmo1 = SNARK_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_SNARK;
	p->iWeight = SNARK_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}

BOOL CSqueak::Deploy()
{
	// play hunt sound
	float flRndSound = RANDOM_FLOAT(0.0, 1.0);

	if (flRndSound <= 0.5)
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 100);
	else
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 100);

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	return DefaultDeploy("models/v_squeak.mdl", "models/p_squeak.mdl", SQUEAK_UP, "squeak");
}

void CSqueak::Holster(int skiplocal /* = 0 */)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		m_pPlayer->pev->weapons &= ~(1 << WEAPON_SNARK);
		SetThink(DestroyItem);
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}

	SendWeaponAnim(SQUEAK_DOWN);
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CSqueak::FourthAttack(void)
{
	if (m_pPlayer->m_flNexTripleCrystalTime < gpGlobals->time)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 18)
		{
			Vector vecThrow = gpGlobals->v_forward * 50;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 18;
			CBaseEntity *pCr = Create("weapon_aicore", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, Vector(0, 0, 0), m_pPlayer->edict());
			pCr->pev->velocity = vecThrow;
			SendWeaponAnim(5);
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNexTripleCrystalTime = gpGlobals->time + 60;
		}
	}
}

void CSqueak::PrimaryAttack()
{

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1)
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		TraceResult tr;
		Vector trace_origin;

		// HACK HACK:  Ugly hacks to handle change in origin based on new physics code for players
		// Move origin up if crouched and start trace a bit outside of body ( 20 units instead of 16 )
		trace_origin = m_pPlayer->pev->origin;
		if (m_pPlayer->pev->flags & FL_DUCKING)
		{
			trace_origin = trace_origin - (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
		}

		// find place to toss monster
		UTIL_TraceLine(trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr);

		PLAYBACK_EVENT_FULL(FEV_GLOBAL, m_pPlayer->edict(), m_usSnarkFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0);

		if (tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > 0.25)
		{
			// player "shoot" animation
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifndef CLIENT_DLL
			CBaseEntity *pSqueak = CBaseEntity::Create("monster_snark", tr.vecEndPos, m_pPlayer->pev->v_angle, m_pPlayer->edict());
			pSqueak->pev->velocity = gpGlobals->v_forward * 200 + m_pPlayer->pev->velocity;
#endif

			// play hunt sound
			float flRndSound = RANDOM_FLOAT(0.0, 1.0);

			if (flRndSound <= 0.5)
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 97);
			else
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 103);

			m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;

			m_fJustThrown = 1;

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		}
	}
}

void CSqueak::SecondaryAttack(void)
{

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		TraceResult tr;
		Vector trace_origin;

		trace_origin = m_pPlayer->pev->origin;
		if (m_pPlayer->pev->flags & FL_DUCKING)
			trace_origin = trace_origin - (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);

		// find place to toss monster
		UTIL_TraceLine(trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr);

		PLAYBACK_EVENT_FULL(FEV_GLOBAL, m_pPlayer->edict(), m_usSnarkFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0);

		if (tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > 0.25)
		{
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);

			CBaseEntity *pSqueak = CBaseEntity::Create("monster_larve", tr.vecEndPos, m_pPlayer->pev->v_angle, m_pPlayer->edict());
			pSqueak->pev->velocity = gpGlobals->v_forward * 200 + m_pPlayer->pev->velocity;

			int RND = RANDOM_LONG(0, 1);

			if (RND == 0)
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 115);
			else
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 95);

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
			m_fJustThrown = 1;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
		}
	}
}

void CSqueak::ThirdAttack(void)
{
	pev->dmg = 35 * m_pPlayer->TripleShot;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1)
	{
		CBaseEntity *pEntity = NULL;

		while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 128 + pev->dmg)) != NULL)
		{
			if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict())) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				UTIL_ScreenShake(pEntity->pev->origin, 1024.0, 1.5, 0.7, 1);
				if (FVisible(pEntity))
				{
					pEntity->TakeDamage(pev, VARS(pev->owner), pev->dmg, DMG_POISON); // destroy all near thinks
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_BEAMPOINTS);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z + 30);
					WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(m_iSpriteTexture); // sprite
					WRITE_BYTE(0);				   // Starting frame
					WRITE_BYTE(0);				   // framerate * 0.1
					WRITE_BYTE(6);				   // life * 0.1
					WRITE_BYTE(12);				   // width
					WRITE_BYTE(0);				   // noise
					WRITE_BYTE(200);			   // color r,g,b
					WRITE_BYTE(25);				   // color r,g,b
					WRITE_BYTE(25);				   // color r,g,b
					WRITE_BYTE(200);			   // brightness
					WRITE_BYTE(0);				   // scroll speed
					MESSAGE_END();
				}
			}
		}
		// draw beam
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_BEAMCYLINDER);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z + 500); // reach damage radius over .2 seconds
		WRITE_SHORT(m_iSpriteTexture);
		WRITE_BYTE(0);				// startframe
		WRITE_BYTE(0);				// framerate
		WRITE_BYTE(3);				// life
		WRITE_BYTE(10);				// width
		WRITE_BYTE(0);				// noise
		WRITE_BYTE(250);			// r, g, b
		WRITE_BYTE(0);				// r, g, b
		WRITE_BYTE(0);				// r, g, b
		WRITE_BYTE(100 + pev->dmg); // brightness
		WRITE_BYTE(0);				// speed
		MESSAGE_END();

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "garg/gar_stomp1.wav", 1.0, ATTN_NORM);
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		m_pPlayer->pev->velocity.z = 340;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1; // * m_pPlayer->TripleShot;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	}
}

void CSqueak::WeaponIdle(void)
{

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_fJustThrown)
	{
		m_fJustThrown = 0;

		if (!m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()])
		{
			RetireWeapon();
			return;
		}

		SendWeaponAnim(SQUEAK_UP);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
		return;
	}

	int iAnim;
	float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
	if (flRand <= 0.75)
	{
		iAnim = SQUEAK_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 30.0 / 16 * (2);
	}
	else if (flRand <= 0.875)
	{
		iAnim = SQUEAK_FIDGETFIT;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 70.0 / 16.0;
	}
	else
	{
		iAnim = SQUEAK_FIDGETNIP;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 80.0 / 16.0;
	}
	SendWeaponAnim(iAnim);
}

////new

// class Dcrystal
class Dcrystal : public CBaseEntity
{
public:
	void Spawn(void);
	void EXPORT MoveThink(void);

private:
	short m_LaserSprite;
	int m_iSpriteTexture;
	int m_flDie;
};
LINK_ENTITY_TO_CLASS(weapon_aicore, Dcrystal);

// class Fcrystal - shield function
class Fcrystal : public CBaseEntity
{
public:
	void Spawn(void);
	void EXPORT MoveThink(void);

private:
	short m_LaserSprite;
	int m_iSpriteTexture;
	int m_flDie;
};
LINK_ENTITY_TO_CLASS(zone_shelter, Fcrystal);

class CSqueakGrenade2 : public CGrenade
{
	void Spawn(void);
	void Precache(void);
	int Classify();
	int IRelationship(CBaseEntity *pTarget);
	void EXPORT SuperBounceTouch(CBaseEntity *pOther);
	void EXPORT HuntThink(void);
	void EXPORT ParasiteThink(void);
	int BloodColor(void) { return BLOOD_COLOR_YELLOW; }
	void Killed(entvars_t *pevAttacker, int iGib);
	void GibMonster(void);

	static float m_flNextBounceSoundTime;

	CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	//entvars_t	*m_paraTarget;
	edict_t 	*m_paraTarget;
	int m_iMyClass;
};

float CSqueakGrenade2::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS(monster_larve, CSqueakGrenade2);

int CSqueakGrenade2::IRelationship(CBaseEntity *pTarget)
{
	if (pTarget->pev->modelindex == pev->modelindex)
	{
		return R_NO;
	}

	return CBaseMonster ::IRelationship(pTarget);
}

//=========================================================
// ID's Hornet as their owner
//=========================================================
int CSqueakGrenade2::Classify()
{

	if (pev->owner && pev->owner->v.flags & FL_CLIENT)
	{
		return CLASS_PLAYER_BIOWEAPON;
	}

	return CLASS_ALIEN_BIOWEAPON;
}

void CSqueakGrenade2 ::Spawn(void)
{
	Precache();

	pev->classname = MAKE_STRING("monster_snark");
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_squeak.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, 0), Vector(4, 4, 8));
	UTIL_SetOrigin(pev, pev->origin);

	if (pev->owner)
		m_hOwner = Instance(pev->owner);

	m_flDie = gpGlobals->time + 8.0;
	m_flNextBounceSoundTime = gpGlobals->time; // reset each time a snark is spawned.
	

	pev->sequence = WSQUEAK_RUN;
	ResetSequenceInfo();

	pev->takedamage = DAMAGE_AIM;
	pev->health = 10.0;
	pev->gravity = 0.5;
	pev->friction = 0.6;
	pev->dmg = 40.0;
	pev->weapons = 1;

	m_pTarget = NULL;

	pev->flags |= FL_MONSTER;
	Classify2 = Classify();
	m_flFieldOfView = 0.4;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());			  // entity
	WRITE_SHORT(g_sModelIndexSmokeTrail); // model
	WRITE_BYTE(20);						  // life
	WRITE_BYTE(2.6);					  // width
	WRITE_BYTE(50);						  // r, g, b
	WRITE_BYTE(50);						  // r, g, b
	WRITE_BYTE(50);						  // r, g, b
	WRITE_BYTE(200);					  // brightness
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());			  // entity
	WRITE_SHORT(g_sModelIndexSmokeTrail); // model
	WRITE_BYTE(8);						  // life
	WRITE_BYTE(1.6);					  // width
	WRITE_BYTE(1);						  // r, g, b
	WRITE_BYTE(1);						  // r, g, b
	WRITE_BYTE(255);					  // r, g, b
	WRITE_BYTE(100);					  // brightness
	MESSAGE_END();

	StudioFrameAdvance();

	SetTouch(SuperBounceTouch);
	SetThink(HuntThink);
	pev->nextthink = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time + 0.1;
}

void CSqueakGrenade2::Precache(void)
{
}




	


void CSqueakGrenade2 ::Killed(entvars_t *pevAttacker, int iGib)
{
	pev->takedamage = DAMAGE_NO;
	m_pTarget = NULL;
	TraceResult tr = UTIL_GetGlobalTrace();
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "squeek/sqk_blast1.wav", 1, 0.5, 0, PITCH_NORM);
	CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, SMALL_EXPLOSION_VOLUME, 3.0);
	UTIL_BloodDrips(pev->origin, g_vecZero, BloodColor(), 80);

	
	// if (m_hOwner)
	// {
	// 	if (m_hOwner->infected == 1) // reset that for lag prevent
	// 	{
	// 		m_hOwner->infected = 0;
	// 		m_hOwner->pev->rendermode = kRenderNormal;
	// 		m_hOwner->pev->renderamt = 255;
	// 	}
	// }
	pev->health = -1;
	::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg * 2, pev->dmg * 2, CLASS_PLAYER_BIOWEAPON, DMG_SHOCK);
	
	// balls ice
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITETRAIL); // TE_RAILTRAIL);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x + tr.vecPlaneNormal.x * 3);
	WRITE_COORD(pev->origin.y + tr.vecPlaneNormal.y * 4);
	WRITE_COORD(pev->origin.z + tr.vecPlaneNormal.z * 5);
	WRITE_SHORT(g_sModelIndexSmoke); // model
	WRITE_BYTE(8);					 // count
	WRITE_BYTE(2);					 // life * 10
	WRITE_BYTE(2);					 // size * 10
	WRITE_BYTE(30);					 // amplitude * 0.1
	WRITE_BYTE(38);					 // speed * 100
	MESSAGE_END();

		//SetThink(SUB_Remove);
	SetThink(NULL);
	//SetTouch(NULL);
	//pev->nextthink = gpGlobals->time + 0.1;

	CBaseMonster ::Killed(pevAttacker, GIB_ALWAYS);
}

void CSqueakGrenade2 ::GibMonster(void)
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);
}

void CSqueakGrenade2::HuntThink(void)
{
	if (!IsInWorld())
	{
		SetTouch(NULL);
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();

	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize();
		Killed(pev, 0);
		return;
	}

	// explode delay
	if (pev->waterlevel != 0)
	{
		Killed(pev, 0);
		return;
	}
	m_flNextHunt = gpGlobals->time + 0.1;
	pev->nextthink = gpGlobals->time + 0.2;
	// return if not time to hunt
	if (m_flNextHunt > gpGlobals->time)
		return;

	// CBaseEntity *pOther = NULL;
	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize();

	UTIL_MakeVectors(pev->angles);

	if (m_pTarget == NULL || !m_pTarget->IsAlive())
	{
		Look(256);
		m_pTarget = BestVisibleEnemy();
	}

	if (m_pTarget != NULL)
	{
		if (FVisible(m_pTarget))
		{
			vecDir = m_pTarget->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize();
		}
		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);
		if (flAdj > 1.2)
			flAdj = 1.2;
		pev->velocity = pev->velocity * flAdj + m_vecTarget * 300;
	}

	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector(0, 0, 0);
		
	}
	else
	{
		if (pev->avelocity == Vector(0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT(-100, 100);
			pev->avelocity.z = RANDOM_FLOAT(-100, 100);
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT(-100, 100);
		pev->velocity.y = RANDOM_FLOAT(-100, 100);
	}

	m_posPrev = pev->origin;

	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->angles.z = 0;
	pev->angles.x = 0;

}

void CSqueakGrenade2::SuperBounceTouch(CBaseEntity *pOther)
{
	TraceResult tr = UTIL_GetGlobalTrace();

	// if (pev->owner && pOther->edict() == pev->owner)
	// 	return;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	m_flNextHit = gpGlobals->time + 0.15;
	m_flNextHunt = gpGlobals->time + 0.15;

	
	pev->dmg += 1.0;

	// if (pOther != NULL && pOther->infected == 1)
	// {
	// 	//::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg * 2.0, pev->dmg * 2.0, CLASS_PLAYER_BIOWEAPON, DMG_SHOCK);
	// 	pev->dmg += 100.0;
	// 	Killed(pev, 0);
	// 	m_flNextAttack = gpGlobals->time + 1.0;
	// 	return;
	// }

	::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, pev->dmg, CLASS_PLAYER_BIOWEAPON, DMG_SHOCK);
	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build1.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG(200, 255));

	if (m_flNextAttack < gpGlobals->time)
	{
		if (pOther != NULL && pOther->IsAlive()) 
		{
			if (pOther->edict() != pev->owner && pOther->pev->flags & (FL_MONSTER | FL_CLIENT) && pOther->Classify() != CLASS_MACHINE)
			{
				if (tr.pHit->v.modelindex != pev->modelindex)
				{
					if (pOther->infected == 0) // attach parasite
					{
						SetAttachment2(pOther->edict(), 0);
						UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
						pOther->infected = 1;
						pOther->pev->rendermode = kRenderTransAlpha;
						pOther->pev->renderamt = 64.0;
						EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "squeek/sqk_deploy1.wav", 1.0, ATTN_NORM, 0, 100);
						SetTouch(NULL);
						SetThink(ParasiteThink);
						m_flNextAttack = gpGlobals->time + 0.5;
						pev->nextthink = gpGlobals->time + 0.5;
						m_paraTarget = pOther->edict();
						
					}
					else 
					{
						pev->dmg += 7.0;
						Killed(pev, 0);
					}
					
				}
			}
		}
		m_flNextAttack = gpGlobals->time + 0.1;
	}

		

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());			  // entity
	WRITE_SHORT(g_sModelIndexSmokeTrail); // model
	WRITE_BYTE(30);						  // life
	WRITE_BYTE(1.6);					  // width
	WRITE_BYTE(16);						  // r, g, b
	WRITE_BYTE(64);						  // r, g, b
	WRITE_BYTE(RANDOM_LONG(1, 255));	  // r, g, b
	WRITE_BYTE(50);						  // brightness
	MESSAGE_END();						  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	// draw light
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x); // X
	WRITE_COORD(pev->origin.y); // Y
	WRITE_COORD(pev->origin.z); // Z
	WRITE_BYTE(12);				// radius * 0.1
	WRITE_BYTE(16);				// r
	WRITE_BYTE(0);				// g
	WRITE_BYTE(200);			// b
	WRITE_BYTE(1);				// time * 10
	WRITE_BYTE(0);				// decay * 0.1
	MESSAGE_END();

	// balls ice
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITETRAIL); // TE_RAILTRAIL);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x + tr.vecPlaneNormal.x * 3);
	WRITE_COORD(pev->origin.y + tr.vecPlaneNormal.y * 4);
	WRITE_COORD(pev->origin.z + tr.vecPlaneNormal.z * 5);
	WRITE_SHORT(g_sModelIndexSmoke); // model
	WRITE_BYTE(1);					 // count
	WRITE_BYTE(1);					 // life * 10
	WRITE_BYTE(1);					 // size * 10
	WRITE_BYTE(30);					 // amplitude * 0.1
	WRITE_BYTE(8);					 // speed * 100
	MESSAGE_END();

	// torus
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMTORUS);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z - pev->dmg*2); // reach damage radius over .2 seconds
	WRITE_SHORT(g_sModelIndexSmokeTrail);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(8);	 // life
	WRITE_BYTE(4);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(90); // r, g, b
	WRITE_BYTE(65); // r, g, b
	WRITE_BYTE(255); // r, g, b
	WRITE_BYTE(155);	 // brightness
	WRITE_BYTE(6);	 // speed
	MESSAGE_END();

	

	pev->velocity.x += RANDOM_FLOAT(-100, 100);
	pev->velocity.y += RANDOM_FLOAT(-100, 100);
	pev->velocity.z -= RANDOM_FLOAT(200, 400);


	
}

void CSqueakGrenade2::ParasiteThink(void)
{
	pev->nextthink = gpGlobals->time + 0.5;

	CBaseEntity *m_paraTargetE;
	if (m_paraTarget)
		m_paraTargetE = CBaseEntity::Instance(m_paraTarget);
	
	

	if (m_paraTargetE != NULL)
	{
		if (m_paraTargetE->infected == 0) 
		{
				m_paraTargetE->pev->rendermode = kRenderNormal;
				m_paraTargetE->pev->renderamt = 255.0;
				Killed(pev, 0);
		}
		else
		{
			if (m_paraTargetE->IsAlive())
			{
				m_paraTargetE->TakeDamage(pev, VARS(pev->owner), 3.0, DMG_BLAST);
			}
			else
			{
				
				Killed(pev, 0);
			}
		}
	}
	else
		Killed(pev, 0);

	
	
}

////////////////////////////
////////////////////////////

void Dcrystal::Spawn(void) // attack crysto
{
	SET_MODEL(ENT(pev), "models/crystal2.mdl");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-5, -5, -4), Vector(5, 5, 12));
	UTIL_SetOrigin(pev, pev->origin);
	pev->gravity = 0.0;
	m_flDie = 60;
	pev->dmg = 10.0;

	pev->avelocity.y = -250;

	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 200;

	// pev->classname = MAKE_STRING( "weapon_satchel" );

	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);
	m_LaserSprite = PRECACHE_MODEL("sprites/laserbeam.spr");

	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 4.0;
}

void Dcrystal::MoveThink()
{
	if (pev->rendermode != kRenderNormal)
	{
		pev->rendermode = kRenderNormal;
		pev->movetype = MOVETYPE_TOSS;
	}
	CBaseEntity *pEntity2 = CBaseEntity::Instance(pev->owner);
	CBasePlayer *pPlayer = (CBasePlayer *)pEntity2;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMCYLINDER);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 550); // reach damage radius over .2 seconds
	WRITE_SHORT(g_sModelIndexSmokeTrail);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(4);	 // life
	WRITE_BYTE(50);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(100); // r, g, b
	WRITE_BYTE(50);	 // r, g, b
	WRITE_BYTE(10);	 // r, g, b
	WRITE_BYTE(175); // brightness
	WRITE_BYTE(0);	 // speed
	MESSAGE_END();

	// make team color
	int index = g_pGameRules->GetTeamIndex(pPlayer->TeamID());
	if (g_pGameRules->IsTeamplay() && index == 0) // Blue
	{
		// pev->rendermode = kRenderNormal;
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor.x = 0;	  // red
		pev->rendercolor.y = 0;	  // green
		pev->rendercolor.z = 100; // blue
		pev->renderamt = 24;
	}
	if (g_pGameRules->IsTeamplay() && index == 1) // Red
	{
		// pev->rendermode = kRenderNormal;
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor.x = 100; // red
		pev->rendercolor.y = 0;	  // green
		pev->rendercolor.z = 0;	  // blue
		pev->renderamt = 24;
	}

	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/crystal_heal.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG(200, 240));

	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 286)) != NULL)
	{
		if ((FVisible(pEntity)))
		{
			CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
			if ((g_pGameRules->PlayerRelationship(pOwner, pEntity) != GR_TEAMMATE))
			{
				if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage > 0 && (pEntity->edict() != edict()))
				{
					// shock ray 1
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_BEAMPOINTS);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z + 85);
					WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(m_LaserSprite); // sprite
					WRITE_BYTE(1);				// Starting frame
					WRITE_BYTE(0);				// framerate * 0.1
					WRITE_BYTE(4);				// life * 0.1
					WRITE_BYTE(22);				// width
					WRITE_BYTE(0);				// noise
					WRITE_BYTE(100);			// color r,g,b
					WRITE_BYTE(10);				// color r,g,b
					WRITE_BYTE(15);				// color r,g,b
					WRITE_BYTE(250);			// brightness
					WRITE_BYTE(64);				// scroll speed
					MESSAGE_END();

					// shock ray 2
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_BEAMPOINTS);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z + 85);
					WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(m_LaserSprite); // sprite
					WRITE_BYTE(1);				// Starting frame
					WRITE_BYTE(0);				// framerate * 0.1
					WRITE_BYTE(5);				// life * 0.1
					WRITE_BYTE(12);				// width
					WRITE_BYTE(0);				// noise
					WRITE_BYTE(100);			// color r,g,b
					WRITE_BYTE(100);			// color r,g,b
					WRITE_BYTE(15);				// color r,g,b
					WRITE_BYTE(200);			// brightness
					WRITE_BYTE(128);			// scroll speed
					MESSAGE_END();

					// lights
					Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
					MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
					WRITE_BYTE(TE_DLIGHT);
					WRITE_COORD(pEntity->pev->origin.x); // X
					WRITE_COORD(pEntity->pev->origin.y); // Y
					WRITE_COORD(pEntity->pev->origin.z); // Z
					WRITE_BYTE(16);						 // radius * 0.1
					WRITE_BYTE(200);					 // r
					WRITE_BYTE(30);						 // g
					WRITE_BYTE(30);						 // b
					WRITE_BYTE(128);					 // time * 10
					WRITE_BYTE(32);						 // decay * 0.1
					MESSAGE_END();

					UTIL_ScreenShake(pEntity->pev->origin, pev->dmg, 80.0, 0.3, 1);

					pEntity->TakeDamage(pev, VARS(pev->owner), pev->dmg, DMG_SHOCK);
					pEntity->pev->max_health *= 0.9;
					pEntity->pev->velocity.z += pEntity->pev->velocity.z + 256;
					// sounds
					switch (RANDOM_LONG(0, 8))
					{
					case 0:
						EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build1.wav", 0.95, ATTN_NORM, 0, RANDOM_LONG(140, 180));
						break;
					case 3:
						EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build2.wav", 0.95, ATTN_NORM, 0, RANDOM_LONG(140, 180));
						break;
					case 5:
						EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build3.wav", 0.95, ATTN_NORM, 0, RANDOM_LONG(140, 180));
						break;
					case 8:
						EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build4.wav", 0.90, ATTN_NORM, 0, RANDOM_LONG(140, 180));
						break;
					}
				}
			}
		}
	}

	// EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/ar2_altfire1.wav", 1.0, ATTN_NORM);
	pev->nextthink = gpGlobals->time + 1.0;
	m_flDie -= 1;

	if (m_flDie <= 0)
	{
		UTIL_Remove(this);
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, pev->dmg, CLASS_NONE, DMG_SHOCK);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); // play sound
		// random explosions
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexFireball);
		WRITE_BYTE(75); // scale * 10
		WRITE_BYTE(10); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();
	}
}

////////////////////////////
////////////////////////////

void Fcrystal::Spawn(void) // god on one hit
{
	SET_MODEL(ENT(pev), "models/crystal4.mdl");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-6, -6, -6), Vector(6, 6, 6));
	UTIL_SetOrigin(pev, pev->origin);
	pev->gravity = 0.0;
	m_flDie = 60;

	pev->avelocity.y = -250;

	pev->rendermode = kRenderTransAdd; // kRenderTransAlpha
	pev->renderamt = 200;

	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);

	m_LaserSprite = PRECACHE_MODEL("sprites/xbeam4.spr");

	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 2.0;
}

void Fcrystal::MoveThink()
{
	if (pev->rendermode != kRenderNormal)
	{
		pev->rendermode = kRenderNormal;
		pev->movetype = MOVETYPE_TOSS;
	}
	CBaseEntity *pEntity2 = CBaseEntity::Instance(pev->owner);
	CBasePlayer *pPlayer = (CBasePlayer *)pEntity2;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMCYLINDER);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 550); // reach damage radius over .2 seconds
	WRITE_SHORT(g_sModelIndexSmokeTrail);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(4);	 // life
	WRITE_BYTE(50);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(100); // r, g, b
	WRITE_BYTE(50);	 // r, g, b
	WRITE_BYTE(10);	 // r, g, b
	WRITE_BYTE(175); // brightness
	WRITE_BYTE(0);	 // speed
	MESSAGE_END();

	// pev->renderfx = kRenderTransAdd;
	// pev->rendercolor.x = 200;  // red
	// pev->rendercolor.y = 200;  // green
	// pev->rendercolor.z = 200; // blue
	// pev->renderamt = 200;

	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/snd3.wav", 0.5, ATTN_NORM, 0, 100);
	EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "zxc/electrofreq.wav", 0.5, ATTN_NORM, 0, 100);

	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 286)) != NULL)
	{
		if ((FVisible(pEntity)))
		{
			CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
			if ((g_pGameRules->PlayerRelationship(pOwner, pEntity) != GR_TEAMMATE))
			{
				if (pEntity->IsPlayer() && (pEntity->edict() != edict()))
				{
					// shock ray 1
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_BEAMPOINTS);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z + 85);
					WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(m_LaserSprite); // sprite
					WRITE_BYTE(1);				// Starting frame
					WRITE_BYTE(0);				// framerate * 0.1
					WRITE_BYTE(5);				// life * 0.1
					WRITE_BYTE(22);				// width
					WRITE_BYTE(0);				// noise
					WRITE_BYTE(100);			// color r,g,b
					WRITE_BYTE(100);			// color r,g,b
					WRITE_BYTE(150);			// color r,g,b
					WRITE_BYTE(240);			// brightness
					WRITE_BYTE(64);				// scroll speed
					MESSAGE_END();

					UTIL_ScreenShake(pEntity->pev->origin, pev->dmg, 90.5, 0.3, 1);

					pEntity->Shield = 2; // enable shield

					pEntity->pev->renderfx = kRenderFxGlowShell;
					pEntity->pev->rendercolor.x = 200; // red
					pEntity->pev->rendercolor.y = 200; // green
					pEntity->pev->rendercolor.z = 200; // blue
					pEntity->pev->renderamt = 200;

					// pEntity->TakeDamage(pev, VARS( pev->owner ), pev->dmg, DMG_SHOCK);
					// pEntity->pev->armorvalue -= 1;
					// pEntity->pev->velocity.z += pEntity->pev->velocity.z + 256;
				}
			}
		}
	}

	// disk
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMDISK);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 512); // reach damage radius over .2 seconds
	WRITE_SHORT(g_sModelIndexBubbles);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(4);	 // life
	WRITE_BYTE(12);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(150); // r, g, b
	WRITE_BYTE(150); // r, g, b
	WRITE_BYTE(150); // r, g, b
	WRITE_BYTE(128); // brightness
	WRITE_BYTE(2);	 // speed
	MESSAGE_END();

	// lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(pev->origin.x); // X
	WRITE_COORD(pev->origin.y); // Y
	WRITE_COORD(pev->origin.z); // Z
	WRITE_BYTE(32);				// radius * 0.1
	WRITE_BYTE(240);			// r
	WRITE_BYTE(240);			// g
	WRITE_BYTE(240);			// b
	WRITE_BYTE(128);			// time * 10
	WRITE_BYTE(32);				// decay * 0.1
	MESSAGE_END();

	// EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/ar2_altfire1.wav", 1.0, ATTN_NORM);
	pev->nextthink = gpGlobals->time + 2.6;
	m_flDie -= 1;

	if (m_flDie <= 0)
	{
		UTIL_Remove(this);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); // play sound
		// random explosions
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexFireball);
		WRITE_BYTE(75); // scale * 10
		WRITE_BYTE(10); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();
	}
}
