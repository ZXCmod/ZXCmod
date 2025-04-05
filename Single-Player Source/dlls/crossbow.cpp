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
#if !defined(OEM_BUILD) && !defined(HLDEMO_BUILD)

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "decals.h"

// UNDONE: Save/restore this?  Don't forget to set classname and LINK_ENTITY_TO_CLASS()
//
// OVERLOADS SOME ENTVARS:
//
// speed - the ideal magnitude of my velocity
class CCrossbowBolt : public CBaseEntity
{
	void Spawn(void);
	void Precache(void);
	int Classify();

public:
	static CCrossbowBolt *BoltCreate(void);
};
LINK_ENTITY_TO_CLASS(crossbow_bolt, CCrossbowBolt);

CCrossbowBolt *CCrossbowBolt::BoltCreate(void)
{
	// Create a new entity with CCrossbowBolt private data
	CCrossbowBolt *pBolt = GetClassPtr((CCrossbowBolt *)NULL);
	pBolt->pev->classname = MAKE_STRING("bolt");
	pBolt->Spawn();

	return pBolt;
}

void CCrossbowBolt::Spawn()
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/crossbow_bolt.mdl");

	UTIL_SetOrigin(pev, pev->origin + Vector(-7, 0, 0));
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hit1.wav", 1.0, ATTN_NORM);
	pev->nextthink = gpGlobals->time + 1.0;
	SetThink(SUB_Remove);
}

void CCrossbowBolt::Precache()
{
	PRECACHE_MODEL("models/crossbow_bolt.mdl");
	PRECACHE_SOUND("weapons/xbow_hitbod1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod2.wav");
	PRECACHE_SOUND("weapons/xbow_fly1.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");
	PRECACHE_SOUND("fvox/beep.wav");
}

int CCrossbowBolt ::Classify()
{
	return CLASS_NONE;
}
// end

// HE-bolt
class CCrossbowBolt2 : public CBaseEntity
{
public:
	void Spawn(void);
	void EXPORT MoveThink();
	void EXPORT MoveTouch(CBaseEntity *pOther);
	//CBaseEntity *m_hEnemy;
	edict_t 	*m_paraTarget;

private:
	BOOL g_lock;
	short g_sModelIndexLaser;
};

LINK_ENTITY_TO_CLASS(bone_follow, CCrossbowBolt2);

void CCrossbowBolt2::Spawn()
{
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->ltime = 1.0;
	pev->gravity = 0.0;
	pev->friction = 0.0;

	pev->classname = MAKE_STRING("bolt");

	g_lock = FALSE;

	Vector vecThrow;
	vecThrow = gpGlobals->v_forward * 2048;
	pev->velocity = vecThrow;

	g_sModelIndexLaser = PRECACHE_MODEL("sprites/plasma.spr");

	pev->angles = UTIL_VecToAngles(pev->velocity);

	pev->effects |= EF_LIGHT;

	// set trails
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(entindex());		 // entity
	WRITE_SHORT(g_sModelIndexLaser); // model
	WRITE_BYTE(10);					 // life
	WRITE_BYTE(1);					 // width
	WRITE_BYTE(64);					 // r, g, b
	WRITE_BYTE(128);				 // r, g, b
	WRITE_BYTE(255);				 // r, g, b
	WRITE_BYTE(255);				 // brightness
	MESSAGE_END();					 // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	SET_MODEL(ENT(pev), "models/crossbow_bolt.mdl");

	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, Vector(-4, -4, -3), Vector(4, 4, 3));

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hit1.wav", 1.0, ATTN_NORM);
	pev->nextthink = gpGlobals->time + 0.01;
	SetThink(MoveThink);
	SetTouch(MoveTouch);
}

