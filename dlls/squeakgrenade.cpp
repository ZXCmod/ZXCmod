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
#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

enum w_squeak_e {
	WSQUEAK_IDLE1 = 0,
	WSQUEAK_FIDGET,
	WSQUEAK_JUMP,
	WSQUEAK_RUN,
};

enum squeak_e {
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};


///class Dcrystal
class   Dcrystal : public CBaseEntity
{
        public:

        void    	Spawn           	   ( void );
        void    	EXPORT MoveThink       ( void );
		// void 		EXPORT MoveTouch	   ( CBaseEntity *pOther );
		
		private:
        short	m_LaserSprite;
		int 	m_iSpriteTexture;
		int 	m_flDie;
		// float 	m_flDie2;
};
LINK_ENTITY_TO_CLASS( weapon_aicore, Dcrystal );




#ifndef CLIENT_DLL
class CSqueakGrenade : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	int  Classify( );
	void EXPORT SuperBounceTouch( CBaseEntity *pOther );
	void EXPORT HuntThink( void );
	int  BloodColor( void ) { return BLOOD_COLOR_YELLOW; }
	void Killed( entvars_t *pevAttacker, int iGib );
	void GibMonster( void );

	static float m_flNextBounceSoundTime;

	// CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	int  m_iMyClass;
};



class CSqueakGrenade2 : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	int  Classify( );
	void EXPORT SuperBounceTouch( CBaseEntity *pOther );
	void EXPORT HuntThink( void );
	void EXPORT ParasiteThink( void );
	int  BloodColor( void ) { return BLOOD_COLOR_YELLOW; }
	void Killed( entvars_t *pevAttacker, int iGib );
	void GibMonster( void );


	static float m_flNextBounceSoundTime;

	// CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	CBaseEntity *m_hEnemy;
	int  m_iMyClass;
};









float CSqueakGrenade::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS( monster_snark, CSqueakGrenade );


#define SQUEEK_DETONATE_DELAY	9.0

int CSqueakGrenade :: Classify ( )
{
	if (m_iMyClass != 0)
		return m_iMyClass; // protect against recursion

	if (m_hEnemy != NULL)
	{
		m_iMyClass = CLASS_INSECT; // no one cares about it
		switch( m_hEnemy->Classify( ) )
		{
			case CLASS_PLAYER:
			case CLASS_HUMAN_PASSIVE:
			case CLASS_HUMAN_MILITARY:
				m_iMyClass = 0;
				return CLASS_ALIEN_MILITARY; // barney's get mad, grunts get mad at it
		}
		m_iMyClass = 0;
	}

	return CLASS_ALIEN_BIOWEAPON;
}

void CSqueakGrenade :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_squeak.mdl");
	UTIL_SetSize(pev, Vector( -3, -3, 0), Vector(4, 4, 5));
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( SuperBounceTouch );
	SetThink( HuntThink );
	pev->nextthink = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time + 1E6;



	m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;

	m_flFieldOfView = 0; // 180 degrees

	if ( pev->owner )
		m_hOwner = Instance( pev->owner );

	m_flNextBounceSoundTime = gpGlobals->time;// reset each time a snark is spawned.

	pev->sequence = WSQUEAK_RUN;
	ResetSequenceInfo( );
	
	//pev->flags |= FL_MONSTER;
	pev->takedamage		= 10;
	pev->health			= 10;
	pev->gravity		= 0.35;
	pev->friction		= 0.4;
	
	// +BubbleMod (thanks)

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 16 ); // life
		WRITE_BYTE( 1.5 ); // width
		WRITE_BYTE( 255 ); // r, g, b
		WRITE_BYTE( 255 ); // r, g, b
		WRITE_BYTE( 255 ); // r, g, b
		WRITE_BYTE( 16 ); // brightness

		MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	// -BubbleMod	
}
	
	
	
	
	
	
	


void CSqueakGrenade::Precache( void )
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
	PRECACHE_MODEL( "sprites/laserbeam.spr" );
	
}


