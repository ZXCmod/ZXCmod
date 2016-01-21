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
#include "gauss.h"



enum gauss_e {
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



float CGauss::GetFullChargeTime( void )
{

	return 1.5;
	
}

#ifdef CLIENT_DLL
	extern int g_irunninggausspred;
#endif


void CGauss::Spawn( )
{
	Precache( );
	m_iId = WEAPON_GAUSS;
	SET_MODEL(ENT(pev), "models/w_gauss.mdl");

	m_iDefaultAmmo = GAUSS_DEFAULT_GIVE;
	m_flNextChatTime3 = gpGlobals->time + 15; // prevent connect/disconnect flood

	FallInit();
}

void CGauss::Precache( void )
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
	
	
	
	
	m_iGlow = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBeam = PRECACHE_MODEL( "sprites/smoke.spr" );

	m_usGaussFire = PRECACHE_EVENT( 1, "events/gauss.sc" );
	m_usGaussSpin = PRECACHE_EVENT( 1, "events/gaussspin.sc" );
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");
	PRECACHE_MODEL("sprites/shrinkf.spr");
	PRECACHE_MODEL( "sprites/zbeam1.spr" );
	m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/cexplo.spr" );
	
	PRECACHE_MODEL( "sprites/nhth1.spr" );
	PRECACHE_MODEL( "sprites/yelflare1.spr" );
	PRECACHE_MODEL( "sprites/richo2.spr" ); //explode
	PRECACHE_MODEL( "sprites/zbeam4.spr" );
	PRECACHE_MODEL( "sprites/cexplo.spr" );
	PRECACHE_MODEL( "models/metalplategibs.mdl" );

}

int CGauss::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
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

BOOL CGauss::Deploy( )
{
	m_pPlayer->m_flPlayAftershock = 0.0;
	return DefaultDeploy( "models/v_gauss.mdl", "models/p_gauss.mdl", GAUSS_DRAW, "gauss" );
            //SetThink(Spawn2);

}

void CGauss::Holster( int skiplocal /* = 0 */ )
{
	PLAYBACK_EVENT_FULL( FEV_GLOBAL | FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );
	
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( GAUSS_HOLSTER );
	m_fInAttack = 0;

}


void CGauss::PrimaryAttack()
{
if (allowmonsters10.value == 0)
	{
		// don't fire underwater
		if ( m_pPlayer->pev->waterlevel == 3 )
		{
			PlayEmptySound( );
			m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
			return;
		}

		
		
		if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] < 2 )
		{
			PlayEmptySound( );
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
		m_fPrimaryFire = TRUE;
		m_fFourthFire = FALSE;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1; 

		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.2;
	}
	else
		ThirdAttack();
}

void CGauss::SecondaryAttack()
{
	float m_b, m_t;
	if (allowmonsters10.value == 1)
	{
		m_b = 1200;
		m_t = 7;
	}
	else
	{
		m_b = 900;
		m_t = 27;
	}
	
		// don't fire underwater
		if ( m_pPlayer->pev->waterlevel == 3 )
		{
			if ( m_fInAttack != 0 )
			{
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
				SendWeaponAnim( GAUSS_IDLE );
				m_fInAttack = 0;
			}
			else
			{
				PlayEmptySound( );
			}

			m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		//*
		if ( m_fInAttack == 0 )
		{
			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			{
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
				m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
				return;
			}
			// fire
			m_fPrimaryFire = FALSE;
			m_fFourthFire = FALSE;

			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
			m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

			// spin up
			m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
			
			SendWeaponAnim( GAUSS_SPINUP );
			m_fInAttack = 1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
			m_pPlayer->m_flStartCharge = gpGlobals->time;
			m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

			PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0 );

			m_iSoundState = SND_CHANGE_PITCH;
		}
		
		else if (m_fInAttack == 1)
		{
			if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
			{
				SendWeaponAnim( GAUSS_SPIN );
				m_fInAttack = 2;
			}
		}
		else
		{
			if (allowmonsters10.value == 1)
			{
				Vector vecDir = gpGlobals->v_forward;
				m_pPlayer->pev->velocity = vecDir*m_pPlayer->m_flNextAmmoBurn;

			}
			// during the charging process, eat one bit of ammo every once in a while
			if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != m_b )
			{

				{
					m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
					m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
				}

			}

			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			{
				// out of ammo! force the gun to fire
				StartFire();
				m_fInAttack = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
				m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
				return;
			}
			
			if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
			{
				// don't eat any more ammo after gun is fully charged.
				m_pPlayer->m_flNextAmmoBurn = m_b;
			}

			int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 150 / GetFullChargeTime() ) + 100;
			if ( pitch > 270 ) 
				 pitch = 270;

			PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );
			m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
			m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
			
		}

}


