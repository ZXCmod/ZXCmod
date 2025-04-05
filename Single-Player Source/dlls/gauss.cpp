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
#include "tripmine.h"

#define GAUSS_PRIMARY_CHARGE_VOLUME 256 // how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME 450	// how loud gauss is when discharged
#define BLASTER_BEAM_RANDOMNESS RANDOM_LONG(1, 24)

// nuke class
class CGaussNuke : public CGrenade
{
public:
	void Spawn();
	void EXPORT MoveThink();
	void EXPORT Hit(CBaseEntity *);
	void Explode(int);
	static CGaussNuke *Create(Vector, Vector, CBaseEntity *);
	CBasePlayer *pPlayer;

private:
	int BeamSprite;
	float m_flDie;
	float m_flDie2;
	float m_flDie3;
	float dmge;
	int rad;
	int m_iSpriteTexture2;
	int m_sp;
};

// Radiation by nuke
class CRadiation : public CBaseEntity
{
public:
	void Spawn();
	void EXPORT MoveThink();
	void Explode();
	int m_expl;

private:
	int m_flDie10;
	int m_iSpriteTexture2;
	int m_iSpriteTexture22;
	int m_iBodyGibs;
};

// Gauss cannon 1.31 [3rd]
class CGCannon : public CBaseEntity
{
public:
	void Spawn(void);
	void EXPORT MoveThink(void);
	void EXPORT MoveTouch(CBaseEntity *pOther);

private:
	short m_Sprite;
	short m_SpriteExp;
	short m_iSpriteTexture;
	float m_timer;
	int m_iBalls;
};

// Gauss particle 1.31 [2nd]
class CGParticle : public CBaseEntity
{

public:
	void Spawn(void);
	void EXPORT MoveThink(void);

private:
	short m_Sprite;
};

// Gauss particle 2 1.31 [2nd]
class CGParticle2 : public CBaseEntity
{

public:
	void Spawn(void);

private:
	short m_Sprite;
};

LINK_ENTITY_TO_CLASS(item_nvg, CGParticle2);
LINK_ENTITY_TO_CLASS(env_beamtrail, CGParticle);
LINK_ENTITY_TO_CLASS(weapon_gauss, CGauss);
LINK_ENTITY_TO_CLASS(virtual_hull, CGCannon);
LINK_ENTITY_TO_CLASS(trigger_killmonster, CRadiation);

enum gauss_e
{
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW,
	GAUSS_RELOAD
};

float CGauss::GetFullChargeTime(void)
{

	return 1.5;
}

#ifdef CLIENT_DLL
extern int g_irunninggausspred;
#endif

void CGauss::Spawn()
{
	Precache();
	m_iId = WEAPON_GAUSS;
	SET_MODEL(ENT(pev), "models/w_gauss.mdl");
	m_iDefaultAmmo = GAUSS_DEFAULT_GIVE;
	FallInit();
}

void CGauss::Precache(void)
{

	PRECACHE_MODEL("models/w_gauss.mdl");
	PRECACHE_MODEL("models/v_gauss.mdl");
	PRECACHE_MODEL("models/p_gauss.mdl");
	PRECACHE_MODEL("models/nuke.mdl");
	PRECACHE_MODEL("models/nukeT.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("buttons/bell1.wav");

	PRECACHE_SOUND("weapons/gauss2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("ambience/pulsemachine.wav");
	PRECACHE_SOUND("zxc/hold_loop.wav");

	PRECACHE_SOUND("zxc/superphys_launch2.wav");
	PRECACHE_SOUND("zxc/superphys_launch3.wav");
	PRECACHE_SOUND("zxc/superphys_launch4.wav");
	PRECACHE_SOUND("zxc/energy_sing_explosion2.wav");
	PRECACHE_SOUND("zxc/ar2_altfire1.wav");
	PRECACHE_SOUND("zxc/nldm.wav");

	m_iGlow = PRECACHE_MODEL("sprites/hotglow.spr");
	m_iBalls = PRECACHE_MODEL("sprites/hotglow.spr");
	m_iBeam = PRECACHE_MODEL("sprites/smoke.spr");

	m_usGaussFire = PRECACHE_EVENT(1, "events/gauss.sc");
	m_usGaussSpin = PRECACHE_EVENT(1, "events/gaussspin.sc");
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");
	PRECACHE_MODEL("sprites/shrinkf.spr");
	PRECACHE_MODEL("sprites/zbeam1.spr");
	m_iSpriteTexture2 = PRECACHE_MODEL("sprites/cexplo.spr");

	PRECACHE_MODEL("sprites/nhth1.spr");
	PRECACHE_MODEL("sprites/yelflare1.spr");
	PRECACHE_MODEL("sprites/richo2.spr");
	PRECACHE_MODEL("sprites/zbeam4.spr");
	PRECACHE_MODEL("sprites/cexplo.spr");
	PRECACHE_MODEL("models/metalplategibs.mdl");
}

int CGauss::AddToPlayer(CBasePlayer *pPlayer)
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev);
		WRITE_BYTE(m_iId);
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

int CGauss::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_GAUSS;
	p->iFlags = 0;
	p->iWeight = GAUSS_WEIGHT;
	p->iId = m_iId = WEAPON_GAUSS;
	return 1;
}