void CCrossbowBolt2::MoveThink()
{
	pev->nextthink = gpGlobals->time + 0.1;

	pev->ltime += 1.0;

	if (g_lock == TRUE)
	{
		pev->velocity = g_vecZero;

		CBaseEntity *m_hEnemy;
		if (m_paraTarget)
			m_hEnemy = CBaseEntity::Instance(m_paraTarget);

		if (m_hEnemy == NULL || pev->ltime >= 60.0) //m_hEnemy == NULL || 
		{
			SetTouch(NULL);
			pev->ltime = 15.0;
			g_lock = FALSE;
			pev->effects = 0;
			pev->takedamage = DAMAGE_NO;
			pev->nextthink = gpGlobals->time + 1.1;
			m_hEnemy = NULL;
		}

		if (m_hEnemy != NULL)
		{
			if (m_hEnemy->IsAlive())
			{
				Vector vecDir;
				UTIL_MakeVectorsPrivate(pev->angles, vecDir, NULL, NULL);
				m_hEnemy->pev->velocity = (pev->origin - m_hEnemy->pev->origin);

				//m_hEnemy->TakeDamage(pev, VARS(pev->owner), 1.0, DMG_BULLET);

				// draw line
				MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
				WRITE_BYTE(TE_BEAMPOINTS);
				WRITE_COORD(pev->origin.x);
				WRITE_COORD(pev->origin.y);
				WRITE_COORD(pev->origin.z);
				WRITE_COORD(m_hEnemy->pev->origin.x);
				WRITE_COORD(m_hEnemy->pev->origin.y);
				WRITE_COORD(m_hEnemy->pev->origin.z);
				WRITE_SHORT(g_sModelIndexLaser); // sprite
				WRITE_BYTE(1);					 // Starting frame
				WRITE_BYTE(0);					 // framerate * 0.1
				WRITE_BYTE(1);					 // life * 0.1
				WRITE_BYTE(8);					 // width
				WRITE_BYTE(1);					 // noise
				WRITE_BYTE(250);				 // color r,g,b
				WRITE_BYTE(250);				 // color r,g,b
				WRITE_BYTE(250);				 // color r,g,b
				WRITE_BYTE(255);				 // brightness
				WRITE_BYTE(255);				 // scroll speed
				MESSAGE_END();

				
				
			}
			else
			{
				m_hEnemy = NULL;
			}
		}
	}
	else // no targets
	{
		if (pev->ltime >= 20.0)
		{
			pev->takedamage = DAMAGE_NO;
			::RadiusDamage(pev->origin, pev, VARS(pev->owner), 50.0, 128.0, CLASS_NONE, DMG_MORTAR); // DMG

			// explode
			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_EXPLOSION);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexFireball);
			WRITE_BYTE(10); // scale
			WRITE_BYTE(24); // framerate
			WRITE_BYTE(TE_EXPLFLAG_NONE);
			MESSAGE_END();

			// smoke
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(pev->dmg); // smoke scale * 10
			WRITE_BYTE(24);		  // framerate
			MESSAGE_END();

			SetTouch(NULL);
			pev->nextthink = gpGlobals->time + 1.0;
			UTIL_Remove(this);
		}
	}
}

void CCrossbowBolt2 ::MoveTouch(CBaseEntity *pOther)
{
	TraceResult tr = UTIL_GetGlobalTrace();
	pev->enemy = pOther->edict();

	pev->velocity = g_vecZero;

	Vector vecSrc = pev->origin;
	Vector vecDir;
	UTIL_MakeVectorsPrivate(pev->angles, vecDir, NULL, NULL);

	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	if (pOther->pev->takedamage > 0)
	{
		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 4096, ignore_monsters, edict(), &tr);
		UTIL_BloodDecalTrace(&tr, BLOOD_COLOR_RED);
		pev->origin = tr.vecEndPos;

		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;

		// lights
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x); // X
		WRITE_COORD(vecSrc.y); // Y
		WRITE_COORD(vecSrc.z); // Z
		WRITE_BYTE(16);		   // radius * 0.1
		WRITE_BYTE(254);	   // r
		WRITE_BYTE(0);		   // g
		WRITE_BYTE(0);		   // b
		WRITE_BYTE(128);	   // time * 10
		WRITE_BYTE(16);		   // decay * 0.1
		MESSAGE_END();

		g_lock = TRUE;
		//m_hEnemy = pOther;
		m_paraTarget = pOther->edict();

		pev->velocity = g_vecZero;															 
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), 10.0, 512.0, CLASS_NONE, DMG_MORTAR); 

		if (tr.pHit && Instance(tr.pHit)->pev->solid != SOLID_BSP)
		{
			SetTouch(NULL);
			UTIL_Remove(this);
		}

		pev->nextthink = gpGlobals->time + 0.01;
	}
	else
	{
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), 50.0, 128.0, CLASS_NONE, DMG_MORTAR);

		// explode
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexFireball);
		WRITE_BYTE(10); // scale
		WRITE_BYTE(24); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();

		// smoke
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_SMOKE);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexSmoke);
		WRITE_BYTE(pev->dmg); // smoke scale * 10
		WRITE_BYTE(24);		  // framerate
		MESSAGE_END();

		UTIL_Remove(this);
	}
	SetTouch(NULL);
}