//// magnet ray ////

void CGauss::ThirdAttack( void )
{
	CBaseEntity *pEntity;
	TraceResult	tr;	
	Vector vecSrc;
	vecSrc = m_pPlayer->GetGunPosition( );
	Vector vecDir = gpGlobals->v_forward;
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 4096, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	pEntity = CBaseEntity::Instance(tr.pHit); //trace hit to entity

	if (pEntity != NULL && pEntity->pev->takedamage && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 1) //  && pEntity->IsPlayer()
    {
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 0.55, ATTN_NORM, 0, RANDOM_LONG(130,160));

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
            WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(vecSrc.x);
            WRITE_COORD(vecSrc.y);
            WRITE_COORD(vecSrc.z);
            WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
            WRITE_COORD( pEntity->pev->origin.y );
            WRITE_COORD( pEntity->pev->origin.z );
            WRITE_SHORT( BSpr ); //sprite
            WRITE_BYTE( 0 ); // Starting frame
            WRITE_BYTE( 0  ); // framerate * 0.1
            WRITE_BYTE( 3 ); // life * 0.1
            WRITE_BYTE( 12 ); // width
            WRITE_BYTE( 15 ); // noise
            WRITE_BYTE( 125 ); // color r,g,b
            WRITE_BYTE( 65 ); // color r,g,b
            WRITE_BYTE( 10 ); // color r,g,b
            WRITE_BYTE( 175 ); // brightness
            WRITE_BYTE( 100 ); // scroll speed
		MESSAGE_END();
			
		pEntity->pev->punchangle.z = RANDOM_LONG(-45,30);
		pEntity->pev->punchangle.x = RANDOM_LONG(-15,45);
		pEntity->TakeDamage(pev, VARS( pev->owner ), 15, DMG_FALL);
		
		// fix cheat in teamplay
		CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
		if ( (g_pGameRules->PlayerRelationship( pOwner, pEntity ) != GR_TEAMMATE) && m_pPlayer->pev->health <= 150 && pEntity->IsPlayer())
			m_pPlayer->pev->health += 15; //take unlimited health

		// pEntity->pev->velocity.x = ( ( m_pPlayer->pev->velocity.x + m_pPlayer->pev->origin.x) - pEntity->pev->origin.x);
		// pEntity->pev->velocity.y = ( ( m_pPlayer->pev->velocity.y + m_pPlayer->pev->origin.y) - pEntity->pev->origin.y);
		// pEntity->pev->velocity.z = ( ( m_pPlayer->pev->velocity.z + m_pPlayer->pev->origin.z) - pEntity->pev->origin.z);
		// pEntity->pev->velocity = m_pPlayer->pev->velocity + gpGlobals->v_forward  * -220; //new code
		pEntity->pev->velocity = pEntity->pev->velocity * 0.1; //newer code
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 5; 
	
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		UTIL_Sparks( vecSrc + vecDir * 24 );
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM, 0, 80);
			
	}
}

