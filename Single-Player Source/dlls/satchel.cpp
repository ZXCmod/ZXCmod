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
#include "shake.h"

////INIT CRYSTAL

#define BLASTER_BEAM_SPRITE "sprites/smoke.spr"

/// class crystal
class CHealCrystal : public CGrenade
{
public:
	void Spawn();
	void Precache();
	void EXPORT MoveThink();
	void EXPORT Hit(CBaseEntity *);
	static CHealCrystal *Create(Vector, Vector, CBaseEntity *);

private:
	int BeamSprite;
	int m_iSpriteTexture;
	float m_flDie;
	float m_flDie2;
};

class TriipleDamage_Crystal : public CBaseEntity
{
public:
	void Spawn(void);
	void EXPORT MoveThink(void);

private:
	short m_LaserSprite;
	int m_iSpriteTexture;
	int m_flDie;
};

/// class Tripmine Rocket
class TRocket : public CBaseEntity
{
public:
	void Spawn(void);
	void EXPORT MoveThink(void);
	void EXPORT MoveTouch(CBaseEntity *pOther);

private:
	short m_LaserSprite;
	int m_iSpriteTexture;
	int m_flDie;
};

enum satchel_e
{
	SATCHEL_IDLE1 = 0,
	SATCHEL_FIDGET1,
	SATCHEL_DRAW,
	SATCHEL_DROP
};

enum satchel_radio_e
{
	SATCHEL_RADIO_IDLE1 = 0,
	SATCHEL_RADIO_FIDGET1,
	SATCHEL_RADIO_DRAW,
	SATCHEL_RADIO_FIRE,
	SATCHEL_RADIO_HOLSTER
};

class CSatchelCharge : public CGrenade
{
	public:
		void Spawn(void);
		void Precache(void);
		void BounceSound(void);
		void EXPORT SatchelSlide(CBaseEntity *pOther);
		void EXPORT SatchelThink(void);
		void Deactivate(void);
};

// 1.31 new wp
class Jumping_Satchel : public CGrenade
{
	public:
		void Spawn(void);
		void EXPORT SatchelSlide(CBaseEntity *pOther);
		void EXPORT SatchelThink(void);
		void Deactivate(void);
};

//=========================================================
// Deactivate - do whatever it is we do to an orphaned
// satchel when we don't want it in the world anymore.
//=========================================================
void CSatchelCharge::Deactivate(void)
{

	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;
	UTIL_Remove(this);
}

void Jumping_Satchel::Deactivate(void)
{
	// Use( m_pPlayer, m_pPlayer, USE_ON, 0 );
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;
	UTIL_Remove(this);
}

void CSatchelCharge ::Spawn(void)
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_satchel.mdl"); //w_medkit
	// UTIL_SetSize(pev, Vector( -16, -16, -4), Vector(16, 16, 32));	// Old box -- size of headcrab monsters/players get blocked by this
	UTIL_SetSize(pev, Vector(-5, -5, -3), Vector(5, 5, 3)); // Uses point-sized, and can be stepped over
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(SatchelSlide);
	SetUse(DetonateUse);
	SetThink(SatchelThink);
	pev->nextthink = gpGlobals->time + 0.1;

	pev->gravity = 0.5;
	pev->friction = 0.8;

	pev->dmg = 120.0;
	pev->sequence = 1;

	pev->body = 0;

	pev->health = 100.0;
	pev->takedamage = DAMAGE_YES;
}

void CSatchelCharge::SatchelSlide(CBaseEntity *pOther)
{
	entvars_t *pevOther = pOther->pev;

	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	// pev->avelocity = Vector (300, 300, 300);
	pev->gravity = 1.0; // normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin - Vector(0, 0, 10), ignore_monsters, edict(), &tr);

	if (tr.flFraction < 1.0)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
		// play sliding sound, volume based on velocity
	}
	if (!(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10)
	{
		BounceSound();
	}
	StudioFrameAdvance();
}