BOOL CGauss::Deploy()
{
	m_pPlayer->m_flPlayAftershock = 0.0;
	return DefaultDeploy("models/v_gauss.mdl", "models/p_gauss.mdl", GAUSS_DRAW, "gauss");
}

void CGauss::Holster(int skiplocal /* = 0 */)
{
	PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1);

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(GAUSS_HOLSTER);
	m_fInAttack = 0;
}

void CGauss::PrimaryAttack()
{

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		PlayEmptySound();
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.25;
		return;
	}

	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
	m_fPrimaryFire = TRUE;
	m_fFourthFire = FALSE;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 2;
	m_fInAttack = 0;
	StartFireBull();

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.25;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.25;
}

void CGauss::StartFireBull(void)
{
	float flDamage;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;

	if (gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime())
		flDamage = 150;
	else
		flDamage = 150 * ((gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime());

	if (m_fPrimaryFire && !m_fFourthFire)
		flDamage = 24;

	CBaseEntity *GSquidSpit = CBaseEntity::Create("squidspit", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, vecAiming, m_pPlayer->edict());

	GSquidSpit->pev->velocity = vecAiming * 1600;

	SendWeaponAnim(6);
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecSrc.x); // X
	WRITE_COORD(vecSrc.y); // Y
	WRITE_COORD(vecSrc.z); // Z
	WRITE_BYTE(12);		   // radius * 0.1
	WRITE_BYTE(80);		   // r
	WRITE_BYTE(200);	   // g
	WRITE_BYTE(80);		   // b
	WRITE_BYTE(1);		   // time * 10
	WRITE_BYTE(1);		   // decay * 0.1
	MESSAGE_END();

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "weapons/electro4.wav", 0.2, 0.8);
}

void CGauss::SecondaryAttack()
{
	float m_b, m_t;

	m_b = 900;
	m_t = 27;

	//*
	if (m_fInAttack == 0)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}
		// fire
		m_fPrimaryFire = FALSE;
		m_fFourthFire = FALSE;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--; // take one ammo just to start the spin
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		// spin up
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;

		SendWeaponAnim(GAUSS_SPINUP);
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

		PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0);

		m_iSoundState = SND_CHANGE_PITCH;
	}

	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim(GAUSS_SPIN);
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if (UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != m_b)
		{

			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.075;
			}
		}

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
			return;
		}

		if (UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge)
		{
			// don't eat any more ammo after gun is fully charged.
			m_pPlayer->m_flNextAmmoBurn = m_b;
		}

		int pitch = (gpGlobals->time - m_pPlayer->m_flStartCharge) * (150 / GetFullChargeTime()) + 100;
		if (pitch > 270)
			pitch = 270;

		PLAYBACK_EVENT_FULL(FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, (m_iSoundState == SND_CHANGE_PITCH) ? 1 : 0, 0);
		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
	}
}

//// Tp ray ////

void CGauss::ThirdAttack(void)
{

	TraceResult tr;
	Vector vecSrc;
	BOOL hasTripmine = FALSE;
	vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = gpGlobals->v_forward;
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8000, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.25;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	if (pEntity && pEntity->pev->takedamage > 0 && pEntity->pev->movetype != MOVETYPE_NONE && pEntity->pev->movetype != MOVETYPE_FLY)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 0.55, ATTN_NORM, 0, RANDOM_LONG(130, 160));

		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
		WRITE_COORD(pEntity->pev->origin.y);
		WRITE_COORD(pEntity->pev->origin.z);
		WRITE_SHORT(BSpr); // sprite
		WRITE_BYTE(0);	   // Starting frame
		WRITE_BYTE(0);	   // framerate * 0.1
		WRITE_BYTE(3);	   // life * 0.1
		WRITE_BYTE(12);	   // width
		WRITE_BYTE(15);	   // noise
		WRITE_BYTE(125);   // color r,g,b
		WRITE_BYTE(65);	   // color r,g,b
		WRITE_BYTE(10);	   // color r,g,b
		WRITE_BYTE(175);   // brightness
		WRITE_BYTE(100);   // scroll speed
		MESSAGE_END();

		CTripmineGrenade *pGaussTeleportTripmine = NULL;
		while ((pGaussTeleportTripmine = (CTripmineGrenade *)UTIL_FindEntityByClassname(pGaussTeleportTripmine, "monster_tripmine")) != NULL)
		{
			if ((pGaussTeleportTripmine->m_pRealOwner) == m_pPlayer->edict())
			{
				if (pGaussTeleportTripmine->m_TelevecEnd != g_vecZero)
				{
					// vis effect st
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_TELEPORT);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					MESSAGE_END();

					if (pEntity->IsPlayer())
						UTIL_SetOrigin(pEntity->pev, pGaussTeleportTripmine->m_TelevecEnd + Vector(0, 0, 40));
					else
					{
						UTIL_SetOrigin(pEntity->pev, pGaussTeleportTripmine->m_TelevecEnd);
						pEntity->pev->flags &= ~FL_ONGROUND;
					}

					// vis effect en
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_TELEPORT);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					MESSAGE_END();

					EMIT_SOUND(ENT(pEntity->pev), CHAN_BODY, "debris/beamstart7.wav", 1.0, ATTN_NORM); // play sound 1st

					hasTripmine = TRUE;
					if (RANDOM_LONG(0, 2) == 1)
						break;
				}
			}
		}

		if (hasTripmine == FALSE)
		{
			if (m_pPlayer->pev->health < m_pPlayer->pev->max_health)
			{
				pEntity->TakeDamage(pev, VARS(pev->owner), 3, DMG_FALL);
				m_pPlayer->TakeDamage(pev, VARS(pev->owner), -3, DMG_FALL);
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.05;
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.05;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.05;
			}
			else
			{
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/energy_sing_explosion2.wav", 0.8, ATTN_NORM);
				UTIL_Sparks(vecSrc + vecDir * 24);
				m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			}
		}
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		UTIL_Sparks(vecSrc + vecDir * 24);
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM, 0, 80);
	}

	// selftest
	// else
	// {
	// 	CBaseEntity *pGaussTeleportEnt = Create("GaussTeleportEnt", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict());
	// 	pGaussTeleportEnt->pev->velocity = gpGlobals->v_forward * 400;
	// 	m_pPlayer->m_HasTeleportPoint = TRUE;
	// }
}