void CGauss::FourthAttack()
{
	if (allowmonsters10.value == 1 || allowmonsters9.value == 0)
		{
			m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}
		
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	if ( m_fInAttack == 0 )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}
		
		// fire
		m_fPrimaryFire = FALSE;
		m_fFourthFire = TRUE; 

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		SendWeaponAnim( GAUSS_SPINUP );
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
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}
	}
	else
	{
		// during the charging process, eat one bit of ammo every once in a while
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flNextAmmoBurn && m_pPlayer->m_flNextAmmoBurn != 900 )
		{

			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
			}

		}

		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
			return;
		}
		
		if ( UTIL_WeaponTimeBase() >= m_pPlayer->m_flAmmoStartCharge )
		{
			// don't eat any more ammo after gun is fully charged.
			m_pPlayer->m_flNextAmmoBurn = 900;
		}

		int pitch = ( gpGlobals->time - m_pPlayer->m_flStartCharge ) * ( 150 / GetFullChargeTime() ) + 100;
		if ( pitch > 270 ) 
			 pitch = 270;

		
		
		PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );
		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		// great self explode
		
		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 5 )
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
				
			UTIL_ScreenFade( m_pPlayer, Vector(255,0,0), 4, 8.5, 160, FFADE_IN );

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
				WRITE_COORD( pev->origin.x);
				WRITE_COORD( pev->origin.y);
				WRITE_COORD( pev->origin.z);
				WRITE_SHORT( m_iSpriteTexture2 ); //other sprite
				WRITE_BYTE( 80  ); // scale * 10
				WRITE_BYTE( 12  ); // framerate
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();
			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 120, 640, CLASS_NONE, DMG_ENERGYBEAM  ); //DMG
			
			SendWeaponAnim( GAUSS_IDLE );
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
}




// now unused
BOOL CGauss::Lock( )
{

	return TRUE;

}



//=========================================================
// StartFire- since all of this code has to run and then 
// call Fire(), it was easier at this point to rip it out 
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names 
//=========================================================
void CGauss::StartFire( void )
{
	float flDamage;
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition( );
	
	if ( gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime() )
		flDamage = 150;
	else
		flDamage = 150 * (( gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime() );

	if ( m_fPrimaryFire && !m_fFourthFire )
		flDamage = 16;

	if (m_fInAttack != 3)
	{

		float flZVel = m_pPlayer->pev->velocity.z;
		if ( m_fPrimaryFire==FALSE && allowmonsters6.value != 0 && allowmonsters10.value == 0) // added in 1.30a
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * (flDamage+50) * 5;

		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	}
	
	Fire( vecSrc, vecAiming, flDamage );
}

void CGauss::Fire( Vector vecOrigSrc, Vector vecDir, float flDamage )
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

				// explode a bit
				m_pPlayer->RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, flDamage * n, CLASS_NONE, DMG_ENERGYBEAM );

				nTotal += 16;
				
				// lose energy
				if (n == 0) n = 0.1;
				
			}
			else
			{
				nTotal += 16;

				// limit it to one hole punch
				if (fHasPunched)
					break;
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( m_fPrimaryFire == FALSE && m_fFourthFire == FALSE ) //if second attack
				{
					m_pPlayer->pev->punchangle.x = -(flDamage/7); //hardest aim, stop easy fraging
					UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, pentIgnore, &beam_tr);

					if (!beam_tr.fAllSolid)
					{
						// trace backwards to find exit point
						UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, pentIgnore, &beam_tr);

						float n = (beam_tr.vecEndPos - tr.vecEndPos).Length( );
						
						if (n < flDamage)
						{
							if (n == 0) n = 1;
							flDamage -= n;
							nTotal += 16;
							float damage_radius;
							damage_radius = flDamage * 3.6;  // Old code == 2.5, rebalanced in 1.35
							::RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage*0.3, damage_radius, CLASS_NONE, DMG_BLAST );
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
			UTIL_MakeVectors( anglesAim );

			CBaseEntity *pGauss = Create( "virtual_hull", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pGauss->pev->velocity = vecThrow;
			pGauss->pev->scale = (flDamage * 0.02);
			
			if ( !FBitSet( m_pPlayer->pev->flags, FL_DUCKING ) )
				pGauss->pev->dmg = (flDamage * 0.64);
			else
				pGauss->pev->dmg = (flDamage * 0.86); // higher dmg

			SendWeaponAnim( 5 );
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			pentIgnore = ENT( pEntity->pev );
			return;
		}
		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
		}
	}
	return;
}



void CGauss::WeaponIdle( void )
{

	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 200 && ( m_pPlayer->m_flNextChatTime3 < gpGlobals->time ) ) 
		{
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			Reload();
			return;
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
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0;

	
}