void CSqueakGrenade :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->model = iStringNull;// make invisible
	SetThink( SUB_Remove );
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 0.1;

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	pev->takedamage = DAMAGE_NO;

	// play squeek blast
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "squeek/sqk_blast1.wav", 1, 0.5, 0, RANDOM_LONG(95,100));	

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, SMALL_EXPLOSION_VOLUME, 3.0 );

	UTIL_BloodDrips( pev->origin, g_vecZero, BloodColor(), 80 );

	// if (m_hOwner != NULL)
		// RadiusDamage ( pev, m_hOwner->pev, pev->dmg, CLASS_NONE, DMG_POISON );
	// else
		// RadiusDamage ( pev, pev, pev->dmg, CLASS_NONE, DMG_POISON );

	// reset owner so death message happens
	if (m_hOwner != NULL)
		pev->owner = m_hOwner->edict();

	//CBaseMonster :: Killed( pevAttacker, GIB_ALWAYS );
}

void CSqueakGrenade :: GibMonster( void )
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);		
}



void CSqueakGrenade::HuntThink( void )
{
	// ALERT( at_console, "think\n" );

	if (!IsInWorld())
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}
	
	
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize( );
		pev->health = -1;
		Killed( pev, 0 );
		return;
	}

	// float
	if (pev->waterlevel != 0)
	{
		if (pev->movetype == MOVETYPE_BOUNCE)
		{
			pev->movetype = MOVETYPE_FLY;
		}
		pev->velocity = pev->velocity * 0.5;
		pev->velocity.z += 3.0;
	}
	else if (pev->movetype = MOVETYPE_FLY)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}

	// return if not time to hunt
	if (m_flNextHunt > gpGlobals->time)
		return;

	m_flNextHunt = gpGlobals->time + 2.0;
	//    m_hEnemy->edict() != pev->owner
	
	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize( );

	UTIL_MakeVectors( pev->angles );

	if (m_hEnemy == NULL || !m_hEnemy->IsAlive())
	{
		
		// find target, bounce a bit towards it.
		Look( 512 );
		m_hEnemy = BestVisibleEnemy( );
	}


	// squeek if it's about time blow up
	if ((m_flDie - gpGlobals->time <= 0.5) && (m_flDie - gpGlobals->time >= 0.3))
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_die1.wav", 1, ATTN_NORM, 0, 100 + RANDOM_LONG(0,0x3F));
		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 256, 0.25 );
	}

	// higher pitch as squeeker gets closer to detonation time
	float flpitch = 155.0 - 70.0 * ((m_flDie - gpGlobals->time) / SQUEEK_DETONATE_DELAY);
	if (flpitch < 80)
		flpitch = 80;

	if (m_hEnemy != NULL)
	{
	
		if (FVisible( m_hEnemy ))
		{
			vecDir = m_hEnemy->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize( );
		}

		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;
		
		// ALERT( at_console, "think : enemy\n");

		// ALERT( at_console, "%.0f %.2f %.2f %.2f\n", flVel, m_vecTarget.x, m_vecTarget.y, m_vecTarget.z );

		pev->velocity = pev->velocity * flAdj + m_vecTarget * 250;
	}

	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector( 0, 0, 0 );
	}
	else
	{
		if (pev->avelocity == Vector( 0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT( -100, 100 );
			pev->avelocity.z = RANDOM_FLOAT( -100, 100 );
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT( -100, 100 );
		pev->velocity.y = RANDOM_FLOAT( -100, 100 );
	}
	m_posPrev = pev->origin;

	pev->angles = UTIL_VecToAngles( pev->velocity );
	pev->angles.z = 0;
	pev->angles.x = 0;
}


void CSqueakGrenade::SuperBounceTouch( CBaseEntity *pOther )
{
	float	flpitch;

	TraceResult tr = UTIL_GetGlobalTrace( );

	// don't hit the guy that launched this grenade
	if ( pev->owner && pOther->edict() == pev->owner )
		return;

	// at least until we've bounced once
	pev->owner = NULL;

	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	// higher pitch as squeeker gets closer to detonation time
	flpitch = 175.0 - 60.0 * ((m_flDie - gpGlobals->time) / SQUEEK_DETONATE_DELAY);

	if ( pOther->pev->takedamage && m_flNextAttack < gpGlobals->time )
	{
		// attack!

		// make sure it's me who has touched them
		if (tr.pHit == pOther->edict())
		{
			// and it's not another squeakgrenade
			if (tr.pHit->v.modelindex != pev->modelindex)
			{
				// ALERT( at_console, "hit enemy\n");
				ClearMultiDamage( );
				pOther->TraceAttack(pev, RANDOM_LONG(25,35), gpGlobals->v_forward, &tr, DMG_POISON ); 
				if (m_hOwner != NULL)
					ApplyMultiDamage( pev, m_hOwner->pev );
				else
					ApplyMultiDamage( pev, pev );

				g_vecAttackDir = pev->velocity.Normalize( );
				pev->health = -1;
				Killed( pev, 0 );

				// make bite sound
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "squeek/sqk_deploy1.wav", 1.0, ATTN_NORM, 0, (int)flpitch);
				m_flNextAttack = gpGlobals->time + 0.5;
				
			}
		}

	}

	m_flNextHit = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time;

	if ( g_pGameRules->IsMultiplayer() )
	{
		// in multiplayer, we limit how often snarks can make their bounce sounds to prevent overflows.
		if ( gpGlobals->time < m_flNextBounceSoundTime )
		{
			// too soon!
			return;
		}
	}

	if (!(pev->flags & FL_ONGROUND))
	{
		// play bounce sound
		float flRndSound = RANDOM_FLOAT ( 0.0 , 1.0 );

		if ( flRndSound <= 0.33 )
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt1.wav", 1, ATTN_NORM, 0, (int)flpitch);		
		else if (flRndSound <= 0.66)
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, (int)flpitch);
		else 
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, (int)flpitch);
		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 256, 0.25 );
	}
	else
	{
		// skittering sound
		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 100, 0.1 );
	}

	m_flNextBounceSoundTime = gpGlobals->time + 0.5;// half second.
}