void CGauss::FourthAttack()
{
	if (m_fInAttack == 0)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_fPrimaryFire = FALSE;
		m_fFourthFire = TRUE;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		SendWeaponAnim(GAUSS_SPINUP);
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();
		m_iSoundState = SND_CHANGE_PITCH;
	}

	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim(GAUSS_SPIN);
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if (UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 900)
		{

			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
			}
		}

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
			return;
		}

		if (UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge)
		{
			m_pPlayer->m_flNextAmmoBurn = 900;
		}

		int pitch = (gpGlobals->time - m_pPlayer->m_flStartCharge) * (150 / GetFullChargeTime()) + 100;
		if (pitch > 270)
			pitch = 270;

		PLAYBACK_EVENT_FULL(FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, (m_iSoundState == SND_CHANGE_PITCH) ? 1 : 0, 0);
		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;

		if (m_pPlayer->m_flStartCharge < gpGlobals->time - 5)
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0, 0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0, 0x3f));

			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

			UTIL_ScreenFade(m_pPlayer, Vector(255, 0, 0), 4, 8.5, 160, FFADE_IN);

			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION); // This just makes a dynamic light now
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(m_iSpriteTexture2); // other sprite
			WRITE_BYTE(80);					// scale * 10
			WRITE_BYTE(12);					// framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();
			::RadiusDamage(pev->origin, pev, VARS(pev->owner), 120, 640, CLASS_NONE, DMG_BLAST); // DMG

			SendWeaponAnim(GAUSS_IDLE);

			// Player may have been killed and this weapon dropped, don't exeGaussTeleportPointte any more code after this!
			return;
		}
	}
}

// now unused
BOOL CGauss::Lock()
{
	return TRUE;
}

