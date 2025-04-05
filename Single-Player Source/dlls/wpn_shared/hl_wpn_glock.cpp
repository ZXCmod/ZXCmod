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
#include "game.h"
#include "shake.h"

enum glock_e
{
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER
};

//////////////NEW weapon

class CGB : public CGrenade
{
public:
	void Spawn(void);
	void EXPORT MoveThink(void);
	void EXPORT MoveTouch(CBaseEntity *pOther);

private:
	int m_flDie;
	int m_flDie2;
	int BeamSprite;
	int m_iSpriteTexture;
	short m_LaserSprite;
	int m_iBalls;
};

LINK_ENTITY_TO_CLASS(weapon_minigun, CGB);
LINK_ENTITY_TO_CLASS(weapon_glock, CGlock);
LINK_ENTITY_TO_CLASS(weapon_9mmhandgun, CGlock);

void CGlock::Spawn()
{
	pev->classname = MAKE_STRING("weapon_9mmhandgun"); // hack to allow for old names
	Precache();
	m_iId = WEAPON_GLOCK;
	SET_MODEL(ENT(pev), "models/w_9mmhandgun.mdl");

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;

	m_typeG = 0;

	FallInit();
}

void CGlock::Precache(void)
{
	PRECACHE_MODEL("models/v_9mmhandgun.mdl");
	PRECACHE_MODEL("models/w_9mmhandgun.mdl");
	PRECACHE_MODEL("models/p_9mmhandgun.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");
	PRECACHE_MODEL("sprites/blflare.spr");
	PRECACHE_MODEL("sprites/blueflare2.spr");
	PRECACHE_MODEL("sprites/xspark3.spr");

	PRECACHE_SOUND("weapons/pl_gun1.wav"); // silenced handgun
	PRECACHE_SOUND("weapons/pl_gun2.wav"); // silenced handgun
	PRECACHE_SOUND("weapons/pl_gun3.wav"); // handgun

	PRECACHE_SOUND("zxc/GaussGun.wav");
	PRECACHE_SOUND("zxc/Build1.wav");
	PRECACHE_SOUND("zxc/Build2.wav");
	PRECACHE_SOUND("zxc/Build3.wav");
	PRECACHE_SOUND("zxc/Build4.wav");
	PRECACHE_SOUND("zxc/LsrExpl2.wav");

	m_usFireGlock1 = PRECACHE_EVENT(1, "events/glock1.sc");
	m_usFireGlock2 = PRECACHE_EVENT(1, "events/glock2.sc");
}

int CGlock::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = GLOCK_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_GLOCK;
	p->iWeight = GLOCK_WEIGHT;

	return 1;
}

void CGlock::Reload(void)
{
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload(17, GLOCK_RELOAD, 1.25);
	else
		iResult = DefaultReload(20, GLOCK_RELOAD_NOT_EMPTY, 1.25);

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8;
	}
}

BOOL CGlock::Deploy()
{
	return DefaultDeploy("models/v_9mmhandgun.mdl", "models/p_9mmhandgun.mdl", GLOCK_DRAW, "onehanded", /*UseDecrement() ? 1 : 0*/ 0);
}

void CGlock::PrimaryAttack(void)
{

	GlockFire(1.0, 0.3); // 0.25
}
void CGlock::SecondaryAttack(void)
{

	GlockFire(3.64, 0.2);
}

// updated in v1.33 with both types
void CGlock::ThirdAttack(void)
{

	if (m_iClip < 4)
		DefaultReload(20, GLOCK_RELOAD_NOT_EMPTY, 1.25);

	if (m_iClip >= 4) // need delay
	{

		SendWeaponAnim(GLOCK_SHOOT);
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		Vector vecSrc = m_pPlayer->pev->origin;

		if (m_typeG == 0) // normal
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/GaussGun.wav", 1.0, ATTN_NORM);
			Vector vecThrow = gpGlobals->v_forward * 700;

			CBaseEntity *pGlockCore = Create("weapon_minigun", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict());
			pGlockCore->pev->velocity = vecThrow;
			pGlockCore->pev->dmg = 16;
			m_iClip -= 4;

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		}
		if (m_typeG == 1) // typed as 1 (red trails)
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/GaussGun.wav", 1.0, ATTN_NORM);
			Vector vecThrow = gpGlobals->v_forward * 1200;

			CBaseEntity *pGlockCore = Create("weapon_minigun", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict());
			pGlockCore->pev->velocity = vecThrow;
			pGlockCore->pev->dmg = 24;

			m_iClip -= 4;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		}
	}
}