#endif

LINK_ENTITY_TO_CLASS( weapon_snark, CSqueak );


void CSqueak::Spawn( )
{
	Precache( );
	m_iId = WEAPON_SNARK;
	SET_MODEL(ENT(pev), "models/w_sqknest.mdl");

	FallInit();//get ready to fall down.

	m_iDefaultAmmo = SNARK_DEFAULT_GIVE;
		
	pev->sequence = 1;
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0;
	m_flNextChatTime16 = gpGlobals->time; //start timer
}


void CSqueak::Precache( void )
{
	PRECACHE_MODEL("models/w_sqknest.mdl");
	PRECACHE_MODEL("models/v_squeak.mdl");
	PRECACHE_MODEL("models/p_squeak.mdl");
	PRECACHE_SOUND("squeek/sqk_hunt2.wav");
	PRECACHE_SOUND("squeek/sqk_hunt3.wav");
	PRECACHE_SOUND("garg/gar_stomp1.wav");
	UTIL_PrecacheOther("monster_snark");
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );

	m_usSnarkFire = PRECACHE_EVENT ( 1, "events/snarkfire.sc" );
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



BOOL CSqueak::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 270 );
	// play hunt sound
	float flRndSound = RANDOM_FLOAT ( 0.0 , 1.0 );

	if ( flRndSound <= 0.5 )
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 100);
	else 
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 100);

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	return DefaultDeploy( "models/v_squeak.mdl", "models/p_squeak.mdl", SQUEAK_UP, "squeak" );
}


void CSqueak::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	if ( !m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_SNARK);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	
	SendWeaponAnim( SQUEAK_DOWN );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CSqueak::FourthAttack( void )
{
	if (allowmonsters9.value == 0)
		return;

	if (  m_pPlayer->m_flNextChatTime16 < gpGlobals->time )
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 18)
		{
				Vector vecThrow = gpGlobals->v_forward * 50;
				
				// #ifndef CLIENT_DLL
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=18;
				CBaseEntity *pCr = Create( "weapon_aicore", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, Vector(0,0,0), m_pPlayer->edict() );
				pCr->pev->velocity = vecThrow;
				SendWeaponAnim( 5 );
				m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.08; //0.08
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.08;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.08;
				m_pPlayer->m_flNextChatTime16 = gpGlobals->time + 90;
				// #endif

				// EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/2plasma_fire1.wav", 1.0, ATTN_NORM); //play sound
		}
	}
}