//=========================================================
// StartFire- since all of this code has to run and then
// call Fire(), it was easier at this point to rip it out
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names
//=========================================================
void CGauss::StartFire(void)
{
	float flDamage;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition();

	if (gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime())
		flDamage = 150.0;
	else
		flDamage = 150.0 * ((gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime());

	if (m_fPrimaryFire && !m_fFourthFire)
		flDamage = 24.0;

	if (m_fInAttack > 0)
	{

		float flZVel = m_pPlayer->pev->velocity.z;
		//m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * (flDamage) * 5;
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * (flDamage + 50) * 5;
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	}

	Fire(vecSrc, vecAiming, flDamage);
}

void CGauss::Fire(Vector vecOrigSrc, Vector vecDir, float flDamage)
{
	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;

	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;
	edict_t		*pentIgnore;
	TraceResult tr, beam_tr;
	float flMaxFrac = 1.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 24;
	
	short m_Sprite2;
	m_Sprite2 = PRECACHE_MODEL( "sprites/muz1.spr" );

	pentIgnore = ENT( m_pPlayer->pev );

	#ifdef CLIENT_DLL
		if ( m_fPrimaryFire == false )
			 g_irunninggausspred = true;
	#endif
	
	if (m_fFourthFire == FALSE) //the laser visual line, no other effects, disable when 4th attack
		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, flDamage, 0.0, 0, 0, m_fPrimaryFire ? 1 : 0, 0 );

	PLAYBACK_EVENT_FULL( FEV_GLOBAL | FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );

	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;
		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);

		if (tr.fAllSolid)
			break;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (pEntity == NULL)
			break;

		if ( fFirstBeam )
		{
			m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
			fFirstBeam = 0;
			nTotal += 16;
		}
		
		if (pEntity->pev->takedamage && m_fFourthFire == FALSE)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecDir, &tr, DMG_ENERGYBEAM );
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}

		if ( pEntity->ReflectGauss() && m_fFourthFire == FALSE) //normal 2nd attack
		{ //* delete here to disable true physical laser
			float n;

			pentIgnore = m_pPlayer->edict();

			n = -DotProduct(tr.vecPlaneNormal, vecDir);

			if (n < 0.5) // 60 degrees
			{
				// reflect
				Vector r;
			
				r = 2.0 * tr.vecPlaneNormal * n + vecDir;
				flMaxFrac = flMaxFrac - tr.flFraction;
				vecDir = r;
				vecSrc = tr.vecEndPos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;
				
				nTotal += 1;
				
				// lose energy
				if (n == 0) n = 0.1;
				
				// explode a bit
				m_pPlayer->RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, flDamage * n, CLASS_NONE, DMG_ENERGYBEAM );

/* 			
		
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMENTPOINT );
					WRITE_SHORT( m_pPlayer->entindex() | 0x1000  );
					WRITE_COORD( tr.vecEndPos.x );
					WRITE_COORD( tr.vecEndPos.y );
					WRITE_COORD( tr.vecEndPos.z );
					WRITE_SHORT( g_sModelIndexLaser );
					WRITE_BYTE( 1 ); // frame start
					WRITE_BYTE( 10 ); // framerate
					WRITE_BYTE( 3 ); // life
					WRITE_BYTE( 10 );  // width
					WRITE_BYTE( 1 );   // noise
					WRITE_BYTE( 64 );   // r, g, b
					WRITE_BYTE( 196 );   // r, g, b
					WRITE_BYTE( 255);   // r, g, b
					WRITE_BYTE( 255 );	// brightness
					WRITE_BYTE( 10 );		// speed
				MESSAGE_END();
				

				
*/
				// animated  sprite
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITE );
					WRITE_COORD( tr.vecEndPos.x );
					WRITE_COORD( tr.vecEndPos.y );
					WRITE_COORD( tr.vecEndPos.z );
					WRITE_SHORT( m_Sprite2 );
					WRITE_BYTE( pev->dmg ); // scale
					WRITE_BYTE( 128 ); // brightness
				MESSAGE_END();
				
				// UTIL_DecalTrace( &tr, 3 );
				
			}
			else
			{
				nTotal += 1;

				// limit it to one hole punch
				if (fHasPunched)
					break;
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( m_fFourthFire == FALSE ) //if second attack    m_fPrimaryFire == FALSE && 
				{
					//m_pPlayer->pev->punchangle.x = -(flDamage/7); //hardest aim, stop easy fraging
					UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, pentIgnore, &beam_tr);
					
					// animated  sprite
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_SPRITE );
						WRITE_COORD( tr.vecEndPos.x );
						WRITE_COORD( tr.vecEndPos.y );
						WRITE_COORD( tr.vecEndPos.z );
						WRITE_SHORT( m_Sprite2 );
						WRITE_BYTE( pev->dmg ); // scale
						WRITE_BYTE( 128 ); // brightness
					MESSAGE_END();
					
					MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
						WRITE_BYTE( TE_IMPLOSION );
						WRITE_COORD( beam_tr.vecEndPos.x );
						WRITE_COORD( beam_tr.vecEndPos.y );
						WRITE_COORD( beam_tr.vecEndPos.z );
						WRITE_BYTE( 64 );	// rad
						WRITE_BYTE( 16 );		// count
						WRITE_BYTE( 12 );		// life
					MESSAGE_END();


					if (!beam_tr.fAllSolid)
					{
						// trace backwards to find exit point
						UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, pentIgnore, &beam_tr);

						float n = (beam_tr.vecEndPos - tr.vecEndPos).Length( );
						
						if (n < flDamage)
						{
							if (n == 0) n = 1;
							flDamage -= n;
							nTotal += 1;
							float damage_radius;
							damage_radius = flDamage * 3.6;  // Old code == 2.5, rebalanced in 1.35
							::RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage*0.35, damage_radius, CLASS_NONE, DMG_BLAST );
							CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );
							vecSrc = beam_tr.vecEndPos + vecDir;
						}
						
					}
					else
						flDamage = 0;
				}
				else
					flDamage = 0;
				return;
			}
		
		} //*
		else if (m_fFourthFire == TRUE)
		{
			Vector vecThrow = gpGlobals->v_forward * 800;
			Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
			UTIL_MakeVectors(anglesAim);

			CBaseEntity *pGauss = Create("virtual_hull", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict());
			pGauss->pev->velocity = vecThrow;
			pGauss->pev->scale = (flDamage * 0.02);

			// if (!FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
				pGauss->pev->dmg = (flDamage / 1.25); // when full charge
			// else
			// 	pGauss->pev->dmg = (flDamage * 0.86); // higher dmg

			SendWeaponAnim(5);
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
			pentIgnore = ENT(pEntity->pev);
			return;
		}
		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT(pEntity->pev);
		}
	}
	return;
}

void CGauss::WeaponIdle(void)
{
	if (m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 200 && (m_pPlayer->m_flNextNukeTime < gpGlobals->time))
	{
		Reload();
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;

		switch (RANDOM_LONG(0, 255))
		{
		case 0:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "bullchicken/bc_idle1.wav", 0.8, 0.8);
			break;
		case 1:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "bullchicken/bc_idle2.wav", 0.8, 0.8);
			break;
		case 2:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "bullchicken/bc_idle3.wav", 0.8, 0.8);
			break;
		case 3:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "bullchicken/bc_idle4.wav", 0.8, 0.8);
			break;
		case 4:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "bullchicken/bc_idle5.wav", 0.8, 0.8);
			break;
		}
	}

	char szText[10];
	hudtextparms_t hText;
	sprintf(szText, "Nuke: %d", (max(0, m_pPlayer->m_flNextNukeTime - INT(gpGlobals->time))));
	hText.channel = 2;
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
	if (m_pPlayer->pevEntity != NULL)
		UTIL_HudMessage(m_pPlayer->pevEntity, hText, szText);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}