void CGlock::FourthAttack(void)
{

	if (m_pPlayer->pev->fov != 0)
	{
		m_fInZoom = FALSE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
		m_flNextSecondaryAttack = 0.3;
		m_typeG = 0;
	}
	else if (m_pPlayer->pev->fov != 60)
	{
		m_fInZoom = TRUE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 60;
		m_flNextSecondaryAttack = 0.3;
		m_typeG = 1;
	}
	PlayEmptySound();
	m_flNextSecondaryAttack = 0.3;
}

void CGlock::GlockFire(float flSpread, float flCycleTime)
{
	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;
		}

		return;
	}

	CBaseEntity *pEntity;
	TraceResult tr;
	Vector vecSrc;

	vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	pEntity = CBaseEntity::Instance(tr.pHit); // trace hit to entity

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_2DEGREES * flSpread, 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);
	PLAYBACK_EVENT_FULL(FEV_GLOBAL, m_pPlayer->edict(), g_vecZero ? m_usFireGlock1 : m_usFireGlock2, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
	m_iClip--;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	// draw light
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(tr.vecEndPos.x); // X
	WRITE_COORD(tr.vecEndPos.y); // Y
	WRITE_COORD(tr.vecEndPos.z); // Z
	WRITE_BYTE(4);				 // radius * 0.1
	WRITE_BYTE(80);				 // r
	WRITE_BYTE(200);			 // g
	WRITE_BYTE(80);				 // b
	WRITE_BYTE(1);				 // time * 10
	WRITE_BYTE(0);				 // decay * 0.1
	MESSAGE_END();
	// draw light 2
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecSrc.x); // X
	WRITE_COORD(vecSrc.y); // Y
	WRITE_COORD(vecSrc.z); // Z
	WRITE_BYTE(3);		   // radius * 0.1
	WRITE_BYTE(50);		   // r
	WRITE_BYTE(250);	   // g
	WRITE_BYTE(50);		   // b
	WRITE_BYTE(128);	   // time * 10
	WRITE_BYTE(16);		   // decay * 0.1
	MESSAGE_END();

	// don't hit on transparent objects
	// if (pEntity != NULL && (pEntity->IsBSPModel()))
	// 	return;

	// if (pEntity != NULL && pEntity->IsPlayer()) // is player
	// {
	// 	CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
	// 	pEntity->PTime += 72; // timer of slow
	// }
	// pEntity->TakeDamage(pev, VARS( pev->owner ), 1, DMG_PARALYZE);
}

void CGlock::Holster(int skiplocal /* = 0 */)
{
	m_fInReload = FALSE; // cancel any reload in progress.

	if (m_pPlayer->pev->fov != 0)
	{
		m_fInZoom = FALSE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
		m_flNextSecondaryAttack = 0.3;
		// return;
	}

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8;
	SendWeaponAnim(3);
}

void CGlock::WeaponIdle(void)
{
	

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	ResetEmptySound();

	// if (m_iClip > 0)
	// {
	// 	if (RANDOM_LONG(0, 5) == 0)
	// 	{
	// 		int iAnim;
	// 		float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);

	// 		if (flRand <= 0.75)
	// 		{
	// 			iAnim = GLOCK_IDLE2;
	// 		}
	// 		else
	// 		{
	// 			iAnim = GLOCK_IDLE3;				
	// 		}
	// 		SendWeaponAnim(iAnim);

	// 		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	// 	}
	// }
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
}

