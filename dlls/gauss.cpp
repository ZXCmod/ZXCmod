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
#include "shake.h"
#include "gamerules.h"
#include "blaster2.h"
#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged



////Radiation by nuke 

class   CRadiation : public CBaseEntity
{
        public:

        void    Spawn           ( );
        void    MoveThink       ( );
        void    Explode         ();
		int m_flDie10;
};

LINK_ENTITY_TO_CLASS( trigger_killmonster, CRadiation );







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

LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );

float CGauss::GetFullChargeTime( void )
{
#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
	
		return 1.5;
	}

	return 4;
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
	m_flNextChatTime3 = gpGlobals->time;

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

	PRECACHE_SOUND("weapons/gauss2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("ambience/pulsemachine.wav");
	
	m_iGlow = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBeam = PRECACHE_MODEL( "sprites/smoke.spr" );

	m_usGaussFire = PRECACHE_EVENT( 1, "events/gauss.sc" );
	m_usGaussSpin = PRECACHE_EVENT( 1, "events/gaussspin.sc" );

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
	PLAYBACK_EVENT_FULL( FEV_RELIABLE | FEV_NOTHOST, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );
	
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( GAUSS_HOLSTER );
	m_fInAttack = 0;

}


void CGauss::PrimaryAttack()
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

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 2;

	StartFire();
	m_fInAttack = 0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.2;
}

void CGauss::SecondaryAttack()
{
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

	if ( m_fInAttack == 0 )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
			return;
		}

		m_fPrimaryFire = FALSE;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
		m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase();

		// spin up
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		SendWeaponAnim( GAUSS_SPINUP );
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_flStartCharge = gpGlobals->time;
		m_pPlayer->m_flAmmoStartCharge = UTIL_WeaponTimeBase() + GetFullChargeTime();

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0 );

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
#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.1;
			}
			else
			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_pPlayer->m_flNextAmmoBurn = UTIL_WeaponTimeBase() + 0.3;
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
		

		if ( m_iSoundState == 0 )
			ALERT( at_console, "sound state %d\n", m_iSoundState );

		PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, ( m_iSoundState == SND_CHANGE_PITCH ) ? 1 : 0, 0 );

		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		// m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		if ( m_pPlayer->m_flStartCharge < gpGlobals->time - 9 )
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
				
#ifndef CLIENT_DLL
			m_pPlayer->TakeDamage( VARS(eoNullEntity), VARS(eoNullEntity), 50, DMG_SHOCK );
			UTIL_ScreenFade( m_pPlayer, Vector(255,0,0), 4, 8.5, 200, FFADE_IN );
#endif
			SendWeaponAnim( GAUSS_IDLE );
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
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
	Vector vecSrc = m_pPlayer->GetGunPosition( ); // + gpGlobals->v_up * -8 + gpGlobals->v_right * 8;
	
	if ( gpGlobals->time - m_pPlayer->m_flStartCharge > GetFullChargeTime() )
	{
		flDamage = 200;
	}
	else
	{
		flDamage = 200 * (( gpGlobals->time - m_pPlayer->m_flStartCharge) / GetFullChargeTime() );
	}

	if ( m_fPrimaryFire )
	{
		// fixed damage on primary attack
#ifdef CLIENT_DLL
		flDamage = 21;
#else 
		flDamage = 21;
#endif
	}

	if (m_fInAttack != 3)
	{
		//ALERT ( at_console, "Time:%f Damage:%f\n", gpGlobals->time - m_pPlayer->m_flStartCharge, flDamage );

#ifndef CLIENT_DLL
		float flZVel = m_pPlayer->pev->velocity.z;

		if ( !m_fPrimaryFire )
		{
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * flDamage * 5;
		}

		if ( !g_pGameRules->IsMultiplayer() )

		{
			// in deathmatch, gauss can pop you up into the air. Not in single play.
			m_pPlayer->pev->velocity.z = flZVel;
		}
#endif
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	}

	// time until aftershock 'static discharge' sound
	m_pPlayer->m_flPlayAftershock = gpGlobals->time + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.3, 0.8 );

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
	int	nMaxHits = 40;

	pentIgnore = ENT( m_pPlayer->pev );

#ifdef CLIENT_DLL
	if ( m_fPrimaryFire == false )
		 g_irunninggausspred = true;
#endif
	
	// The main firing event is sent unreliably so it won't be delayed.
	PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usGaussFire, 0.0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, flDamage, 0.0, 0, 0, m_fPrimaryFire ? 1 : 0, 0 );

	// This reliable event is used to stop the spinning sound
	// It's delayed by a fraction of second to make sure it is delayed by 1 frame on the client
	// It's sent reliably anyway, which could lead to other delays

	PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1 );

	
	/*ALERT( at_console, "%f %f %f\n%f %f %f\n", 
		vecSrc.x, vecSrc.y, vecSrc.z, 
		vecDest.x, vecDest.y, vecDest.z );*/
	