void CGauss :: Reload( void )
{

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector GunPosition = m_pPlayer->GetGunPosition( );
	GunPosition = GunPosition + gpGlobals->v_forward * 0;
	GunPosition = GunPosition + gpGlobals->v_right   * 7;
	GunPosition = GunPosition + gpGlobals->v_up      * 0;


	CGaussNuke* Beam = CGaussNuke :: Create( GunPosition,
											 m_pPlayer->pev->v_angle,
											 m_pPlayer );

	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 200;
	m_pPlayer->m_flNextChatTime3 = gpGlobals->time + 240; //4 minutes
	// dont silent spawn
	UTIL_ShowMessageAll( "nuclear missile launched!"  ); // STRING(m_pPlayer->pev->netname)
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "buttons/bell1.wav", 1, ATTN_NORM);
	
	int		i;
	// loop through all players
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
		if ( pPlayer )
			EMIT_SOUND(ENT(pPlayer->pev), CHAN_STATIC, "zxc/nldm.wav", 1, ATTN_NORM);
	}
	
	
}

class CGaussAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_gaussammo.mdl");
		CBasePlayerAmmo::Spawn( );
		//
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_gaussammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_gaussclip, CGaussAmmo );






/**********************************

Nuke bomb code

**********************************/

void    CGaussNuke :: Spawn( )
{
	m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/shockwave.spr" );
	BeamSprite = PRECACHE_MODEL( "sprites/smoke.spr" );
	
	SET_MODEL( ENT(pev), "models/nuke.mdl" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	
	UTIL_SetSize( pev, Vector(-4,-4,-4), Vector(4,4,4) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "nuke" );
	SetThink( MoveThink );
	SetTouch( Hit );
	pev->velocity = gpGlobals->v_forward * 100;
	pev->gravity = 0.65;
	pev->friction = 0.015;
	pev->dmg = 15;
	m_flDie = gpGlobals->time + 30;
	pev->avelocity.z = 128;
	
	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 0.5; 

}

void    CGaussNuke :: Hit( CBaseEntity* Target )
{
	pev->angles = UTIL_VecToAngles (pev->velocity);
	TraceResult TResult;
	Vector      StartPosition;
	pev->enemy = Target->edict( );
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;
	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );
					
	Explode( &TResult, DMG_ENERGYBEAM ); //direct damage, moved to radiation spawn
}

void    CGaussNuke :: Explode( TraceResult* TResult, int DamageType )
{
	if( TResult->fAllSolid ) return;
	
	//MORE EFFECTS!
	CBaseEntity *pEntity = NULL;
	Vector	vecDir;
	vecDir = Vector( 0, 0, 0 );
	static int bitsDamageType;

	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 2048+pev->dmg )) != NULL) //1600
		{
			if (pEntity->pev->takedamage || pEntity->pev->solid == SOLID_NOT) ///check only players
			{
				vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize(); ///NOW WORKED! CONGRATULATIONS!
				pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -2048;
				UTIL_ScreenShake( pEntity->pev->origin, 1024.0, 90.5, 154.7, 1 );

				pEntity->TakeDamage(pev, VARS( pev->owner ), pev->dmg, bitsDamageType & ( DMG_POISON | DMG_SHOCK | DMG_SLASH ));

				UTIL_ScreenFade( pEntity, Vector(200,30,0), 300, 30, 30, FFADE_IN );
				pEntity->pev->punchangle.x = 10;
				pEntity->pev->punchangle.y = RANDOM_LONG(-74, 40);
				pEntity->pev->punchangle.z = -20;

			}
		}
		
	CBaseEntity::Create( "trigger_killmonster", pev->origin, pev->angles, pev->owner );
	UTIL_Remove( this );
		
}

CGaussNuke* CGaussNuke :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
	CGaussNuke* Beam = GetClassPtr( (CGaussNuke*)NULL );
	UTIL_SetOrigin( Beam->pev, Pos );
	Beam->Spawn( );
	Beam->SetTouch( CGaussNuke :: Hit );
	Beam->pev->owner = Owner->edict( );
	Beam->pev->angles = UTIL_VecToAngles (Beam->pev->velocity);
	return Beam;
}