void CSqueak::PrimaryAttack()
{
	if (allowmonsters10.value == 1)
		{
			ThirdAttack();
			return;
		}
	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] >= 1 )
	{
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		TraceResult tr;
		Vector trace_origin;

		// HACK HACK:  Ugly hacks to handle change in origin based on new physics code for players
		// Move origin up if crouched and start trace a bit outside of body ( 20 units instead of 16 )
		trace_origin = m_pPlayer->pev->origin;
		if ( m_pPlayer->pev->flags & FL_DUCKING )
		{
			trace_origin = trace_origin - ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
		}

		// find place to toss monster
		UTIL_TraceLine( trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr );

	int flags;
	#ifdef CLIENT_WEAPONS
		flags = FEV_GLOBAL;
	#else
		flags = 0;
	#endif

	    PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSnarkFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

		if ( tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > 0.25 )
		{
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	#ifndef CLIENT_DLL
			CBaseEntity *pSqueak = CBaseEntity::Create( "monster_snark", tr.vecEndPos, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSqueak->pev->velocity = gpGlobals->v_forward * 400 + m_pPlayer->pev->velocity;
	#endif

			// play hunt sound
			float flRndSound = RANDOM_FLOAT ( 0.0 , 1.0 );

			if ( flRndSound <= 0.5 )
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 97);
			else 
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 103);

			m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=1;

			m_fJustThrown = 1;

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		}
	}	
}


void CSqueak::SecondaryAttack( void )
{
	if (allowmonsters9.value == 0)
		return;

// return;
	if (allowmonsters10.value == 1)
		{
			return;
		}
	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] >= 3 )
	{
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		TraceResult tr;
		Vector trace_origin;

		// HACK HACK:  Ugly hacks to handle change in origin based on new physics code for players
		// Move origin up if crouched and start trace a bit outside of body ( 20 units instead of 16 )
		trace_origin = m_pPlayer->pev->origin;
		if ( m_pPlayer->pev->flags & FL_DUCKING )
		{
			trace_origin = trace_origin - ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
		}

		// find place to toss monster
		UTIL_TraceLine( trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr );

	    PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usSnarkFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

		if ( tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > 0.25 )
		{
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

#ifndef CLIENT_DLL
			CBaseEntity *pSqueak = CBaseEntity::Create( "monster_larve", tr.vecEndPos, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSqueak->pev->velocity = gpGlobals->v_forward * 160 + m_pPlayer->pev->velocity;
#endif

			// play hunt sound
			float flRndSound = RANDOM_FLOAT ( 0 , 1 );

			if ( flRndSound <= 0.5 )
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt2.wav", 1, ATTN_NORM, 0, 105);
			else 
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "squeek/sqk_hunt3.wav", 1, ATTN_NORM, 0, 105);

			m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=3;

			m_fJustThrown = 1;

			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1;
		}
	}
}

void CSqueak::ThirdAttack( void )
{
	if (allowmonsters9.value == 0)
		return;

	if (allowmonsters10.value == 0)
		{
			pev->dmg = 55;
		}
	if (allowmonsters10.value == 1)
		{
			pev->dmg = 60;
		}
		
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1)
	{
	CBaseEntity *pEntity = NULL;

	
	
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "garg/gar_stomp1.wav", 1.0, ATTN_NORM);
	


	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 1;
	
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;

	
	m_pPlayer->pev->velocity.z = 340;

	

	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 130 )) != NULL)
       		 {
				if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict())) //!(pEntity->pev->movetype == MOVETYPE_FLY)
					{
						UTIL_ScreenShake( pEntity->pev->origin, 1024.0, 1.5, 0.7, 1 );
						if (FVisible( pEntity ))
							pEntity->TakeDamage(pev, VARS( pev->owner ), pev->dmg, DMG_POISON); //destroy all near thinks
					} 
			}
		//beam
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 300 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 16 ); // framerate
			WRITE_BYTE( 6 ); // life
			WRITE_BYTE( 10 );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 200 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( 155 ); // brightness
			WRITE_BYTE( 4 );		// speed
		MESSAGE_END();

	return;
	
	}

}