void CSatchelCharge ::SatchelThink(void)
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->waterlevel == 3)
	{
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = pev->velocity * 0.8;
		pev->avelocity = pev->avelocity * 0.9;
		pev->velocity.z += 8;
	}
	else if (pev->waterlevel == 0)
		pev->movetype = MOVETYPE_BOUNCE;
	else
		pev->velocity.z -= 8;
}

void CSatchelCharge ::Precache(void)
{
	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
	PRECACHE_SOUND("zxc/crystal_heal.wav");
	PRECACHE_MODEL("sprites/shrinkf.spr");
	PRECACHE_MODEL("models/crystal.mdl");
	PRECACHE_SOUND("zxc/warningbell1.wav");
	PRECACHE_SOUND("zxc/sg_24.wav");
	PRECACHE_MODEL("sprites/xflare1.spr");

	PRECACHE_MODEL("models/v_satchel.mdl");
	PRECACHE_MODEL("models/v_satchel_radio.mdl");
	PRECACHE_MODEL("models/w_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel_radio.mdl");
	PRECACHE_MODEL("models/w_medkit.mdl");
	PRECACHE_MODEL("models/snapbug.mdl");
	PRECACHE_MODEL("models/crystal3.mdl");
	PRECACHE_MODEL("sprites/xbeam4.spr");
	PRECACHE_SOUND("weapons/dryfire1.wav");
	PRECACHE_SOUND("weapons/pl_gun1.wav");
}

void CSatchelCharge ::BounceSound(void)
{
	switch (RANDOM_LONG(0, 2))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce1.wav", 1, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce2.wav", 1, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce3.wav", 1, ATTN_NORM);
		break;
	}
}

//=========================================================
// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
//=========================================================
int CSatchel::AddDuplicate(CBasePlayerItem *pOriginal)
{
	CSatchel *pSatchel;

	pSatchel = (CSatchel *)pOriginal;

	if (pSatchel->m_chargeReady != 0)
	{
		return FALSE;
	}

	return CBasePlayerWeapon::AddDuplicate(pOriginal);
}

//=========================================================
//=========================================================
int CSatchel::AddToPlayer(CBasePlayer *pPlayer)
{
	int bResult = CBasePlayerItem::AddToPlayer(pPlayer);

	pPlayer->pev->weapons |= (1 << m_iId);
	m_chargeReady = 0;

	if (bResult)
	{
		return AddWeapon();
	}
	return FALSE;
}

void CSatchel::Spawn()
{
	Precache();
	m_iId = WEAPON_SATCHEL;
	SET_MODEL(ENT(pev), "models/w_satchel.mdl");

	m_iDefaultAmmo = SATCHEL_DEFAULT_GIVE;
	m_flNextCrystalTime = gpGlobals->time; // start timer
	FallInit(); // get ready to fall down.
}

void CSatchel::Precache(void)
{
	UTIL_PrecacheOther("monster_satchel");
}

int CSatchel::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Satchel Charge";
	p->iMaxAmmo1 = SATCHEL_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 1;
	p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = m_iId = WEAPON_SATCHEL;
	p->iWeight = SATCHEL_WEIGHT;
	// creload = 0;
	return 1;
}

//=========================================================
//=========================================================
BOOL CSatchel::IsUseable(void)
{
	if (m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] > 0)
	{
		// player is carrying some satchels
		return TRUE;
	}

	if (m_chargeReady != 0)
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}

	return FALSE;
}

BOOL CSatchel::CanDeploy(void)
{
	if (m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] > 0)
	{
		// player is carrying some satchels
		return TRUE;
	}

	if (m_chargeReady != 0)
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}

	return FALSE;
}

BOOL CSatchel::Deploy()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;

	if (m_chargeReady)
		return DefaultDeploy("models/v_satchel_radio.mdl", "models/p_satchel_radio.mdl", SATCHEL_RADIO_DRAW, "hive");
	else
		return DefaultDeploy("models/v_satchel.mdl", "models/p_satchel.mdl", SATCHEL_DRAW, "trip");

	return TRUE;
}