enum crossbow_e
{
	CROSSBOW_IDLE1 = 0, // full
	CROSSBOW_IDLE2,		// empty
	CROSSBOW_FIDGET1,	// full
	CROSSBOW_FIDGET2,	// empty
	CROSSBOW_FIRE1,		// full
	CROSSBOW_FIRE2,		// reload
	CROSSBOW_FIRE3,		// empty
	CROSSBOW_RELOAD,	// from empty
	CROSSBOW_DRAW1,		// full
	CROSSBOW_DRAW2,		// empty
	CROSSBOW_HOLSTER1,	// full
	CROSSBOW_HOLSTER2,	// empty
};

LINK_ENTITY_TO_CLASS(weapon_crossbow, CCrossbow);

void CCrossbow::Spawn()
{
	Precache();
	m_iId = WEAPON_CROSSBOW;
	SET_MODEL(ENT(pev), "models/w_crossbow.mdl");

	m_iDefaultAmmo = CROSSBOW_DEFAULT_GIVE;

	FallInit();
}

int CCrossbow::AddToPlayer(CBasePlayer *pPlayer)
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

void CCrossbow::Precache(void)
{
	PRECACHE_MODEL("models/w_crossbow.mdl");
	PRECACHE_MODEL("models/v_crossbow.mdl");
	PRECACHE_MODEL("models/p_crossbow.mdl");
	PRECACHE_MODEL("sprites/plasma.spr");

	PRECACHE_SOUND("weapons/xbow_fire1.wav");
	PRECACHE_SOUND("weapons/xbow_reload1.wav");

	UTIL_PrecacheOther("crossbow_bolt");

	m_usCrossbow = PRECACHE_EVENT(1, "events/crossbow1.sc");
	m_usCrossbow2 = PRECACHE_EVENT(1, "events/crossbow2.sc");
}

int CCrossbow::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "bolts";
	p->iMaxAmmo1 = BOLT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = CROSSBOW_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 2;
	p->iId = WEAPON_CROSSBOW;
	p->iFlags = 0;
	p->iWeight = CROSSBOW_WEIGHT;
	return 1;
}

BOOL CCrossbow::Deploy()
{
	
	if (m_iClip)
		return DefaultDeploy("models/v_crossbow.mdl", "models/p_crossbow.mdl", CROSSBOW_DRAW1, "bow");
	return DefaultDeploy("models/v_crossbow.mdl", "models/p_crossbow.mdl", CROSSBOW_DRAW2, "bow");
}

void CCrossbow::Holster(int skiplocal /* = 0 */)
{
	m_fInReload = FALSE; // cancel any reload in progress.

	if (m_fInZoom)
	{
		SecondaryAttack();
	}

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	if (m_iClip)
		SendWeaponAnim(CROSSBOW_HOLSTER1);
	else
		SendWeaponAnim(CROSSBOW_HOLSTER2);
}

void CCrossbow::PrimaryAttack(void)
{

	FireSniperBolt();
}