class CGlockAmmo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_9mmclip.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_9mmclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		if (pOther->GiveAmmo(AMMO_GLOCKCLIP_GIVE, "9mm", _9MM_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(ammo_glockclip, CGlockAmmo);
LINK_ENTITY_TO_CLASS(ammo_9mmclip, CGlockAmmo);

void CGB ::Spawn()
{
	m_iSpriteTexture = PRECACHE_MODEL("sprites/xspark3.spr"); // shockwave.spr
	m_LaserSprite = PRECACHE_MODEL("sprites/blflare.spr");
	m_iBalls = PRECACHE_MODEL("sprites/gradbeam.spr");

	SET_MODEL(ENT(pev), "sprites/blflare.spr");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	pev->angles.x = -(pev->angles.x);
	UTIL_SetOrigin(pev, pev->origin);
	pev->classname = MAKE_STRING("weapon_9mmhandgun");
	m_flDie = gpGlobals->time + 3;
	pev->dmg = 16;
	pev->takedamage = DAMAGE_NO;

	pev->avelocity.z = 64;

	pev->rendermode = kRenderTransAdd; // kRenderTransAlpha
	pev->renderamt = 195;

	SetTouch(MoveTouch);
	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 0.1;

	pev->health = 256;
}

void CGB ::MoveThink()
{
	switch (RANDOM_LONG(0, 8))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build1.wav", 0.4, ATTN_NORM);
		break;
	case 3:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build2.wav", 0.5, ATTN_NORM);
		break;
	case 5:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build3.wav", 0.6, ATTN_NORM);
		break;
	case 8:
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build4.wav", 0.7, ATTN_NORM);
		break;
	}

	// capture targets
	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 80)) != NULL)
	{
		if ((pEntity->edict() != pev->owner) && (pEntity->edict() != edict()) && pEntity->pev->health > 0) //!(pEntity->pev->movetype == MOVETYPE_FLY)
		{
			if (FBitSet(pEntity->pev->flags, FL_DUCKING) && (pEntity->pev->flags & FL_ONGROUND))
			{
				::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, 96, CLASS_NONE, DMG_SHOCK);
			}
			else // target in air
			{
				pEntity->pev->velocity.x = (((pev->velocity.x + pev->origin.x) - pEntity->pev->origin.x));
				pEntity->pev->velocity.y = (((pev->velocity.y + pev->origin.y) - pEntity->pev->origin.y));
				pEntity->pev->velocity.z = (((pev->velocity.z + pev->origin.z) - pEntity->pev->origin.z));
				::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg * 0.5, 64, CLASS_NONE, DMG_SHOCK);
			}
			// direct lighting
			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_BEAMPOINTS);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(pEntity->pev->origin.x);
			WRITE_COORD(pEntity->pev->origin.y);
			WRITE_COORD(pEntity->pev->origin.z);
			WRITE_SHORT(m_LaserSprite);
			WRITE_BYTE(0);	 // Starting frame
			WRITE_BYTE(0);	 // framerate * 0.1
			WRITE_BYTE(1);	 // life * 0.1
			WRITE_BYTE(12);	 // width
			WRITE_BYTE(0);	 // noise
			WRITE_BYTE(64);	 // color r,g,b
			WRITE_BYTE(128); // color r,g,b
			WRITE_BYTE(255); // color r,g,b
			WRITE_BYTE(200); // brightness
			WRITE_BYTE(0);	 // scroll speed
			MESSAGE_END();
		}
	}

	if (gpGlobals->time >= m_flDie) // full explode and self destroy
	{
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), 100, 200, CLASS_NONE, DMG_SHOCK); // end blast
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/LsrExpl2.wav", 1.0, ATTN_NORM);
		pev->takedamage = DAMAGE_NO;
		SetThink(SUB_Remove);
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

void CGB ::MoveTouch(CBaseEntity *pOther)
{
	TraceResult TResult;
	Vector StartPosition;
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;

	UTIL_TraceLine(StartPosition,
				   StartPosition + pev->velocity.Normalize() * 64,
				   dont_ignore_monsters,
				   ENT(pev),
				   &TResult);

	if ((UTIL_PointContents(pev->origin) == CONTENTS_WATER))
	{
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), 128, 512, CLASS_NONE, DMG_SHOCK); // end blast
		pev->dmg = 300;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/LsrExpl2.wav", 1.0, ATTN_NORM);
	}

	// full explode after touch with wall
	::RadiusDamage(pev->origin, pev, VARS(pev->owner), 26, 128, CLASS_NONE, DMG_SHOCK); // end blast

	// sprite
	pev->model = MAKE_STRING("sprites/blueflare2.spr");
	CSprite *pSprite = CSprite::SpriteCreate("sprites/blueflare2.spr", pev->origin, TRUE);
	if (pSprite)
	{
		pSprite->pev->nextthink = gpGlobals->time + 2.4;
		pSprite->pev->scale = pev->dmg * 0.05;
		pSprite->SetThink(SUB_Remove);
		pSprite->SetTransparency(kRenderTransAdd, 128, 128, 128, 200, kRenderFxGlowShell);
	}

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_PARTICLEBURST);
	WRITE_COORD(pev->origin.x); // pos
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(8);
	WRITE_BYTE(4); // color index into host_basepal
	WRITE_BYTE(1);
	MESSAGE_END();

	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/LsrExpl2.wav", 1.0, ATTN_NORM);
	SetTouch(NULL);
	UTIL_Remove(this);
}