void CSatchel::Holster(int skiplocal /* = 0 */)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (m_chargeReady)
	{
		SendWeaponAnim(SATCHEL_RADIO_HOLSTER);
	}
	else
	{
		SendWeaponAnim(SATCHEL_DROP);
	}
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "common/null.wav", 1.0, ATTN_NORM);

	if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_chargeReady)
	{
		m_pPlayer->pev->weapons &= ~(1 << WEAPON_SATCHEL);
		SetThink(DestroyItem);
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CSatchel::PrimaryAttack()
{
	switch (m_chargeReady)
	{
	case 0:
	{
		Throw();
	}
	break;
	case 1:
	{
		SendWeaponAnim(SATCHEL_RADIO_FIRE);

		edict_t *pPlayer = m_pPlayer->edict();

		CBaseEntity *pSatchel = NULL;

		while ((pSatchel = UTIL_FindEntityByClassname(pSatchel, "monster_satchel")) != NULL) // 1.35 fix 'no happened' bug
		{
			if (FClassnameIs(pSatchel->pev, "monster_satchel") || FClassnameIs(pSatchel->pev, "monster_pipebomb"))
			{
				if (pSatchel->pev->owner == pPlayer)
				{
					pSatchel->Use(m_pPlayer, m_pPlayer, USE_ON, 0);
					m_chargeReady = 2;
				}
			}
		}

		m_chargeReady = 2;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		break;
	}

	case 2:
		// we're reloading, don't allow fire
		{
		}
		break;
	}
}

void CSatchel::SecondaryAttack(void)
{

		if (m_chargeReady != 2)
		{
			Throw();
		}
	
}

void CSatchel::ThirdAttack(void)
{
	// Invisible
	// new code for 1.26

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 5)
	{
		CBasePlayer *pl = (CBasePlayer *)CBasePlayer::Instance(m_pPlayer->pev);

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "debris/beamstart1.wav", 0.9, ATTN_NORM);

		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		// m_pPlayer->pev->health = 10;

		SET_MODEL(ENT(m_pPlayer->pev), "models/rpgrocket.mdl");

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 5;
		m_pPlayer->pev->effects |= EF_NODRAW; // true invis
		pl->m_pActiveItem->m_iId = WEAPON_CROWBAR;
		UTIL_ScreenFade(m_pPlayer, Vector(0, 0, 200), 1.95, 3.5, 70, FFADE_IN);

		MESSAGE_BEGIN(MSG_ONE, gmsgHudText, NULL, ENT(m_pPlayer->pev));
		WRITE_STRING("Now you are invisible.");
		MESSAGE_END();

		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "buttons/bell1.wav", 0.75, ATTN_NORM, 1.0, 102);
		return;
	}
}

void CSatchel::FourthAttack()
{

	if (g_zxc_promode.value == 0 && (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 2) && m_chargeReady != 2)
	{
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

		CBaseEntity *pSatchel = Create("monster_pipebomb", vecSrc, Vector(0, 0, 0), m_pPlayer->edict());
		pSatchel->pev->velocity = vecThrow;

		SendWeaponAnim(SATCHEL_RADIO_FIRE);
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel_radio.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");

		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		m_chargeReady = 1;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 2;
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}

void CSatchel::Throw(void)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		Vector vecSrc = m_pPlayer->pev->origin;

		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

#ifndef CLIENT_DLL
		CBaseEntity *pSatchel = Create("monster_satchel", vecSrc, Vector(0, 0, 0), m_pPlayer->edict());
		pSatchel->pev->velocity = vecThrow;
		pSatchel->pev->avelocity.y = 400;

		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel_radio.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");
#else
		LoadVModel("models/v_satchel_radio.mdl", m_pPlayer);
#endif

		SendWeaponAnim(SATCHEL_RADIO_DRAW);

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		m_chargeReady = 1;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}