// Edited in v1.34
void CCrossbow::FireSniperBolt()
{

	if (m_iClip == 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5; // 0.75
		return;
	}

	TraceResult tr;
	int playernum;
	int nFrames;
	CBasePlayer *pPlayer;
	pPlayer = (CBasePlayer *)GET_PRIVATE(pev->owner);
	Vector Temp = m_pPlayer->pev->origin;
	m_pPlayer->pev->punchangle.y += RANDOM_FLOAT(-0.15, 0.15);
	m_pPlayer->pev->punchangle.x += RANDOM_FLOAT(-0.15, 0.15);
	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle; 
	UTIL_MakeVectors(anglesAim); // Vector( 0.03268, 0.13716, 0.13134 )
	Vector vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2;
	Vector vecDir = gpGlobals->v_forward; // + Vector( RANDOM_FLOAT( -0.005, 0.005 ), RANDOM_FLOAT( -0.005, 0.005 ),RANDOM_FLOAT( -0.005, 0.005 )
	if (m_fInZoom == 0)
		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8192, ignore_monsters, m_pPlayer->edict(), &tr);
	else
		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8192, dont_ignore_monsters, m_pPlayer->edict(), &tr);

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usCrossbow, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType], 0, 0);

	CBaseEntity *pSatchel = Create("crossbow_bolt", tr.vecEndPos, m_pPlayer->pev->angles, m_pPlayer->edict()); // Vector(0,0,0)

	if (pPlayer)
		nFrames = pPlayer->GetCustomDecalFrames();
	else
		nFrames = -1;

	playernum = ENTINDEX(pev->owner);

	UTIL_PlayerDecalTrace(&tr, playernum, pev->frame, TRUE);

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_iClip--;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	// for ( int i = 0; i < 3; i++ )
	if (m_fInZoom == 0)
	{

		if (!m_pPlayer->IsInWorld() && (UTIL_PointContents(tr.vecEndPos) == CONTENTS_SKY)) // && (Instance( tr.pHit )->IsBSPModel())
		{
			return;
		}
		TraceResult tr2;
		UTIL_TraceHull(tr.vecEndPos + (tr.vecPlaneNormal * 40), m_pPlayer->Center(), dont_ignore_monsters, human_hull, m_pPlayer->edict(), &tr2);

		if (tr2.fStartSolid)
		{
			// GetClassPtr( (CBasePlayer *)m_pPlayer->pev)->Reset( ); // reset position without stuck
			UTIL_SetOrigin(m_pPlayer->pev, Temp);
			MESSAGE_BEGIN(MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev));
			WRITE_STRING("Teleport destination is wrong.");
			MESSAGE_END();
		}
		else
		{
			m_pPlayer->pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * 40); // teleport

			// vis effects #1
			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_TELEPORT);
			WRITE_COORD(m_pPlayer->pev->origin.x);
			WRITE_COORD(m_pPlayer->pev->origin.y);
			WRITE_COORD(m_pPlayer->pev->origin.z);
			MESSAGE_END();

			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "debris/beamstart7.wav", 1.0, ATTN_NORM); // play sound 1st
		}

		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(tr.vecEndPos.x);
		WRITE_COORD(tr.vecEndPos.y);
		WRITE_COORD(tr.vecEndPos.z);
		WRITE_SHORT(g_sModelIndexLaser); // model
		WRITE_BYTE(0);					 // framestart?
		WRITE_BYTE(0);					 // framerate?
		WRITE_BYTE(20);					 // life
		WRITE_BYTE(10);					 // width
		WRITE_BYTE(0);					 // noise
		WRITE_BYTE(200);				 // r, g, b
		WRITE_BYTE(100);				 // r, g, b
		WRITE_BYTE(0);					 // r, g, b
		WRITE_BYTE(64);					 // brightness
		WRITE_BYTE(0);					 // speed?
		MESSAGE_END();
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(tr.vecEndPos.x);
		WRITE_COORD(tr.vecEndPos.y);
		WRITE_COORD(tr.vecEndPos.z);
		WRITE_SHORT(g_sModelIndexLaser); // model
		WRITE_BYTE(0);					 // framestart?
		WRITE_BYTE(0);					 // framerate?
		WRITE_BYTE(26);					 // life
		WRITE_BYTE(16);					 // width
		WRITE_BYTE(0);					 // noise
		WRITE_BYTE(200);				 // r, g, b
		WRITE_BYTE(100);				 // r, g, b
		WRITE_BYTE(0);					 // r, g, b
		WRITE_BYTE(64);					 // brightness
		WRITE_BYTE(0);					 // speed?
		MESSAGE_END();

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
	}
	else
	{

#ifndef CLIENT_DLL
		if (tr.pHit->v.takedamage)
		{
			ClearMultiDamage();
			CBaseEntity::Instance(tr.pHit)->TraceAttack(m_pPlayer->pev, 120.0, vecDir, &tr, DMG_BULLET);
			ApplyMultiDamage(pev, m_pPlayer->pev);
		}
#endif

		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(tr.vecEndPos.x);
		WRITE_COORD(tr.vecEndPos.y);
		WRITE_COORD(tr.vecEndPos.z);
		WRITE_SHORT(g_sModelIndexLaser); // model
		WRITE_BYTE(0);					 // framestart?
		WRITE_BYTE(0);					 // framerate?
		WRITE_BYTE(7);					 // life
		WRITE_BYTE(1);					 // width
		WRITE_BYTE(0);					 // noise
		WRITE_BYTE(200);				 // r, g, b
		WRITE_BYTE(100);				 // r, g, b
		WRITE_BYTE(0);					 // r, g, b
		WRITE_BYTE(50);					 // brightness
		WRITE_BYTE(0);					 // speed?
		MESSAGE_END();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	}
}