void    CGaussNuke :: MoveThink( )
{

	// pev->angles = UTIL_VecToAngles (pev->velocity); //dynamic angles

	// blast circle in process (bounce)
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 1600 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture2 );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 40 ); // life
		WRITE_BYTE( 4 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 192 );   // r, g, b
		WRITE_BYTE( 128 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();
	
	if (gpGlobals->time >= m_flDie)
		{
			CBaseEntity *pEntity = NULL;
			Vector	vecDir;
			vecDir = Vector( 0, 0, 0 );
			static int bitsDamageType;

			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 2048+pev->dmg )) != NULL) //1600
				{
					if (pEntity->pev->takedamage || pEntity->pev->solid == SOLID_NOT) ///check only players
					{
						vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize(); ///NOW WORKED! CONGRATULATIONS!
						pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -2048;
						UTIL_ScreenShake( pEntity->pev->origin, 1024.0, 90.5, 154.7, 1 );

						pEntity->TakeDamage(pev, VARS( pev->owner ), pev->dmg, bitsDamageType & ( DMG_POISON | DMG_SHOCK | DMG_SLASH ));

						UTIL_ScreenFade( pEntity, Vector(200,30,0), 300, 30, 30, FFADE_IN );
						pEntity->pev->punchangle.x = 10;
						pEntity->pev->punchangle.y = RANDOM_LONG(-74, 40);
						pEntity->pev->punchangle.z = -20;
						
					}
				}
			CBaseEntity::Create( "trigger_killmonster", pev->origin, pev->angles, pev->owner );
			UTIL_Remove( this );
		}
		
	pev->dmg += 4;
	pev->nextthink = gpGlobals->time + 1.0; 

}

//////////////radiation point

void    CRadiation :: Spawn( )
{
	SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 0;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "Nuke_Radiation" );
	m_flDie10 = gpGlobals->time + 60; // timer
	pev->dmg = 4;
	pev->nextthink = gpGlobals->time + 0.1;//10 times a second
	SetThink( MoveThink );
	m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/shockwave.spr" );
	m_iSpriteTexture22 = PRECACHE_MODEL( "sprites/cexplo.spr" );
	m_iBodyGibs = PRECACHE_MODEL( "models/metalplategibs.mdl" );
	pev->gravity		= 0;
	pev->friction		= 0;
	
	TraceResult TResult;
	Vector      StartPosition;
	StartPosition = pev->origin;
	
	//break metals
	Vector vecSpot = pev->origin + (pev->mins + pev->maxs);
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
		WRITE_BYTE( TE_BREAKMODEL);
		WRITE_COORD( vecSpot.x );
		WRITE_COORD( vecSpot.y );
		WRITE_COORD( vecSpot.z);
		WRITE_COORD( 200 );
		WRITE_COORD( 200 );
		WRITE_COORD( 128 );
		WRITE_COORD( 30 ); 
		WRITE_COORD( 30 );
		WRITE_COORD( 300 );
		WRITE_BYTE( 40 ); 
		WRITE_SHORT( m_iBodyGibs );	//model id#
		WRITE_BYTE( 30 );
		WRITE_BYTE( 200 );// 1.0 seconds
		WRITE_BYTE( BREAK_CONCRETE );
	MESSAGE_END();
/* 	
	// very strange effect ^_^
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_EXPLODEMODEL	 );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->origin.x);///
		WRITE_SHORT( 1 ); // entity
		WRITE_SHORT( RANDOM_LONG(1,2) ); // model
		WRITE_BYTE( 64 ); // life
	MESSAGE_END();
 */
/*  
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BREAKMODEL);
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( 512 );
		WRITE_COORD( 512 );
		WRITE_COORD( 512 );
		WRITE_COORD( 0 ); 
		WRITE_COORD( 0 );
		WRITE_COORD( 0 );
		WRITE_BYTE( 20 ); 
		WRITE_SHORT( 1 );	//model id#
		WRITE_BYTE( 1 );
		WRITE_BYTE( 10 );// 1.0 seconds
		WRITE_BYTE( BREAK_METAL );
	MESSAGE_END();
	 */