void CSatchel::WeaponIdle(void)
{

	if (m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 5) // && creload==0

		if (m_pPlayer->m_flNextCrystalTime < gpGlobals->time)
		{

			{
				// reload when reload is pressed, or if no buttons are down and weapon is empty.
				Reload();
				m_pPlayer->m_flNextCrystalTime = gpGlobals->time + 90;
				return;
			}
		}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	switch (m_chargeReady)
	{
	case 0:
		SendWeaponAnim(SATCHEL_FIDGET1);
		// use tripmine animations
		strcpy(m_pPlayer->m_szAnimExtention, "trip");
		break;
	case 1:
		SendWeaponAnim(SATCHEL_RADIO_FIDGET1);
		// use hivehand animations
		strcpy(m_pPlayer->m_szAnimExtention, "hive");
		break;
	case 2:
		if (!m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			m_chargeReady = 0;
			RetireWeapon();
			return;
		}

#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel.mdl");
#else
		LoadVModel("models/v_satchel.mdl", m_pPlayer);
#endif

		SendWeaponAnim(SATCHEL_DRAW);

		// use tripmine animations
		strcpy(m_pPlayer->m_szAnimExtention, "trip");

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = 0;
		break;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 10.0;
}

void CSatchel ::Reload(void)
{

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecThrow = gpGlobals->v_forward;
	Vector GunPosition = m_pPlayer->GetGunPosition();
	// CHealCrystal :: Create( GunPosition, vecThrow, m_pPlayer );
	CBaseEntity::Create("heal_crystal", GunPosition, Vector(0, 0, 0), m_pPlayer->edict());
	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 5;
}

//=========================================================
// DeactivateSatchels - removes all satchels owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateSatchels(CBasePlayer *pOwner)
{
	edict_t *pFind;

	pFind = FIND_ENTITY_BY_CLASSNAME(NULL, "monster_satchel");

	while (!FNullEnt(pFind))
	{
		CBaseEntity *pEnt = CBaseEntity::Instance(pFind);
		CSatchelCharge *pSatchel = (CSatchelCharge *)pEnt;

		if (pSatchel)
		{
			if (pSatchel->pev->owner == pOwner->edict())
			{
				pSatchel->Use(pOwner, pOwner, USE_ON, 0);
				// pSatchel->Deactivate();
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME(pFind, "monster_satchel");
	}

	///////////////
	edict_t *pFind2;
	pFind2 = FIND_ENTITY_BY_CLASSNAME(NULL, "monster_pipebomb");
	while (!FNullEnt(pFind2))
	{
		CBaseEntity *pEnt = CBaseEntity::Instance(pFind2);
		CSatchelCharge *pSatchel = (CSatchelCharge *)pEnt;

		if (pSatchel)
		{
			if (pSatchel->pev->owner == pOwner->edict())
			{
				pSatchel->Use(pOwner, pOwner, USE_ON, 0);
				// pSatchel->Deactivate();
			}
		}

		pFind2 = FIND_ENTITY_BY_CLASSNAME(pFind2, "monster_pipebomb");
	}
}

#endif

////// New code for heal crystal

#define SQUEEK_DETONATE_DELAY 90.0

void CHealCrystal ::Spawn()
{

	Precache();
	SET_MODEL(ENT(pev), "models/crystal.mdl");

	pev->movetype = MOVETYPE_TOSS; // So gravity affects it a *tad*
	pev->solid = SOLID_BBOX;

	UTIL_SetSize(pev, Vector(-5, -5, -4), Vector(5, 5, 12));
	UTIL_SetOrigin(pev, pev->origin);
	//pev->classname = MAKE_STRING("Crystal");

	SetThink(MoveThink);
	SetTouch(Hit);

	pev->velocity = gpGlobals->v_forward;
	pev->angles.x = -(pev->angles.x);

	pev->nextthink = gpGlobals->time + 0.1;
	pev->dmg = 1.0;
	pev->takedamage = 0;

	pev->effects = EF_MUZZLEFLASH;
	m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;

	pev->gravity = 0;
	pev->friction = 1;

	EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/sg_24.wav", 1.0, ATTN_NORM);
}

void CHealCrystal ::Precache()
{
	BeamSprite = PRECACHE_MODEL(BLASTER_BEAM_SPRITE);
	PRECACHE_MODEL("models/crystal.mdl");
	PRECACHE_SOUND("weapons/rocket1.wav");
	m_iSpriteTexture = PRECACHE_MODEL("sprites/shrinkf.spr");
}

void CHealCrystal ::Hit(CBaseEntity *Target)
{
}



CHealCrystal *CHealCrystal ::Create(Vector Pos, Vector Aim, CBaseEntity *Owner)
{
	CHealCrystal *Beam = GetClassPtr((CHealCrystal *)NULL);

	UTIL_SetOrigin(Beam->pev, Pos);
	Beam->pev->angles = Aim;
	Beam->Spawn();
	Beam->SetTouch(CHealCrystal ::Hit);
	Beam->pev->owner = Owner->edict();
	return Beam;
}

void CHealCrystal ::MoveThink()
{
	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 250.0)) != NULL)
	{
		if (pEntity->IsAlive() && (pEntity->IsPlayer() || pEntity->IsMonster()) && (FVisible(pEntity))) /// check only players
		{
			pEntity->TakeHealth(pev->dmg * 10.0, DMG_GENERIC); // give health all around

			//if (pEntity->pev->max_health < 500.0)
				pEntity->pev->max_health += 1.0;
			if (pEntity->pev->armorvalue < 100.0)
				pEntity->pev->armorvalue += 1.0;
			if (pEntity->pev->fuser1 < 100.0)
				pEntity->pev->fuser1 += 1.0;
		}
	}

	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/crystal_heal.wav", 0.15, ATTN_NORM); // play sound
	

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_BEAMCYLINDER);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z + 550); // reach damage radius over .2 seconds
		WRITE_SHORT(m_iSpriteTexture);
		WRITE_BYTE(0);	 // startframe
		WRITE_BYTE(0);	 // framerate
		WRITE_BYTE(3);	 // life
		WRITE_BYTE(50);	 // width
		WRITE_BYTE(0);	 // noise
		WRITE_BYTE(0);	 // r, g, b
		WRITE_BYTE(50);	 // r, g, b
		WRITE_BYTE(50);	 // r, g, b
		WRITE_BYTE(192); // brightness
		WRITE_BYTE(0);	 // speed
	MESSAGE_END();

	// lights
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_BYTE( 25 );		// radius * 0.1
		WRITE_BYTE(0);	 // r, g, b
		WRITE_BYTE(54);	 // r, g, b
		WRITE_BYTE(16);	 // r, g, b
		WRITE_BYTE( 128 );		// life * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );

	if (gpGlobals->time >= m_flDie)
	{
		// lots of smoke
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z - 32);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(55); // scale * 10
			WRITE_BYTE(1);	// framerate
		MESSAGE_END();

		SetThink(SUB_Remove);
	}
	
	pev->nextthink = gpGlobals->time + 1.0;
}