void CSqueak::WeaponIdle( void )
{



	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	if (m_fJustThrown)
	{
		m_fJustThrown = 0;

		if ( !m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] )
		{
			RetireWeapon();
			return;
		}

		SendWeaponAnim( SQUEAK_UP );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		return;
	}

	int iAnim;
	float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
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
	SendWeaponAnim( iAnim );
}

#endif





















////new


float CSqueakGrenade2::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS( monster_larve, CSqueakGrenade2 );

#define SQUEEK_DETONATE_DELAY2	60.0 // remover time 

int CSqueakGrenade2 :: Classify ( )
{
	if (m_iMyClass != 0)
		return m_iMyClass; // protect against recursion

	if (m_hEnemy != NULL)
	{
		m_iMyClass = CLASS_INSECT; // no one cares about it
		switch( m_hEnemy->Classify( ) )
		{
			case CLASS_PLAYER:
			case CLASS_HUMAN_PASSIVE:
			case CLASS_HUMAN_MILITARY:
				m_iMyClass = 0;
				return CLASS_ALIEN_MILITARY; // barney's get mad, grunts get mad at it
		}
		m_iMyClass = 0;
	}

	return CLASS_ALIEN_BIOWEAPON;
}

void CSqueakGrenade2 :: Spawn( void )
{
	Precache( );
	// motor
	pev->classname = MAKE_STRING( "monster_snark" );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_squeak.mdl");
	UTIL_SetSize(pev, Vector( -4, -4, 0), Vector(4, 4, 8));
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( SuperBounceTouch );
	SetThink( HuntThink );
	pev->nextthink = gpGlobals->time + 0.05;
	m_flNextHunt = gpGlobals->time + 1E6;
	
	Classify2 = Classify();



	m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY2;

	m_flFieldOfView = 0; // 180 degrees

	if ( pev->owner )
		m_hOwner = Instance( pev->owner );

	m_flNextBounceSoundTime = gpGlobals->time;// reset each time a snark is spawned.

	pev->sequence = WSQUEAK_RUN;
	ResetSequenceInfo( );
	
	//pev->flags |= FL_MONSTER;
	pev->takedamage		= DAMAGE_YES;
	pev->health			= SQUEEK_DETONATE_DELAY2;
	pev->gravity		= 0.2;
	pev->friction		= 0.1;
	

	pev->dmg = 20;
	// pev->ltime = 0;
	
	m_hEnemy=NULL;



	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 30 ); // life
		WRITE_BYTE( 1.6 ); // width
		WRITE_BYTE( 1 ); // r, g, b
		WRITE_BYTE( 55 ); // r, g, b
		WRITE_BYTE( 1 ); // r, g, b
		WRITE_BYTE( 100 ); // brightness
	MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 20 ); // life
		WRITE_BYTE( 2.6 ); // width
		WRITE_BYTE( 60 ); // r, g, b
		WRITE_BYTE( 125 ); // r, g, b
		WRITE_BYTE( 1 ); // r, g, b
		WRITE_BYTE( 87 ); // brightness
	MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)


}
	
	
	
	
	
	
	


void CSqueakGrenade2::Precache( void )
{

}