/* 	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BREAKMODEL);
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( 256 );
		WRITE_COORD( 256 );
		WRITE_COORD( 256 );
		WRITE_COORD( 0 ); 
		WRITE_COORD( 0 );
		WRITE_COORD( 0 );
		WRITE_BYTE( 20 ); 
		WRITE_SHORT( 1 );	//model id#
		WRITE_BYTE( 1 );
		WRITE_BYTE( 200 );// x.0 seconds
		WRITE_BYTE( BREAK_CONCRETE );
	MESSAGE_END();
	
	 */
	UTIL_TraceLine( StartPosition,
					StartPosition,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );
					
	// Pull out of the wall a bit
	if ( TResult.flFraction != 1.0 )
	{
		pev->origin = TResult.vecEndPos + (TResult.vecPlaneNormal * 25.6);
	}

	
	
	//Explode
	// ::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 10240, 512, CLASS_NONE, DMG_BULLET  ); //*10 increase dmg! (< 1.26)

	//torus
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMTORUS );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z); 
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 170 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture2 );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 128 ); // life
		WRITE_BYTE( 64 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 128 );   // r, g, b
		WRITE_BYTE( 0 );   // r, g, b
		WRITE_BYTE( 128 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();
	//torus 2
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMDISK );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z); 
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 150 ); // reach damage radius over .2 seconds
		WRITE_SHORT( m_iSpriteTexture2 );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 128 ); // life
		WRITE_BYTE( 250 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 150 );   // r, g, b
		WRITE_BYTE( 120 );   // r, g, b
		WRITE_BYTE( 70 );   // r, g, b
		WRITE_BYTE( 100 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_LARGEFUNNEL );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT( m_iSpriteTexture22 ); // model
		WRITE_SHORT( RANDOM_LONG(0,10) ); 
	MESSAGE_END();
	
	//effects
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 50 ));
		WRITE_SHORT( m_iSpriteTexture22 );
		WRITE_BYTE( 400  ); // scale * 10
		WRITE_BYTE( 8  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	// random explosions
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -250, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 250 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 50 ));
		WRITE_SHORT( m_iSpriteTexture22 );
		WRITE_BYTE( 450  ); // scale * 10
		WRITE_BYTE( 6  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	// random explosions
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
		WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 50 ));
		WRITE_SHORT( m_iSpriteTexture22 );
		WRITE_BYTE( 500  ); // scale * 10
		WRITE_BYTE( 5  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	//big lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 64 );		// radius * 0.1
		WRITE_BYTE( 64 );		// r
		WRITE_BYTE( 40 );		// g
		WRITE_BYTE( 1 );		// b
		WRITE_BYTE( 256 );		// life * 10
		WRITE_BYTE( 4 );		// decay * 0.1
	MESSAGE_END( );
	
	
}

//edited in 1.26
void    CRadiation:: Explode()
{	

	if (m_expl <= 30)
		{
			int X = RANDOM_LONG(-1024,1024);
			int Y = RANDOM_LONG(-1024,1024);
			int Z = RANDOM_LONG(-512,1024);
			Vector vecSpot = Vector(pev->origin.x+X, pev->origin.y+Y, pev->origin.z+Z) ;
			
			TraceResult	tr;	
			UTIL_TraceLine(vecSpot, Vector(0,0,-8000), ignore_monsters, edict(), &tr);
			
			if ( (UTIL_PointContents(tr.vecEndPos ) == CONTENTS_SOLID) )
				{
					// random explosions
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_EXPLOSION);
						WRITE_COORD( vecSpot.x );
						WRITE_COORD( vecSpot.y );
						WRITE_COORD( vecSpot.z );
						WRITE_SHORT( m_iSpriteTexture22 );
						WRITE_BYTE( 500  ); // scale * 10
						WRITE_BYTE( 20  ); // framerate
						WRITE_BYTE( TE_EXPLFLAG_NONE );
					MESSAGE_END();
					
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
						WRITE_BYTE( TE_BEAMCYLINDER );
						WRITE_COORD( vecSpot.x);
						WRITE_COORD( vecSpot.y);
						WRITE_COORD( vecSpot.z);
						WRITE_COORD( vecSpot.x);
						WRITE_COORD( vecSpot.y);
						WRITE_COORD( vecSpot.z + 1600 ); 
						WRITE_SHORT( m_iSpriteTexture22 );
						WRITE_BYTE( 0 ); // startframe
						WRITE_BYTE( 5 ); // framerate
						WRITE_BYTE( 40 ); // life
						WRITE_BYTE( 30 );  // width
						WRITE_BYTE( 255 );   // noise
						WRITE_BYTE( 192 );   // r, g, b
						WRITE_BYTE( 192 );   // r, g, b
						WRITE_BYTE( 128 );   // r, g, b
						WRITE_BYTE( 100 ); // brightness
						WRITE_BYTE( 0 );		// speed
					MESSAGE_END();
					
					::RadiusDamage( vecSpot, pev, VARS( pev->owner ), pev->dmg, 1200, CLASS_NONE, DMG_BULLET  );

					m_expl++;
				}
			pev->nextthink = gpGlobals->time + 0.1;
				
		}
	else
		pev->nextthink = gpGlobals->time + 0.3;

	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, 512, CLASS_NONE, DMG_RADIATION  );
	