void CGauss ::Reload(void)
{

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector GunPosition = m_pPlayer->GetGunPosition();
	GunPosition = GunPosition + gpGlobals->v_forward * 0;
	GunPosition = GunPosition + gpGlobals->v_right * 7;
	GunPosition = GunPosition + gpGlobals->v_up * 0;

	CGaussNuke ::Create(GunPosition,
										   m_pPlayer->pev->v_angle,
										   m_pPlayer);

	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 200;
	m_pPlayer->m_flNextNukeTime = gpGlobals->time + 240;
	// dont silent spawn
	UTIL_ShowMessageAll("nuclear missile launched!"); // STRING(m_pPlayer->pev->netname)
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "buttons/bell1.wav", 1, ATTN_NORM);

	int i;
	// loop through all players
	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer)
			EMIT_SOUND(ENT(pPlayer->pev), CHAN_STATIC, "zxc/nldm.wav", 1, ATTN_NORM);
	}
}

class CGaussAmmo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_gaussammo.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_gaussammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		if (pOther->GiveAmmo(AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(ammo_gaussclip, CGaussAmmo);

/**********************************

Nuke bomb

**********************************/

void CGaussNuke ::Spawn()
{
	m_iSpriteTexture2 = PRECACHE_MODEL("sprites/shockwave.spr");
	BeamSprite = PRECACHE_MODEL("sprites/smoke.spr");

	SET_MODEL(ENT(pev), "models/nuke.mdl");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4));
	UTIL_SetOrigin(pev, pev->origin);
	pev->classname = MAKE_STRING("nuke");
	SetThink(MoveThink);
	SetTouch(Hit);
	pev->velocity = gpGlobals->v_forward * 100;
	pev->gravity = 0.65;
	pev->friction = 0.015;
	pev->takedamage = 0;
	pev->health = 10000.0;

	if (g_zxc_promode.value == 0)
		pev->dmg = 1200.0;
	else
		pev->dmg = 600.0;

	m_flDie = gpGlobals->time + 30.0;
	pev->avelocity.z = 128;

	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 0.5;
}

void CGaussNuke ::Hit(CBaseEntity *Target)
{
	pev->angles = UTIL_VecToAngles(pev->velocity);
	TraceResult TResult;
	Vector StartPosition;
	
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine(StartPosition,
				   StartPosition + pev->velocity.Normalize() * 64,
				   dont_ignore_monsters,
				   ENT(pev),
				   &TResult);

	Explode(DMG_BLAST); // direct damage, moved to radiation spawn
}

void CGaussNuke ::Explode(int DamageType)
{
	// if (TResult->fAllSolid)
	// {
	// 	UTIL_Remove(this);
	// 	return;
	// }

	CBaseEntity *pEntity = NULL;
	Vector vecDir;
	vecDir = Vector(0, 0, 0);

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 2500.0)) != NULL) // 1600
	{
		if (pEntity->pev->takedamage > 0) 
		{
			//pEntity->TakeDamage(pev, VARS(pev->owner), pev->dmg, DMG_BLAST);



			if (pEntity->IsPlayer())
			{
				UTIL_ScreenFade(pEntity, Vector(200, 30, 0), 300, 30, 30, FFADE_IN);
				UTIL_ScreenShake(pEntity->pev->origin, 1024.0, 90.5, 154.7, 1);
				vecDir = (pEntity->Center() - Vector(0, 0, 10) - Center()).Normalize(); /// NOW WORKED! CONGRATULATIONS!
				pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -2048;
				pEntity->pev->punchangle.x = 10;
				pEntity->pev->punchangle.y = RANDOM_LONG(-74, 74);
				pEntity->pev->punchangle.z = -20;
			}
		}
	}

	::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, 2500.0, CLASS_NONE, DMG_BLAST);

	CBaseEntity::Create("trigger_killmonster", pev->origin, pev->angles, pev->owner);
	UTIL_Remove(this);
}

CGaussNuke *CGaussNuke ::Create(Vector Pos, Vector Aim, CBaseEntity *Owner)
{
	CGaussNuke *Beam = GetClassPtr((CGaussNuke *)NULL);
	UTIL_SetOrigin(Beam->pev, Pos);
	Beam->Spawn();
	Beam->SetTouch(CGaussNuke ::Hit);
	Beam->pev->owner = Owner->edict();
	Beam->pev->angles = UTIL_VecToAngles(Beam->pev->velocity);
	return Beam;
}

void CGaussNuke ::MoveThink()
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMCYLINDER);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 1600); // reach damage radius over .2 seconds
	WRITE_SHORT(m_iSpriteTexture2);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(40);	 // life
	WRITE_BYTE(4);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(128); // r, g, b
	WRITE_BYTE(128); // r, g, b
	WRITE_BYTE(192); // r, g, b
	WRITE_BYTE(128); // brightness
	WRITE_BYTE(0);	 // speed
	MESSAGE_END();

	if (gpGlobals->time >= m_flDie)
	{
		Explode(DMG_BLAST);
	}

	//pev->dmg += 1.5;
	pev->nextthink = gpGlobals->time + 1.0;
}