//	ALERT( at_console, "%f %f\n", tr.flFraction, flMaxFrac );

#ifndef CLIENT_DLL
	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		// ALERT( at_console, "." );
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
	
			nTotal += 26;
		}
		
		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecDir, &tr, DMG_BULLET );
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}

		if ( pEntity->ReflectGauss() )
		{
			float n;

			pentIgnore = m_pPlayer->edict();

			n = -DotProduct(tr.vecPlaneNormal, vecDir);

			if (n < 0.5) // 60 degrees
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				Vector r;
			
				r = 2.0 * tr.vecPlaneNormal * n + vecDir;
				flMaxFrac = flMaxFrac - tr.flFraction;
				vecDir = r;
				vecSrc = tr.vecEndPos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;

				// explode a bit
				m_pPlayer->RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, flDamage * n, CLASS_NONE, DMG_BLAST );

				nTotal += 34;
				
				// lose energy
				if (n == 0) n = 0.1;
				flDamage = flDamage * (1 - n);
			}
			else
			{
				nTotal += 13;

				// limit it to one hole punch
				if (fHasPunched)
					break;
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( !m_fPrimaryFire )
				{
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

							// ALERT( at_console, "punch %f\n", n );
							nTotal += 21;

							// exit blast damage
							//m_pPlayer->RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, CLASS_NONE, DMG_BLAST );
							float damage_radius;
							

							if ( g_pGameRules->IsMultiplayer() )
							{
								damage_radius = flDamage * 1.5;  // Old code == 2.5
							}
							else
							{
								damage_radius = flDamage * 1.7;
							}

							::RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, damage_radius, CLASS_NONE, DMG_BLAST );

							CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );

							nTotal += 53;

							vecSrc = beam_tr.vecEndPos + vecDir;
						}
					}
					else
					{
						 //ALERT( at_console, "blocked %f\n", n );
						flDamage = 0;
					}
				}
				else
				{
					//ALERT( at_console, "blocked solid\n" );
					
					flDamage = 0;
				}

			}
		}
		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
		}
	}
#endif
	// ALERT( at_console, "%d bytes\n", nTotal );
}



void CGauss::WeaponIdle( void )
{
//if 
//nreloaddelay++;
if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 200) 

if (  m_pPlayer->m_flNextChatTime3 < gpGlobals->time )
{

	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		//nreloaddelay = 20;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		Reload();
		//m_flStartThrow = gpGlobals->time;z
		m_pPlayer->m_flNextChatTime3 = gpGlobals->time + 120;
		return;
		
	}
}

else
{
	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 200) 
	{
	PlayEmptySound();
	return;
	}
}
	
	
	ResetEmptySound( );

	// play aftershock static discharge
	if ( m_pPlayer->m_flPlayAftershock && m_pPlayer->m_flPlayAftershock < gpGlobals->time )
	{
		switch (RANDOM_LONG(0,3))
		{
		case 0:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 1:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro5.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 2:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro6.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 3:	break; // no sound
		}
		m_pPlayer->m_flPlayAftershock = 0.0;
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
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5)
		{
			iAnim = GAUSS_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		}
		else if (flRand <= 0.75)
		{
			iAnim = GAUSS_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		}
		else
		{
			iAnim = GAUSS_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
		}

		return;
		SendWeaponAnim( iAnim );
	}
	

}


void CGauss :: Reload( void )
{

        UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

        Vector GunPosition = m_pPlayer->GetGunPosition( );
        GunPosition = GunPosition + gpGlobals->v_forward * BLASTER_OFFSET_FORWARD;
        GunPosition = GunPosition + gpGlobals->v_right   * BLASTER_OFFSET_RIGHT;
        GunPosition = GunPosition + gpGlobals->v_up      * BLASTER_OFFSET_UP;


        CBlaster2Beam* Beam = CBlaster2Beam :: Create( GunPosition,
                                                 m_pPlayer->pev->v_angle,
                                                 m_pPlayer );

        float RandomX = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
                                       BLASTER_BEAM_RANDOMNESS );
        float RandomY = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
                                       BLASTER_BEAM_RANDOMNESS );

        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right * RandomX;
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up    * RandomY;
	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 200;
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

#endif





//////////newww!!!!!!






// Associate it with the Worldcraft entity:

LINK_ENTITY_TO_CLASS( weapon_nuke, CBlaster2 );

// The CBlaster2Beam class:


void CBlaster2 :: Spawn( )
{
}
void CBlaster2 :: Precache( )
{
}
void CBlaster2 :: Reload( )
{

        UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

        Vector GunPosition = m_pPlayer->GetGunPosition( );
        GunPosition = GunPosition + gpGlobals->v_forward * BLASTER_OFFSET_FORWARD;
        GunPosition = GunPosition + gpGlobals->v_right   * BLASTER_OFFSET_RIGHT;
        GunPosition = GunPosition + gpGlobals->v_up      * BLASTER_OFFSET_UP;


        CBlaster2Beam* Beam = CBlaster2Beam :: Create( GunPosition,
                                                 m_pPlayer->pev->v_angle,
                                                 m_pPlayer );

        float RandomX = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
                                       BLASTER_BEAM_RANDOMNESS );
        float RandomY = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
                                       BLASTER_BEAM_RANDOMNESS );

        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right * RandomX;
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up    * RandomY;

}

/**********************************

CBlaster2BEAM STUFF:

**********************************/
#define SQUEEK_DETONATE_DELAY	16.0
#define SQUEEK_DETONATE_DELAY2	4.0
#define SQUEEK_DETONATE_DELAY3	4.5
void    CBlaster2Beam :: Spawn( )
{
        Precache( );
        SET_MODEL( ENT(pev), "models/nuke.mdl" );
        pev->movetype = MOVETYPE_BOUNCE;
        pev->solid = SOLID_BBOX;
		pev->flags |= FL_MONSTER;
		
        UTIL_SetSize( pev, Vector(-1,-1,-1), Vector(1,1,1) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "nuke" );
        SetThink( MoveThink );
        SetTouch( Hit );
        pev->velocity = gpGlobals->v_forward * BLASTER_BEAM_SPEED;
		pev->gravity = 0.65;
		pev->friction = 0.015;
        pev->dmg = DMG_BLAST;
		dmge = pev->dmg = 160;
		m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;
		m_flDie2 = gpGlobals->time + SQUEEK_DETONATE_DELAY2;
		m_flDie3 = gpGlobals->time + SQUEEK_DETONATE_DELAY3;
}

void    CBlaster2Beam :: Precache( )
{
		m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/shockwave.spr" );
        BeamSprite = PRECACHE_MODEL( BLASTER_BEAM_SPRITE );
		PRECACHE_MODEL("models/nuke.mdl");
}

void    CBlaster2Beam :: Hit( CBaseEntity* Target )
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
						
	//if (gpGlobals->time >= m_flDie - 0.1)
	//{				
	//pev->movetype = MOVETYPE_TOSS;
	//dmge = pev->dmg = 1024;
	//}
	
	if (gpGlobals->time >= m_flDie)
	{
		dmge = pev->dmg = 1024;
		Explode( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR );
		return;
	}
	if (gpGlobals->time >= m_flDie + 0.3)
	{
		dmge = pev->dmg = 0;
		return;
	}
	
	
	if (gpGlobals->time >= m_flDie2)
	{
		pev->effects |= EF_INVLIGHT;
		Explode( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR );
		Explode2( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR );
		pev->nextthink = gpGlobals->time + 0.1;
	}

}
void    CBlaster2Beam :: Explode( TraceResult* TResult, int DamageType )
{
        RadiusDamage( pev,
                      VARS( pev->owner ),
                      pev->dmg,
                      CLASS_NONE,
                      DamageType );

	dmge = pev->dmg = RANDOM_LONG(22,64);
    if( TResult->fAllSolid ) return;
	if (gpGlobals->time >= m_flDie)
		{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(10,20) + 70  ); // scale * 10
			WRITE_BYTE( 3  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		// random explosions
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -250, 150 ));
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 250 ));
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(10,20) + 90  ); // scale * 10
			WRITE_BYTE( 1  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		// random explosions
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(10,20) + 80  ); // scale * 10
			WRITE_BYTE( 2  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		// random explosions
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -150, 50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(10,20) + 170  ); // scale * 10
			WRITE_BYTE( 3  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		// blast circle
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 100 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture2 );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 99 ); // life
			WRITE_BYTE( 220 );  // width
			WRITE_BYTE( 124 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 192 );   // r, g, b
			WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();
		
		// blast circle
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 40 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture2 );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 99 ); // life
			WRITE_BYTE( 250 );  // width
			WRITE_BYTE( 164 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 160 );   // r, g, b
			WRITE_BYTE( 1 );   // r, g, b
			WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 0 );		// speed
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
		WRITE_BYTE( 32 );		// g
		WRITE_BYTE( 1 );		// b
		WRITE_BYTE( 255 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );

			pev->takedamage = DAMAGE_NO;
			
			//MORE EFFECTS!
			CBaseEntity *pEntity = NULL;
			Vector	vecDir;
			vecDir = Vector( 0, 0, 0 );

			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 2048 )) != NULL)
       		 	{
					if (pEntity->pev->movetype == MOVETYPE_WALK) ///NICE!!!
					{
					vecDir = ( pEntity->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize(); ///NOW WORKED! CONGRATULATIONS!
					pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -2048;
					UTIL_ScreenShake( pEntity->pev->origin, 1024.0, 90.5, 154.7, 1 );
					#ifndef CLIENT_DLL
					UTIL_ScreenFade( pEntity, Vector(RANDOM_LONG(128,255),RANDOM_LONG(0,64),0), 300, 30, 100, FFADE_IN );
					#endif
					}
				}
				
		CBaseEntity::Create( "trigger_killmonster", pev->origin, pev->angles, pev->owner );
		pev->nextthink = gpGlobals->time + 0.1;
		SUB_Remove( );
		}
}