////////////////////////
////////////////////////
void Jumping_Satchel ::Spawn(void)
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	pev->body = 1;

	SET_MODEL(ENT(pev), "models/snapbug.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4)); // Uses point-sized, and can be stepped over
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(SatchelSlide);
	SetUse(DetonateUse2);
	SetThink(SatchelThink);
	pev->nextthink = gpGlobals->time + 0.9;
	pev->classname = MAKE_STRING("monster_satchel");

	pev->gravity = 0.5;
	pev->friction = 0.5;

	pev->dmg = 250.0;
	pev->sequence = 1;

	pev->health = 35.0;
	pev->takedamage = DAMAGE_YES;
}

void Jumping_Satchel::SatchelSlide(CBaseEntity *pOther)
{
	entvars_t *pevOther = pOther->pev;

	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	pev->gravity = 1; // normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin - Vector(0, 0, 10), ignore_monsters, edict(), &tr);

	if (tr.flFraction < 1.0)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
	}

	StudioFrameAdvance();
}

void Jumping_Satchel ::SatchelThink(void)
{

	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->velocity.Length2D() < 3 && (pev->flags & FL_ONGROUND))
	{

		pev->velocity.x += RANDOM_FLOAT(-450, 450);
		pev->velocity.y += RANDOM_FLOAT(-450, 450);
		pev->velocity.z += 160;
	}

	// angle
	switch (RANDOM_LONG(0, 1))
	{
	case 0:
	{
		pev->avelocity.y += 90;
		pev->angles.y += 90;
		break;
	}
	case 1:
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/pl_gun1.wav", 0.35, ATTN_NORM, 1.0, RANDOM_LONG(85, 100));
		pev->avelocity.y -= 90;
		pev->angles.y -= 90;
		break;
	}
	}

	pev->nextthink = gpGlobals->time + 1.5;
}