void CCrossbow::FireBolt()
{

	// deprecated
}

void CCrossbow::SecondaryAttack()
{

	if (m_pPlayer->pev->fov != 0)
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
		m_fInZoom = 0;
	}
	else if (m_pPlayer->pev->fov != 20)
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 20;
		m_fInZoom = 1;
	}

	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
}

void CCrossbow::ThirdAttack()
{

	TraceResult tr;

	int iCount;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;

	if (m_iClip == 0)
	{
		PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_iClip--;

	for (iCount = 0; iCount < 5; iCount++)
	{
		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
		UTIL_MakeVectors(anglesAim); // Vector( 0.03268, 0.13716, 0.13134 )
		Vector vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2;
		Vector vecDir = gpGlobals->v_forward;

		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8192, dont_ignore_monsters, m_pPlayer->edict(), &tr);

		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usCrossbow, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType], 0, 0);

		Create("crossbow_bolt", tr.vecEndPos, m_pPlayer->pev->angles, m_pPlayer->edict()); // Vector(0,0,0)

		if (!FBitSet(m_pPlayer->pev->flags, FL_DUCKING)) // the true sensor
		{
			m_pPlayer->pev->punchangle.y = RANDOM_FLOAT(-3.75, 3.75);
			pev->dmg = 36; // 75
		}
		else
		{
			m_pPlayer->pev->punchangle.y = RANDOM_FLOAT(-1.0, 1.0);
			pev->dmg = 35;
		}
		// 15.35
#ifndef CLIENT_DLL
		if (tr.pHit->v.takedamage)
		{
			ClearMultiDamage();
			CBaseEntity::Instance(tr.pHit)->TraceAttack(m_pPlayer->pev, pev->dmg, vecDir, &tr, DMG_SHOCK);
			ApplyMultiDamage(pev, m_pPlayer->pev);
		}
#endif

		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(tr.vecEndPos.x);
		WRITE_COORD(tr.vecEndPos.y);
		WRITE_COORD(tr.vecEndPos.z);
		WRITE_SHORT(g_sModelIndexLaser); // model
		WRITE_BYTE(0);					 // framestart?
		WRITE_BYTE(0);					 // framerate?
		WRITE_BYTE(12);					 // life
		WRITE_BYTE(1);					 // width
		WRITE_BYTE(0);					 // noise
		WRITE_BYTE(pev->dmg);			 // r, g, b
		WRITE_BYTE(128);				 // r, g, b
		WRITE_BYTE(255);				 // r, g, b
		WRITE_BYTE(128);				 // brightness
		WRITE_BYTE(0);					 // speed?
		MESSAGE_END();
	}

	pev->nextthink = UTIL_WeaponTimeBase() + 1.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
}

void CCrossbow::FourthAttack()
{

	Vector vecThrow = gpGlobals->v_forward * 2048; // 1600

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;

	if (m_iClip == 0)
	{
		PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_iClip--;

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usCrossbow, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType], 0, 0);

	CBaseEntity *pCr = Create("bone_follow", m_pPlayer->GetGunPosition() + gpGlobals->v_up * -6 + gpGlobals->v_right * 5, m_pPlayer->pev->v_angle, m_pPlayer->edict());

	pCr->pev->velocity = vecThrow;

	pev->nextthink = UTIL_WeaponTimeBase() + 1.0;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
}

void CCrossbow::Reload(void)
{
	if (m_pPlayer->ammo_bolts <= 0)
		return;

	if (m_pPlayer->pev->fov != 0)
	{
		SecondaryAttack();
	}

	if (DefaultReload(5, CROSSBOW_RELOAD, 3.5)) // 5, 4.5
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/xbow_reload1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0, 0xF));
	}
}

void CCrossbow::WeaponIdle(void)
{
	ResetEmptySound();

	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
		SendWeaponAnim(CROSSBOW_IDLE1);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 7.0;
	}
}

class CCrossbowAmmo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_crossbow_clip.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_crossbow_clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		if (pOther->GiveAmmo(AMMO_CROSSBOWCLIP_GIVE, "bolts", BOLT_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(ammo_crossbow, CCrossbowAmmo);

#endif