//////////////radiation point

void CRadiation ::Spawn()
{
	SET_MODEL(ENT(pev), "models/rpgrocket.mdl");
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 0;
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin(pev, pev->origin);
	pev->classname = MAKE_STRING("Nuke_Radiation");
	m_flDie10 = gpGlobals->time + 30.0; // timer
	pev->dmg = 5.0;
	pev->nextthink = gpGlobals->time + 0.1; // 10 times a second
	SetThink(MoveThink);
	m_iSpriteTexture2 = PRECACHE_MODEL("sprites/shockwave.spr");
	m_iSpriteTexture22 = PRECACHE_MODEL("sprites/cexplo.spr");
	m_iBodyGibs = PRECACHE_MODEL("models/metalplategibs.mdl");
	pev->gravity = 0;
	pev->friction = 0;

	TraceResult TResult;
	Vector StartPosition;
	StartPosition = pev->origin;

	// break metals
	Vector vecSpot = pev->origin + (pev->mins + pev->maxs);
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSpot);
	WRITE_BYTE(TE_BREAKMODEL);
	WRITE_COORD(vecSpot.x);
	WRITE_COORD(vecSpot.y);
	WRITE_COORD(vecSpot.z);
	WRITE_COORD(200);
	WRITE_COORD(200);
	WRITE_COORD(128);
	WRITE_COORD(30);
	WRITE_COORD(30);
	WRITE_COORD(255);
	WRITE_BYTE(40);
	WRITE_SHORT(m_iBodyGibs); // model id#
	WRITE_BYTE(30);
	WRITE_BYTE(200); // 1.0 seconds
	WRITE_BYTE(BREAK_CONCRETE);
	MESSAGE_END();

	UTIL_TraceLine(StartPosition,
				   StartPosition,
				   dont_ignore_monsters,
				   ENT(pev),
				   &TResult);

	// Pull out of the wall a bit
	if (TResult.flFraction != 1.0)
	{
		pev->origin = TResult.vecEndPos + (TResult.vecPlaneNormal * 25.6);
	}

	// torus
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMTORUS);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 170); // reach damage radius over .2 seconds
	WRITE_SHORT(m_iSpriteTexture2);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(128); // life
	WRITE_BYTE(64);	 // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(255); // r, g, b
	WRITE_BYTE(128); // r, g, b
	WRITE_BYTE(0);	 // r, g, b
	WRITE_BYTE(128); // brightness
	WRITE_BYTE(0);	 // speed
	MESSAGE_END();

	// torus 2
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_BEAMDISK);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z + 150); // reach damage radius over .2 seconds
	WRITE_SHORT(m_iSpriteTexture2);
	WRITE_BYTE(0);	 // startframe
	WRITE_BYTE(0);	 // framerate
	WRITE_BYTE(128); // life
	WRITE_BYTE(250); // width
	WRITE_BYTE(0);	 // noise
	WRITE_BYTE(150); // r, g, b
	WRITE_BYTE(120); // r, g, b
	WRITE_BYTE(70);	 // r, g, b
	WRITE_BYTE(100); // brightness
	WRITE_BYTE(0);	 // speed
	MESSAGE_END();

	// MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	// WRITE_BYTE(TE_LARGEFUNNEL);
	// WRITE_COORD(pev->origin.x);
	// WRITE_COORD(pev->origin.y);
	// WRITE_COORD(pev->origin.z);
	// WRITE_SHORT(m_iSpriteTexture22); // model
	// WRITE_SHORT(RANDOM_LONG(0, 10));
	// MESSAGE_END();

	// effects
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION); // This just makes a dynamic light now
	WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-150, 150));
	WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-150, 150));
	WRITE_COORD(pev->origin.z + RANDOM_FLOAT(0, 50));
	WRITE_SHORT(m_iSpriteTexture22);
	WRITE_BYTE(255); // scale * 10
	WRITE_BYTE(6);	 // framerate
	WRITE_BYTE(TE_EXPLFLAG_NONE);
	MESSAGE_END();
	// random explosions
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_EXPLOSION); // This just makes a dynamic light now
	WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-250, 150));
	WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-150, 250));
	WRITE_COORD(pev->origin.z + RANDOM_FLOAT(0, 50));
	WRITE_SHORT(m_iSpriteTexture22);
	WRITE_BYTE(255); // scale * 10
	WRITE_BYTE(4);	 // framerate
	WRITE_BYTE(TE_EXPLFLAG_NONE);
	MESSAGE_END();
	// random explosions
	// MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	// WRITE_BYTE(TE_EXPLOSION); // This just makes a dynamic light now
	// WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-150, 150));
	// WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-150, 150));
	// WRITE_COORD(pev->origin.z + RANDOM_FLOAT(0, 50));
	// WRITE_SHORT(m_iSpriteTexture22);
	// WRITE_BYTE(500); // scale * 10
	// WRITE_BYTE(5);	 // framerate
	// WRITE_BYTE(TE_EXPLFLAG_NONE);
	// MESSAGE_END();

	// big lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecSrc.x); // X
	WRITE_COORD(vecSrc.y); // Y
	WRITE_COORD(vecSrc.z); // Z
	WRITE_BYTE(64);		   // radius * 0.1
	WRITE_BYTE(64);		   // r
	WRITE_BYTE(40);		   // g
	WRITE_BYTE(1);		   // b
	WRITE_BYTE(255);	   // life * 10
	WRITE_BYTE(4);		   // decay * 0.1
	MESSAGE_END();
}