void CSqueakGrenade2 :: Killed( entvars_t *pevAttacker, int iGib )
{


	pev->model = iStringNull;// make invisible
	SetThink( SUB_Remove );
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 0.2;
	

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	pev->takedamage = DAMAGE_NO;

	// play squeek blast
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "squeek/sqk_blast1.wav", 1, 0.5, 0, PITCH_NORM);	

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, SMALL_EXPLOSION_VOLUME, 3.0 );

	UTIL_BloodDrips( pev->origin, g_vecZero, BloodColor(), 80 );

	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 30, 50, CLASS_NONE, DMG_NERVEGAS  ); //end blast	
		
	// reset owner so death message happens
	if (m_hOwner != NULL)
		pev->owner = m_hOwner->edict();

	//CBaseMonster :: Killed( pevAttacker, GIB_ALWAYS );
}

void CSqueakGrenade2 :: GibMonster( void )
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);		
}



void CSqueakGrenade2::HuntThink( void )
{
	if (!IsInWorld())
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}
	
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize( );
		pev->health = -1;
		Killed( pev, 0 );
		return;
	}

	if (pev->waterlevel != 0)
	{
		if (pev->movetype == MOVETYPE_BOUNCE)
		{
			pev->movetype = MOVETYPE_FLY;
		}
		pev->velocity = pev->velocity * 0.5;
		pev->velocity.z += 4.0;
		
		pev->health = -1;
		Killed( pev, 0 );
		return;
	}
	else if (pev->movetype = MOVETYPE_FLY)
	{

		pev->movetype = MOVETYPE_BOUNCE;
	}

	if (m_flNextHunt > gpGlobals->time)
		return;

	m_flNextHunt = gpGlobals->time + 0.05;
	
	CBaseEntity *pOther = NULL;

	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize( );

	UTIL_MakeVectors( pev->angles );

	if (m_hEnemy == NULL || !m_hEnemy->IsAlive())
	{
		Look( 120 );
		m_hEnemy = BestVisibleEnemy( );
	}

	if (m_hEnemy != NULL )
	{
		if (FVisible( m_hEnemy ))
		{
			vecDir = m_hEnemy->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize( );
		}

		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;

		pev->velocity = pev->velocity * flAdj + m_vecTarget * 400;
	}
	

	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector( 0, 0, 0 );
	}
	else
	{
		if (pev->avelocity == Vector( 0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT( -55, 55 );
			pev->avelocity.y = RANDOM_FLOAT( -55, 55 );
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT( -65, 65 );
		pev->velocity.y = RANDOM_FLOAT( -65, 65 );
	}
	m_posPrev = pev->origin;

	pev->angles = UTIL_VecToAngles( pev->velocity );
	pev->angles.z = 0;
	pev->angles.x = 0;
	
}


void CSqueakGrenade2::SuperBounceTouch( CBaseEntity *pOther )
{
	TraceResult tr = UTIL_GetGlobalTrace( );
	
	if ( pev->owner && pOther->edict() == pev->owner )
		return;

	if ( pOther==NULL )
		return;

	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	m_flNextHit = gpGlobals->time + 0.01;
	m_flNextHunt = gpGlobals->time;
	
	if ( pOther->pev->takedamage && pOther->infected == 0) // && m_flNextAttack < gpGlobals->time
	{
		// make sure it's me who has touched them
		if (tr.pHit == pOther->edict())
		{
			// and it's not another squeakgrenade
			if (tr.pHit->v.modelindex != pev->modelindex)
			{

				// attach parasite
				if ( pOther->pev->solid != SOLID_BSP && ( pOther->pev->flags & (FL_MONSTER|FL_CLIENT) ))  //  && pev->ltime == 0
				{
					SetAttachment2( pOther->edict(), 0 );
					UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
					SetTouch(NULL);

					if ( pOther->pev->flags & FL_MONSTER)
					{
						pev->dmg=5;
					}
					else
					{
						//m_flDie -= 1; // decr timer
						pev->dmg=3;
					}
					
					m_hEnemy = pOther; // remember target
					pOther->infected = 1;

					// make bite sound
					EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "squeek/sqk_deploy1.wav", 1.0, ATTN_NORM, 0, 100);
					m_flNextAttack = gpGlobals->time + 1.0;
					SetThink( ParasiteThink );
					pev->nextthink = gpGlobals->time + 1.0;
				}
				
			}
		}
	}
	else if ( pOther->pev->solid != SOLID_BSP) // dont explode with touch
	{

		// find target, bounce a bit towards it.
		//m_hEnemy = NULL;
		m_flDie = -1;
	}


	m_flNextBounceSoundTime = gpGlobals->time + 1.0;// half second.
}