/* 	
	// lots of smoke
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z - 8 );
		WRITE_SHORT( g_sModelIndexBubbles );
		WRITE_BYTE( 16 ); // scale * 10
		WRITE_BYTE( 10 ); // framerate
	MESSAGE_END();
*/
		 
	//lights
	Vector vecSrc = pev->origin;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 42 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 44 );		// g
		WRITE_BYTE( 0 );		// b
		WRITE_BYTE( 4 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
	
	SetThink(MoveThink);
}


void    CRadiation :: MoveThink( )
{
	Explode();

	if (gpGlobals->time >= m_flDie10)
		{
			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 50, 512, CLASS_NONE, DMG_RADIATION  );
			UTIL_Remove( this );
		}
}



/////////////////////////////
/////////////////////////////
/////////////////////////////
//Gauss Cannon 3rd update f-e

void CGCannon::Spawn( void )
{
	SET_MODEL( ENT(pev), "sprites/yelflare1.spr" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, -4), Vector(4, 4, 4) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	m_timer = 1.0;
	
	pev->avelocity.z = -500;
	
	pev->rendermode = kRenderTransAdd; //kRenderTransAlpha
	pev->renderamt = 200;
	
	pev->classname = MAKE_STRING( "weapon_gauss" );
	
	m_Sprite    = PRECACHE_MODEL( "sprites/richo2.spr" ); //explode

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/zbeam1.spr" ); //trails
	
	m_iBalls = PRECACHE_MODEL( "sprites/nhth1.spr" );
	
	//spawn sounds
	switch(RANDOM_LONG(0,3))
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
	
	//loop
	//EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/hold_loop.wav", 1.00, ATTN_NORM, 1.0, 100 );
	

	SetTouch( MoveTouch );
	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 0.1; 
}

void CGCannon::MoveThink( )
{

	m_timer += 1.5;
	
	if (m_timer >= pev->dmg * 0.6)
	{
		SetTouch( NULL );
		UTIL_Remove( this );
	}
	
	//trails
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( entindex() );	// entity
		WRITE_SHORT( m_iSpriteTexture );	// model
		WRITE_BYTE( 9 ); // life
		WRITE_BYTE( 10 );  // width
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 160 );	// brightness
	MESSAGE_END();
	
	//Create( "env_beamtrail", pev->origin, pev->angles, pev->owner );
	UTIL_ParticleEffect ( pev->origin, Vector(0,0,-128), 92, 25 );

	pev->nextthink = gpGlobals->time + 0.1; 
}