// edited in 1.26
void CRadiation::Explode()
{
	if (m_expl <= 15)
	{
		int X = RANDOM_LONG(-2048, 2048);
		int Y = RANDOM_LONG(-2048, 2048);
		int Z = RANDOM_LONG(-2048, 2048);
		Vector vecSpot = Vector(pev->origin.x + X, pev->origin.y + Y, pev->origin.z + Z);

		TraceResult tr;
		UTIL_TraceLine(vecSpot, Vector(0, 0, -8000), ignore_monsters, edict(), &tr);

		if ((UTIL_PointContents(tr.vecEndPos) == CONTENTS_SOLID))
		{
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(vecSpot.x);
			WRITE_COORD(vecSpot.y);
			WRITE_COORD(vecSpot.z);
			WRITE_SHORT(m_iSpriteTexture22);
			WRITE_BYTE(128); // scale * 10
			WRITE_BYTE(20);	 // framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_BEAMCYLINDER);
			WRITE_COORD(vecSpot.x);
			WRITE_COORD(vecSpot.y);
			WRITE_COORD(vecSpot.z);
			WRITE_COORD(vecSpot.x);
			WRITE_COORD(vecSpot.y);
			WRITE_COORD(vecSpot.z + 1600);
			WRITE_SHORT(m_iSpriteTexture22);
			WRITE_BYTE(0);	 // startframe
			WRITE_BYTE(5);	 // framerate
			WRITE_BYTE(40);	 // life
			WRITE_BYTE(30);	 // width
			WRITE_BYTE(255); // noise
			WRITE_BYTE(192); // r, g, b
			WRITE_BYTE(192); // r, g, b
			WRITE_BYTE(128); // r, g, b
			WRITE_BYTE(100); // brightness
			WRITE_BYTE(0);	 // speed
			MESSAGE_END();

			::RadiusDamage(vecSpot, pev, VARS(pev->owner), pev->dmg*10.0, 1024, CLASS_NONE, DMG_RADIATION);

			m_expl++;
		}
	}

	pev->nextthink = gpGlobals->time + 0.35;

	::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, 512, CLASS_NONE, DMG_RADIATION);

	// lots of smoke
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_SMOKE);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z - 8);
	WRITE_SHORT(g_sModelIndexBubbles);
	WRITE_BYTE(64); // scale * 10
	WRITE_BYTE(4);	// framerate
	MESSAGE_END();

	// lights
	Vector vecSrc = pev->origin;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecSrc.x); // X
	WRITE_COORD(vecSrc.y); // Y
	WRITE_COORD(vecSrc.z); // Z
	WRITE_BYTE(64);		   // radius * 0.1
	WRITE_BYTE(0);		   // r
	WRITE_BYTE(64);		   // g
	WRITE_BYTE(0);		   // b
	WRITE_BYTE(4);		   // life * 10
	WRITE_BYTE(0);		   // decay * 0.1
	MESSAGE_END();

	SetThink(MoveThink);
}

void CRadiation ::MoveThink()
{
	Explode();

	if (gpGlobals->time >= m_flDie10)
	{
		UTIL_Remove(this);
	}
}

/////////////////////////////
/////////////////////////////
/////////////////////////////
// Gauss Cannon 3rd update f-e

void CGCannon::Spawn(void)
{
	SET_MODEL(ENT(pev), "sprites/yelflare1.spr");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4));
	UTIL_SetOrigin(pev, pev->origin);
	pev->gravity = 0.0;
	m_timer = 1.0;

	pev->avelocity.z = -500;

	pev->rendermode = kRenderTransAdd; // kRenderTransAlpha
	pev->renderamt = 200;

	pev->classname = MAKE_STRING("weapon_gauss");

	m_Sprite = PRECACHE_MODEL("sprites/richo2.spr"); // explode

	m_iSpriteTexture = PRECACHE_MODEL("sprites/zbeam1.spr"); // trails

	m_iBalls = PRECACHE_MODEL("sprites/nhth1.spr");

	switch (RANDOM_LONG(0, 3))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch3.wav", 1.0, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch4.wav", 1.0, ATTN_NORM);
		break;
	case 3:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/ar2_altfire1.wav", 1.0, ATTN_NORM);
		break;
	}

	SetTouch(MoveTouch);
	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CGCannon::MoveThink()
{
	m_timer += 1.5;

	if (m_timer >= pev->dmg * 0.6)
	{
		SetTouch(NULL);
		UTIL_Remove(this);
	}

	// trails
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());	   // entity
	WRITE_SHORT(m_iSpriteTexture); // model
	WRITE_BYTE(9);				   // life
	WRITE_BYTE(10);				   // width
	WRITE_BYTE(100);			   // r, g, b
	WRITE_BYTE(100);			   // r, g, b
	WRITE_BYTE(100);			   // r, g, b
	WRITE_BYTE(160);			   // brightness
	MESSAGE_END();

	UTIL_ParticleEffect(pev->origin, Vector(0, 0, -128), 92, 25);

	pev->nextthink = gpGlobals->time + 0.1;
}