////////////////////////////
////////////////////////////

void TriipleDamage_Crystal::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/crystal3.mdl");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-6, -6, -6), Vector(6, 6, 6));
	UTIL_SetOrigin(pev, pev->origin);
	pev->gravity = 0.0;
	m_flDie = 600;

	pev->dmg = 100.0;

	pev->avelocity.y = -250;

	pev->rendermode = kRenderTransAdd; // kRenderTransAlpha
	pev->renderamt = 200;

	pev->classname = MAKE_STRING("weapon_satchel");

	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);
	m_LaserSprite = PRECACHE_MODEL("sprites/xbeam4.spr");

	// SetTouch( MoveTouch );
	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 2.0;
}

void TriipleDamage_Crystal::MoveThink(void)
{
	if (pev->rendermode != kRenderNormal)
	{
		pev->rendermode = kRenderNormal;
		pev->movetype = MOVETYPE_TOSS;
	}

	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 286)) != NULL)
	{

		if ((FVisible(pEntity)))
		{
			{
				if ((pEntity->edict() != edict()) && (pEntity->IsAlive()) && (pEntity->TripleShot == 1) && (pEntity->pev->flags & (FL_CLIENT))) // !(pEntity->pev->flags & (FL_MONSTER|FL_CLIENT))
				{

					// shock ray 1
					MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
					WRITE_BYTE(TE_BEAMPOINTS);
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z + 70);
					WRITE_COORD(pEntity->pev->origin.x); // tr.vecEndPos.
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_SHORT(m_LaserSprite); // sprite
					WRITE_BYTE(0);				// Starting frame
					WRITE_BYTE(0);				// framerate * 0.1
					WRITE_BYTE(8);				// life * 0.1
					WRITE_BYTE(8);				// width
					WRITE_BYTE(0);				// noise
					WRITE_BYTE(220);			// color r,g,b
					WRITE_BYTE(220);			// color r,g,b
					WRITE_BYTE(200);			// color r,g,b
					WRITE_BYTE(200);			// brightness
					WRITE_BYTE(0);				// scroll speed
					MESSAGE_END();

					MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
					WRITE_BYTE(TE_BEAMTORUS);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z);
					WRITE_COORD(pEntity->pev->origin.x);
					WRITE_COORD(pEntity->pev->origin.y);
					WRITE_COORD(pEntity->pev->origin.z - 200); // reach damage radius over .2 seconds
					WRITE_SHORT(m_LaserSprite);
					WRITE_BYTE(0);	 // startframe
					WRITE_BYTE(0);	 // framerate
					WRITE_BYTE(4);	 // life
					WRITE_BYTE(12);	 // width
					WRITE_BYTE(0);	 // noise
					WRITE_BYTE(220); // r, g, b
					WRITE_BYTE(200); // r, g, b
					WRITE_BYTE(200); // r, g, b
					WRITE_BYTE(240); // brightness
					WRITE_BYTE(0);	 // speed
					MESSAGE_END();

					EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build1.wav", 1, ATTN_NORM, 0, RANDOM_LONG(100, 180));
					pEntity->TripleShot = 3;  // enable X dmg for entities
				}
			}
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
	m_flDie -= 1;

	if (m_flDie <= 0)
	{
		UTIL_Remove(this);
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, pev->dmg, CLASS_NONE, DMG_POISON);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); // play sound

		// explosions
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