void CSqueakGrenade2::ParasiteThink( void )
{
	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize( );
		pev->health = -1;
		Killed( pev, 0 );
		return;
	}
	
	// if ((!m_hEnemy->IsAlive()) && pev->deadflag == DEAD_DEAD)
	if (m_hEnemy!=NULL && m_hEnemy->pev->deadflag == DEAD_DYING)
	{
		pev->health = -1;
		Killed( pev, 0 );
	}
	if ( m_hEnemy->infected==0 )
		{
			UTIL_Remove( this );
		}
/* 
	if (m_hEnemy==NULL)
	{
		//UTIL_Remove( this );
		SetThink( SUB_Remove );
		pev->nextthink = gpGlobals->time + 1.0;
		pev->movetype = MOVETYPE_FOLLOW;
		m_hEnemy = this;
		// ClearBits (m_hEnemy->pev->flags, FL_MONSTER);
		//return;
		SetAttachment2( NULL, 0 );
	}
	else */
	if (m_hEnemy->pev->rendermode != kRenderTransTexture) //reset values
	{
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 33, CLASS_NONE, DMG_CRUSH  ); //end blast	
		m_flDie -= 2.0;
		//m_hEnemy->pev->dmg = m_hEnemy->pev->dmg*12;
		pev->nextthink = gpGlobals->time + 0.63;
	}
	
}

////////////////////////////
////////////////////////////

void Dcrystal::Spawn( void )
{
	SET_MODEL( ENT(pev), "models/crystal2.mdl" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, -4), Vector(4, 4, 4) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	m_flDie = 90;
	pev->dmg = 100;
	
	pev->avelocity.y = -250;
	
	pev->rendermode = kRenderTransAdd; //kRenderTransAlpha
	pev->renderamt = 200;
	
	pev->classname = MAKE_STRING( "weapon_satchel" );
	
	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/superphys_launch2.wav", 1.0, ATTN_NORM);
	// SetTouch( MoveTouch );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	

	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 4.0; 
}