void CGCannon::MoveTouch(CBaseEntity *pOther)
{
	TraceResult TResult;
	Vector StartPosition;
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;

	UTIL_TraceLine(StartPosition,
				   StartPosition + pev->velocity.Normalize() * 64,
				   dont_ignore_monsters,
				   ENT(pev),
				   &TResult);

	// Pull out of the wall a bit
	if (TResult.flFraction != 1.0)
	{
		pev->origin = TResult.vecEndPos + (TResult.vecPlaneNormal * 25.6);
	}

	if (TResult.fAllSolid)
	{
		UTIL_Remove(this);
		return;
	}

	// animated sprite
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_SPRITE);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(m_Sprite);
	WRITE_BYTE(70);	 // scale
	WRITE_BYTE(172); // brightness
	MESSAGE_END();

	// lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecSrc.x); // X
	WRITE_COORD(vecSrc.y); // Y
	WRITE_COORD(vecSrc.z); // Z
	WRITE_BYTE(24);		   // radius * 0.1
	WRITE_BYTE(150);	   // r
	WRITE_BYTE(200);	   // g
	WRITE_BYTE(200);	   // b
	WRITE_BYTE(128);	   // time * 10
	WRITE_BYTE(16);		   // decay * 0.1
	MESSAGE_END();

	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM);

	while ((pOther = UTIL_FindEntityInSphere(pOther, pev->origin, pev->dmg * 2)) != NULL)
	{
		if (pOther->pev->takedamage && (FVisible(pOther)))
		{
			if ((pOther->edict() != pev->owner) && pOther->pev->takedamage && (pOther->edict() != edict()) && pOther->pev->health >= 3) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				Create("item_nvg", pOther->pev->origin, pev->angles, pev->owner);

				// balls
				MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
				WRITE_BYTE(TE_SPRITETRAIL);
				WRITE_COORD(pOther->pev->origin.x);
				WRITE_COORD(pOther->pev->origin.y);
				WRITE_COORD(pOther->pev->origin.z);
				WRITE_COORD(pOther->pev->origin.x);
				WRITE_COORD(pOther->pev->origin.y);
				WRITE_COORD(pOther->pev->origin.z);
				WRITE_SHORT(m_iBalls); // model
				WRITE_BYTE(9);		   // count
				WRITE_BYTE(1);		   // life * 10
				WRITE_BYTE(2);		   // size * 10
				WRITE_BYTE(90);		   // amplitude * 0.1
				WRITE_BYTE(2);		   // speed * 100
				MESSAGE_END();
			}
		}
	}

	::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, pev->dmg * 3.25, CLASS_NONE, DMG_ENERGYBEAM);
	STOP_SOUND(ENT(pev), CHAN_VOICE, "zxc/hold_loop.wav");
	SetTouch(NULL);
	UTIL_Remove(this);
}

/////////////
/////particle
/////////////

void CGParticle::Spawn()
{
	SET_MODEL(ENT(pev), "sprites/shrinkf.spr");
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	UTIL_SetOrigin(pev, pev->origin);
	pev->gravity = 0.0;

	pev->scale = 0.5;

	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 120;

	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CGParticle::MoveThink()
{
	pev->scale -= 0.04;
	pev->renderamt -= 7;
	if (pev->scale <= 0.03)
		UTIL_Remove(this);

	pev->nextthink = gpGlobals->time + 0.05;
}

////////////////////
/////particle 2/////
////////////////////

void CGParticle2::Spawn()
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->classname = MAKE_STRING("weapon_gauss");
	UTIL_SetOrigin(pev, pev->origin);
	pev->gravity = 0.0;
	pev->scale = 1.6;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 90;
	m_Sprite = PRECACHE_MODEL("sprites/zbeam4.spr");
	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 462)) != NULL)
	{
		if (pEntity->pev->takedamage && (FVisible(pEntity)))
		{
			if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict()) && pEntity->pev->health >= 3)
			{
				// shock ray
				MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
				WRITE_BYTE(TE_BEAMPOINTS);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
				WRITE_COORD(pEntity->pev->origin.y);
				WRITE_COORD(pEntity->pev->origin.z);
				WRITE_SHORT(m_Sprite); // sprite
				WRITE_BYTE(1);		   // Starting frame
				WRITE_BYTE(0);		   // framerate * 0.1
				WRITE_BYTE(4);		   // life * 0.1
				WRITE_BYTE(40);		   // width
				WRITE_BYTE(4);		   // noise
				WRITE_BYTE(250);	   // color r,g,b
				WRITE_BYTE(250);	   // color r,g,b
				WRITE_BYTE(55);		   // color r,g,b
				WRITE_BYTE(128);	   // brightness
				WRITE_BYTE(256);	   // scroll speed
				MESSAGE_END();

				UTIL_ScreenShake(pEntity->pev->origin, 12.0, 90.5, 0.3, 1);
				pEntity->TakeDamage(pev, VARS(pev->owner), RANDOM_LONG(21, 39), DMG_ENERGYBEAM);
			}
		}
	}

	UTIL_Remove(this);
}