void TRocket::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/alt_nuke2.mdl");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, Vector(-6, -6, -6), Vector(6, 6, 6));
	UTIL_SetOrigin(pev, pev->origin);

	pev->gravity = 0.0;
	m_flDie = 36;

	pev->dmg = 172.0;

	pev->rendermode = kRenderTransAdd; // kRenderTransAlpha
	pev->renderamt = 200.0;

	pev->classname = MAKE_STRING("weapon_tripmine");

	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);
	SetTouch(MoveTouch);
	m_LaserSprite = PRECACHE_MODEL("sprites/xbeam4.spr");

	SetThink(MoveThink);
	pev->nextthink = gpGlobals->time + 1.0;
}

void TRocket::MoveThink(void)
{
	if (pev->rendermode != kRenderNormal)
	{
		UTIL_MakeVectors(pev->angles);
		Vector vecThrow = gpGlobals->v_up * 256;
		pev->velocity = vecThrow;
		pev->rendermode = kRenderNormal;
		pev->angles = UTIL_VecToAngles(pev->velocity);
	}

	// mega hardcoreeed
	if (m_flDie > 26)
	{
		UTIL_MakeVectors(pev->angles);

		Vector vecSrc = pev->origin;
		Vector vecAiming = gpGlobals->v_forward + Vector(0, 0, -45); // -gpGlobals->v_forward + Vector(0,-45,0);
		TraceResult tr;

		UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 8000, dont_ignore_monsters, ENT(pev), &tr);

		CBaseEntity *pEnt = CBaseEntity::Create("monster_tripmine", tr.vecEndPos + tr.vecPlaneNormal * 8, Vector(pev->angles.x + 45, pev->angles.y, pev->angles.z), pev->owner);
		pEnt->pev->rendermode = kRenderTransColor;
		pEnt->pev->rendercolor.x = 250;
		pEnt->pev->rendercolor.y = 200;
		pEnt->pev->rendercolor.z = 200;
		pEnt->pev->renderamt = 255;
		// pEnt->SetThink( SUB_Remove );
		// pEnt->pev->takedamage = DAMAGE_NO;
		// pEnt->pev->nextthink = gpGlobals->time + 20.0;
	}

	pev->nextthink = gpGlobals->time + 0.3;
	m_flDie -= 1;

	if (m_flDie <= 0)
	{
		UTIL_Remove(this);
		::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, pev->dmg * 3, CLASS_NONE, DMG_POISON);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); // play sound

		// explosions
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

		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_LARGEFUNNEL);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(g_sModelIndexFireball); // model
		WRITE_SHORT(RANDOM_LONG(0, 10));
		MESSAGE_END();
	}
}

void TRocket::MoveTouch(CBaseEntity *pOther)
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
	WRITE_SHORT(g_sModelIndexFireball);
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

	// play sound
	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM);	// play sound
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM); // play sound

	::RadiusDamage(pev->origin, pev, VARS(pev->owner), pev->dmg, pev->dmg * 5, CLASS_NONE, DMG_ENERGYBEAM);

	SetTouch(NULL);
	UTIL_Remove(this);
}

LINK_ENTITY_TO_CLASS(weapon_satchel, CSatchel);
LINK_ENTITY_TO_CLASS(monster_pipebomb, Jumping_Satchel);
LINK_ENTITY_TO_CLASS(monster_satchel, CSatchelCharge);
LINK_ENTITY_TO_CLASS(info_airstrike_node, TRocket);
LINK_ENTITY_TO_CLASS(heal_crystal, CHealCrystal);
LINK_ENTITY_TO_CLASS(weapon_bola, TriipleDamage_Crystal);