void CGCannon::MoveTouch( CBaseEntity *pOther )
{
	TraceResult TResult;
	Vector      StartPosition;
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;
	

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );

	// Pull out of the wall a bit
	if ( TResult.flFraction != 1.0 )
	{
		pev->origin = TResult.vecEndPos + (TResult.vecPlaneNormal * 25.6);
	}

	if( TResult.fAllSolid ) return;

	// animated sprite
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( m_Sprite );
		WRITE_BYTE( 70 ); // scale
		WRITE_BYTE( 172 ); // brightness
	MESSAGE_END();

	//lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 24 );		// radius * 0.1
		WRITE_BYTE( 150 );		// r
		WRITE_BYTE( 200 );		// g
		WRITE_BYTE( 200 );		// b
		WRITE_BYTE( 128 );		// time * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );
	
	//play sound
	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM); //play sound
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/energy_sing_explosion2.wav", 1.0, ATTN_NORM); //play sound


	while ((pOther = UTIL_FindEntityInSphere( pOther, pev->origin, pev->dmg*2 )) != NULL)
	{
		if (pOther->pev->takedamage && (FVisible( pOther )))
		{
		if ((pOther->edict() != pev->owner) && pOther->pev->takedamage && (pOther->edict() != edict()) && pOther->pev->health >= 3) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				//pOther->TakeDamage(pev, VARS( pev->owner ), pev->dmg/5, DMG_BLAST);
				Create( "item_nvg", pOther->pev->origin, pev->angles, pev->owner );
				
				// balls
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITETRAIL );
					WRITE_COORD( pOther->pev->origin.x );
					WRITE_COORD( pOther->pev->origin.y );
					WRITE_COORD( pOther->pev->origin.z );
					WRITE_COORD( pOther->pev->origin.x );
					WRITE_COORD( pOther->pev->origin.y );
					WRITE_COORD( pOther->pev->origin.z  );
					WRITE_SHORT( m_iBalls );		// model
					WRITE_BYTE( 9  );				// count
					WRITE_BYTE( 1 );				// life * 10
					WRITE_BYTE( 2 );				// size * 10
					WRITE_BYTE( 90 );				// amplitude * 0.1
					WRITE_BYTE( 2 );				// speed * 100
				MESSAGE_END();
			}
		}
		
	}
		
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg*3.25, CLASS_NONE, DMG_ENERGYBEAM  );
				
	// if (pOther->pev->takedamage != NULL)
	// {
		// UTIL_ScreenShake( pOther->pev->origin, 24.0, 174.5, 0.5, 2 );
		// pOther->TakeDamage(pev, VARS( pev->owner ), pev->dmg, DMG_MORTAR);	
	// }
	STOP_SOUND( ENT(pev), CHAN_VOICE, "zxc/hold_loop.wav" );
	SetTouch( NULL );
	UTIL_Remove( this );

}

/////////////
/////particle
/////////////


void CGParticle::Spawn( )
{
	SET_MODEL( ENT(pev), "sprites/shrinkf.spr" );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	
	pev->scale = 0.5;
	
	pev->rendermode = kRenderTransAdd; //kRenderTransAlpha
	pev->renderamt = 120;

	
	//STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav" );
	
	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 0.1; 
}

void CGParticle::MoveThink( )
{
	pev->scale -= 0.04;
	pev->renderamt -= 7;
	if (pev->scale <= 0.03)
		UTIL_Remove( this );
		
	pev->nextthink = gpGlobals->time + 0.05; 
}



////////////////////
/////particle 2/////
////////////////////


void CGParticle2::Spawn( )
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->classname = MAKE_STRING( "weapon_gauss" );
	UTIL_SetOrigin( pev, pev->origin );
	pev->gravity = 0.0;
	pev->scale = 1.6;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 90;
	m_Sprite = PRECACHE_MODEL( "sprites/zbeam4.spr" );
	CBaseEntity *pEntity = NULL;

	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 462 )) != NULL)
	{
		if (pEntity->pev->takedamage && (FVisible( pEntity )))
		{
			if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict()) && pEntity->pev->health >= 3) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				//::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(15,49), 256, CLASS_NONE, DMG_MORTAR  );
				//shock ray
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMPOINTS );
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_COORD( pEntity->pev->origin.x ); //tr.vecEndPos.
					WRITE_COORD( pEntity->pev->origin.y );
					WRITE_COORD( pEntity->pev->origin.z );
					WRITE_SHORT( m_Sprite ); //sprite
					WRITE_BYTE( 1 ); // Starting frame
					WRITE_BYTE( 0  ); // framerate * 0.1
					WRITE_BYTE( 4 ); // life * 0.1
					WRITE_BYTE( 40 ); // width
					WRITE_BYTE( 4 ); // noise
					WRITE_BYTE( 250 ); // color r,g,b
					WRITE_BYTE( 250 ); // color r,g,b
					WRITE_BYTE( 55 ); // color r,g,b
					WRITE_BYTE( 128 ); // brightness
					WRITE_BYTE( 256 ); // scroll speed
				MESSAGE_END();
				
				UTIL_ScreenShake( pEntity->pev->origin, 12.0, 90.5, 0.3, 1 );
				pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(21,39), DMG_ENERGYBEAM);	
			}
		}
	}
	
	UTIL_Remove( this );
}