void Dcrystal::MoveThink( )
{
	if ( pev->rendermode != kRenderNormal )
	{
		pev->rendermode = kRenderNormal;
		pev->movetype = MOVETYPE_TOSS;
	}
	CBaseEntity *pEntity2 = CBaseEntity::Instance(pev->owner);
	CBasePlayer *pPlayer = (CBasePlayer *)pEntity2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 550 ); // reach damage radius over .2 seconds
		WRITE_SHORT( g_sModelIndexSmokeTrail );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 4 ); // life
		WRITE_BYTE( 50 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 50 );   // r, g, b
		WRITE_BYTE( 10 );   // r, g, b
		WRITE_BYTE( 175 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();
	
	// make team color
	int index = g_pGameRules->GetTeamIndex( pPlayer->TeamID() );
	if (g_pGameRules->IsTeamplay() && index == 0)  // Blue
	{
		// pev->rendermode = kRenderNormal;
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor.x = 0;  // red
		pev->rendercolor.y = 0;  // green
		pev->rendercolor.z = 100; // blue
		pev->renderamt = 24;
	}
	if (g_pGameRules->IsTeamplay() && index == 1)  // Red
	{
		// pev->rendermode = kRenderNormal;
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor.x = 100;  // red
		pev->rendercolor.y = 0;  // green
		pev->rendercolor.z = 0; // blue
		pev->renderamt = 24;
	}
	
	EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/crystal_heal.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG(200,240));
	
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 286 )) != NULL)
       	{		
		if ( (FVisible( pEntity )))
			{
				CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
				if ( (g_pGameRules->PlayerRelationship( pOwner, pEntity ) != GR_TEAMMATE))
					{
					if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict()))
						{
						//shock ray 1
						MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
							WRITE_BYTE( TE_BEAMPOINTS );
							WRITE_COORD(pev->origin.x);
							WRITE_COORD(pev->origin.y);
							WRITE_COORD(pev->origin.z+85);
							WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
							WRITE_COORD( pEntity->pev->origin.y );
							WRITE_COORD( pEntity->pev->origin.z );
							WRITE_SHORT( m_LaserSprite ); //sprite
							WRITE_BYTE( 1 ); // Starting frame
							WRITE_BYTE( 0  ); // framerate * 0.1
							WRITE_BYTE( 4 ); // life * 0.1
							WRITE_BYTE( 22 ); // width
							WRITE_BYTE( 8 ); // noise
							WRITE_BYTE( 100 ); // color r,g,b
							WRITE_BYTE( 10 ); // color r,g,b
							WRITE_BYTE( 15 ); // color r,g,b
							WRITE_BYTE( 250 ); // brightness
							WRITE_BYTE( 64 ); // scroll speed
						MESSAGE_END();
						
						//shock ray 2
						MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
							WRITE_BYTE( TE_BEAMPOINTS );
							WRITE_COORD(pev->origin.x);
							WRITE_COORD(pev->origin.y);
							WRITE_COORD(pev->origin.z+85);
							WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
							WRITE_COORD( pEntity->pev->origin.y );
							WRITE_COORD( pEntity->pev->origin.z );
							WRITE_SHORT( m_LaserSprite ); //sprite
							WRITE_BYTE( 1 ); // Starting frame
							WRITE_BYTE( 0  ); // framerate * 0.1
							WRITE_BYTE( 5 ); // life * 0.1
							WRITE_BYTE( 22 ); // width
							WRITE_BYTE( 16 ); // noise
							WRITE_BYTE( 100 ); // color r,g,b
							WRITE_BYTE( 100 ); // color r,g,b
							WRITE_BYTE( 15 ); // color r,g,b
							WRITE_BYTE( 200 ); // brightness
							WRITE_BYTE( 128 ); // scroll speed
						MESSAGE_END();
						
						//lights
						Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
						MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
							WRITE_BYTE(TE_DLIGHT);
							WRITE_COORD(pEntity->pev->origin.x);	// X
							WRITE_COORD(pEntity->pev->origin.y);	// Y
							WRITE_COORD(pEntity->pev->origin.z);	// Z
							WRITE_BYTE( 16 );		// radius * 0.1
							WRITE_BYTE( 200 );		// r
							WRITE_BYTE( 30 );		// g
							WRITE_BYTE( 30 );		// b
							WRITE_BYTE( 128 );		// time * 10
							WRITE_BYTE( 32 );		// decay * 0.1
						MESSAGE_END( );
	
						
						UTIL_ScreenShake( pEntity->pev->origin, 12.0, 90.5, 0.3, 1 );
					
						pEntity->TakeDamage(pev, VARS( pev->owner ), 10, DMG_SHOCK);
						pEntity->pev->armorvalue -= 1;
						pEntity->pev->velocity.z += pEntity->pev->velocity.z + 256;
						//sounds
						switch(RANDOM_LONG(0,8))
						{
							case 0: EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build1.wav", 0.95, ATTN_NORM, 0, RANDOM_LONG(140,180)); break;
							case 3: EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build2.wav", 0.95, ATTN_NORM, 0, RANDOM_LONG(140,180)); break;
							case 5: EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build3.wav", 0.95, ATTN_NORM, 0, RANDOM_LONG(140,180)); break;
							case 8: EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build4.wav", 0.90, ATTN_NORM, 0, RANDOM_LONG(140,180)); break;
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
		UTIL_Remove( this );
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_POISON  ); 
		EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); //play sound
		// random explosions
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 75  ); // scale * 10
			WRITE_BYTE( 10  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}
	
/* 	
	// deleter
	m_timer += 1.5;
	if (m_timer >= pev->dmg * 0.6)
	{
		SetTouch( NULL );
		UTIL_Remove( this );
	}
	 */
}

/* void CGCannon::MoveTouch( CBaseEntity *pOther )
{


} */