void    CBlaster2Beam :: Explode2( TraceResult* TResult, int DamageType )
{
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -50, 50 ));
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -50, 50 ));
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 10, 50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(10,20) + 20  ); // scale * 10
			WRITE_BYTE( 10  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
	if (gpGlobals->time >= m_flDie3)
		{
			// blast circle in process (bounce)
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 100 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture2 );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 5 ); // framerate
			WRITE_BYTE( 20 ); // life
			WRITE_BYTE( 4 );  // width
			WRITE_BYTE( 255 );   // noise
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 192 );   // r, g, b
			WRITE_BYTE( 200 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();
		}
	}
}

CBlaster2Beam* CBlaster2Beam :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CBlaster2Beam* Beam = GetClassPtr( (CBlaster2Beam*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->Spawn( );
        Beam->SetTouch( CBlaster2Beam :: Hit );
        Beam->pev->owner = Owner->edict( );
		Beam->pev->angles = UTIL_VecToAngles (Beam->pev->velocity);
        return Beam;
}

void    CBlaster2Beam :: MoveThink( )
{
pev->angles = UTIL_VecToAngles (pev->velocity);
        MESSAGE_BEGIN           ( MSG_BROADCAST, SVC_TEMPENTITY );
                WRITE_BYTE      ( TE_BEAMFOLLOW );
                WRITE_SHORT     ( entindex() );
                WRITE_SHORT     ( BeamSprite );
                WRITE_BYTE      ( BLASTER_BEAM_LENGTH );
                WRITE_BYTE      ( BLASTER_BEAM_WIDTH );
                WRITE_BYTE      ( BLASTER_BEAM_RED );
                WRITE_BYTE      ( BLASTER_BEAM_GREEN );
                WRITE_BYTE      ( BLASTER_BEAM_BLUE );
                WRITE_BYTE      ( BLASTER_BEAM_BRIGHTNESS );
        MESSAGE_END             ( );
}


void    CRadiation :: Spawn( )
{
        SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
        //pev->movetype = MOVETYPE_NONE;
        pev->solid = SOLID_BBOX;
		//pev->effects |= EF_LIGHT;
        pev->rendermode = kRenderTransTexture;
        pev->renderamt = 0;
        UTIL_SetSize( pev, Vector(2,2,2), Vector(2,2,2) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "Radiation" );
		m_flDie10 = gpGlobals->time + 180;
		pev->dmg = 5;
		pev->takedamage = DAMAGE_YES;
		pev->nextthink = gpGlobals->time + 0.1;//10 times a second
		SetThink( MoveThink );
	pev->health			= 30000;
	pev->gravity		= 0;
	pev->friction		= 0;
}


void    CRadiation:: Explode()
{	
::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 7, 2048, CLASS_NONE, DMG_RADIATION  );
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -16, 16 ) );
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -16, 16 ) );
			WRITE_COORD( pev->origin.z - 32 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 15 ); // scale * 10
			WRITE_BYTE( 3 ); // framerate
		MESSAGE_END();
		
			//lights
		Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 42 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 92 );		// g
		WRITE_BYTE( 0 );		// b
		WRITE_BYTE( 64 );		// life * 10
		WRITE_BYTE( 0 );		// decay * 0.1
	MESSAGE_END( );
pev->nextthink = gpGlobals->time + 2.0;
SetThink(MoveThink);
}


void    CRadiation :: MoveThink( )
{
Explode();

if (gpGlobals->time >= m_flDie10)
	{
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 204, 204, CLASS_NONE, DMG_GENERIC  );
	// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -16, 16 ) );
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -16, 16 ) );
			WRITE_COORD( pev->origin.z - 32 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 55 ); // scale * 10
			WRITE_BYTE( 1 ); // framerate
		MESSAGE_END();

	
	pev->takedamage = DAMAGE_NO;
	SetThink( SUB_Remove );
	}
}
