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
#include "player.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "customentity.h"
#include "gamerules.h"
#include "decals.h"
#include "shake.h"
#include "egon.h"



enum egon_e {
	EGON_IDLE1 = 0,
	EGON_FIDGET1,
	EGON_ALTFIREON,
	EGON_ALTFIRECYCLE,
	EGON_ALTFIREOFF,
	EGON_FIRE1,
	EGON_FIRE2,
	EGON_FIRE3,
	EGON_FIRE4,
	EGON_DRAW,
	EGON_HOLSTER
};



void CEgon::Spawn( )
{
	Precache( );
	m_iId = WEAPON_EGON;
	SET_MODEL(ENT(pev), "models/w_egon.mdl");
	m_iDefaultAmmo = EGON_DEFAULT_GIVE;
	
	pev->ltime = 0.1; //init charger

	m_flNextChatTime10 = gpGlobals->time; //delay
	m_flNextChatTime15 = gpGlobals->time; //delay
	FallInit();// get ready to fall down.
	
}


void CEgon::Precache( void )
{
	PRECACHE_MODEL("models/w_egon.mdl");
	PRECACHE_MODEL("models/v_egon.mdl");
	PRECACHE_MODEL("models/p_egon.mdl");
	PRECACHE_MODEL( "models/alt_nuke2.mdl" );

	PRECACHE_MODEL("models/w_9mmclip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("debris/beamstart8.wav");
	
	PRECACHE_MODEL( "sprites/gradbeam.spr" );
	PRECACHE_MODEL( "sprites/glow01.spr" );
	PRECACHE_MODEL( "sprites/FrzBoom1.spr" );
	PRECACHE_MODEL("sprites/blueflare2.spr");
	PRECACHE_MODEL("models/clustergrenade.mdl");
	

	PRECACHE_SOUND( EGON_SOUND_OFF );
	PRECACHE_SOUND( EGON_SOUND_RUN );
	PRECACHE_SOUND( EGON_SOUND_STARTUP );

	PRECACHE_MODEL( EGON_BEAM_SPRITE );
	PRECACHE_MODEL( EGON_FLARE_SPRITE );

	PRECACHE_SOUND ("weapons/357_cock1.wav");
	PRECACHE_SOUND ("debris/beamstart5.wav");
	PRECACHE_SOUND ("debris/beamstart15.wav");
	PRECACHE_SOUND ("debris/beamstart1.wav");
	PRECACHE_SOUND ("ambience/alien_minddrill.wav");
	
	PRECACHE_SOUND ("buttons/spark1.wav");
	PRECACHE_SOUND ("buttons/spark2.wav");
	PRECACHE_SOUND ("buttons/spark3.wav");
	PRECACHE_SOUND ("buttons/spark4.wav");
	PRECACHE_SOUND ("buttons/spark5.wav");
	PRECACHE_SOUND ("buttons/spark6.wav");
	PRECACHE_SOUND ("ambience/sandfall2.wav");
	
	
	m_LaserSprite = PRECACHE_MODEL( "sprites/bolt1.spr" );
	PRECACHE_MODEL( "models/light.mdl" );
	PRECACHE_MODEL( "models/lightt.mdl" );
	
	

	m_usEgonFire = PRECACHE_EVENT ( 1, "events/egon_fire.sc" );
	m_usEgonStop = PRECACHE_EVENT ( 1, "events/egon_stop.sc" );
	
	PRECACHE_MODEL( "sprites/WXplo1.spr" );
	
}


int CEgon::AddToPlayer( CBasePlayer *pPlayer )
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



void CEgon::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( EGON_HOLSTER );

    EndAttack();
}

int CEgon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_EGON;
	p->iFlags = 0;
	p->iWeight = EGON_WEIGHT;

	return 1;
}

#define EGON_PULSE_INTERVAL			0.1
#define EGON_DISCHARGE_INTERVAL		0.1

float CEgon::GetPulseInterval( void )
{
	return EGON_PULSE_INTERVAL;
}

float CEgon::GetDischargeInterval( void )
{
	return EGON_DISCHARGE_INTERVAL;
}

BOOL CEgon::HasAmmo( void )
{
	if ( m_pPlayer->ammo_uranium <= 0 )
		return FALSE;

	return TRUE;
}

void CEgon::UseAmmo( int count )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= count )
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;
	else
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
}

void CEgon::Attack( void )
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		
		if ( m_fireState != FIRE_OFF || m_pBeam )
		{
			EndAttack();
		}
		else
		{
			PlayEmptySound( );
		}
		return;
	}

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );


	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if ( !HasAmmo() )
			{
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
				PlayEmptySound( );
				return;
			}

			m_flAmmoUseTime = gpGlobals->time;// start using ammo ASAP.

			PLAYBACK_EVENT_FULL( FEV_NOTHOST, m_pPlayer->edict(), m_usEgonFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_fireState, m_fireMode, 1, 0 );
						
			m_shakeTime = 1.5;

			m_pPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
			pev->fuser1	= UTIL_WeaponTimeBase() + 0.2;

			pev->dmgtime = gpGlobals->time + GetPulseInterval();
			m_fireState = FIRE_CHARGE;
		}
		break;

		case FIRE_CHARGE:
		{
			Fire( vecSrc, vecAiming );
			
			m_pPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;
		
			if ( pev->fuser1 <= UTIL_WeaponTimeBase() )
			{
				PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), m_usEgonFire, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_fireState, m_fireMode, 0, 0 );
				pev->fuser1 = 1000;
			}

			if ( !HasAmmo() )
			{
				EndAttack();
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
			}

		}
		break;
	}
//			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 12.9;	
//		m_fireState = FIRE_CHARGE;
}




void CEgon::PrimaryAttack( void )
{
	m_fireMode = FIRE_WIDE;
	Attack();


}

void CEgon::SecondaryAttack(void)
{
	
	#ifndef CLIENT_DLL
		m_pPlayer->pev->velocity = gpGlobals->v_forward * -260;	//fly!
		m_fireState = FIRE_CHARGE;
	#endif
		
		if ( gpGlobals->time >= m_flAmmoUseTime )
			{
				UseAmmo( 1 ); //eat ammo
				m_flAmmoUseTime = gpGlobals->time + 1.25; //eat 1 ammo\1.25 sec
				pev->ltime = 0.1;
			}
		//ammo out, end attack
		if ( !HasAmmo() ) 
			{
				EndAttack();
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
			}
}



void CEgon::ThirdAttack( void )
{

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel >= 2)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (pev->ltime >= 200)
		return;
	
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 20)
		return;
	
	pev->ltime += 0.75; //0.5
	
	// Make a lightning strike
	TraceResult	tr;	
	Vector vecSrc;
	vecSrc = m_pPlayer->GetGunPosition( )  + gpGlobals->v_right * 4 + gpGlobals->v_up * -10;
	Vector vecDir = gpGlobals->v_forward;
	// vecDir.x = gpGlobals->v_forward.x * RANDOM_FLOAT(-3,3);
	// vecDir.y = gpGlobals->v_forward.y * RANDOM_FLOAT(-3,3);
	// vecDir.z = gpGlobals->v_forward.z * RANDOM_FLOAT(-3,3);
	UTIL_TraceLine(vecSrc, vecSrc + vecDir * (pev->ltime * 4), dont_ignore_monsters, m_pPlayer->edict(), &tr);

	//lightings 1
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD( tr.vecEndPos.x + RANDOM_FLOAT(-16,16) );
		WRITE_COORD( tr.vecEndPos.y + RANDOM_FLOAT(-16,16) );
		WRITE_COORD( tr.vecEndPos.z + RANDOM_FLOAT(-16,16) );
		WRITE_SHORT( m_LaserSprite );
		WRITE_BYTE( 1 ); // Starting frame
		WRITE_BYTE( 0  ); // framerate * 0.1
		WRITE_BYTE( 1 ); // life * 0.1
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 16 ); // noise
		WRITE_BYTE( pev->ltime*2 ); // color r,g,b
		WRITE_BYTE( pev->ltime*2 ); // color r,g,b
		WRITE_BYTE( 160 ); // color r,g,b
		WRITE_BYTE( 140 ); // brightness
		WRITE_BYTE( 16 ); // scroll speed
	MESSAGE_END();
		

	::RadiusDamage( tr.vecEndPos, pev, VARS( pev->owner ), 3, 26, CLASS_NONE, DMG_SHOCK  ); //end blast
		
	UTIL_DecalTrace( &tr, DECAL_GUNSHOT1+RANDOM_LONG(1,3) );
		
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.03;
	
	EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart15.wav", 0.75, ATTN_NORM, 1.0, pev->ltime );

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 20 && pev->ltime >= 100.0)
    {
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=20;
		//EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart11.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,100) );
		CBaseEntity *pTes = Create( "func_particle_system", m_pPlayer->GetGunPosition( ), m_pPlayer->pev->v_angle, m_pPlayer->edict() );
		SendWeaponAnim( 3 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.5;
		m_flNextSecondaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.5;
		pev->ltime = 0.1;
	}
	

}

void CEgon::FourthAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel >= 2)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}
	
	if (pev->ltime >= 200)
		return;
	
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 195)
		return;
		
	if (  m_pPlayer->m_flNextChatTime15 > gpGlobals->time ) //need delay
		return;
	
	pev->ltime += 0.75; //0.5
	
	EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "ambience/alien_minddrill.wav", 1.0, ATTN_NORM, 1.0, pev->ltime );
	
	//lights
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);	// X
		WRITE_COORD(pev->origin.y);	// Y
		WRITE_COORD(pev->origin.z);	// Z
		WRITE_BYTE( 32 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 0 );		// g
		WRITE_BYTE( pev->ltime );		// b
		WRITE_BYTE( 128 );		// life * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );
	
	//STOP_SOUND( ENT(pev), CHAN_VOICE, "ambience/alien_minddrill.wav" );

	
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.03;
	//SendWeaponAnim( 1 );
	
	//the result
	if (pev->ltime >= 128.0)
    {

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
		EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart11.wav", 0.75, ATTN_NORM, 1.0, 100 );

		CBaseEntity *pStorm = Create( "power_gloves", pev->origin, Vector(0,0,0), m_pPlayer->edict() );
		pStorm->pev->velocity = gpGlobals->v_forward * 64;
		
		SendWeaponAnim( 3 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.5;
		m_flNextSecondaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.5;
		pev->ltime = 0.1;
		m_pPlayer->m_flNextChatTime15 = gpGlobals->time + 120.5;
		
		char  szText[64];
		hudtextparms_t hText;
		sprintf(szText, "%s .\n", "Wait 2 minutes. Reloading."); //game text
		memset(&hText, 0, sizeof(hText));
		hText.channel = 12;
		//range by 0.0 to 1.0
		hText.x = 0.90;
		hText.y = 0.85;
		hText.effect = 1; // Fade in/out
		hText.r1 = hText.g1 = hText.b1 = 255;
		hText.a1 = 255;
		hText.r2 = hText.g2 = hText.b2 = 255;
		hText.a2 = 255;
		hText.fadeinTime = 1.5;
		hText.fadeoutTime = 1.5;
		hText.holdTime = 3.0;
		hText.fxTime = 0.5;
		if (  m_pPlayer )
			UTIL_HudMessage(m_pPlayer, hText, szText);
	}


		
		

}


BOOL CEgon::Deploy( void )
{

	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 262 );
	m_deployed = FALSE;
	m_fireState = FIRE_OFF;
	return DefaultDeploy( "models/v_egon.mdl", "models/p_egon.mdl", EGON_DRAW, "egon" );
}

		
void CEgon::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	Vector vecDest = vecOrigSrc + vecDir * 2048;
	edict_t		*pentIgnore;
	TraceResult tr;

	pentIgnore = m_pPlayer->edict();
	Vector tmpSrc = vecOrigSrc + gpGlobals->v_up * -8 + gpGlobals->v_right * 3;

	// ALERT( at_console, "." );
	
	UTIL_TraceLine( vecOrigSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr );

	

#ifndef CLIENT_DLL
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	if (pEntity == NULL)
		return;

	if ( g_pGameRules->IsMultiplayer() )
	{
		if ( m_pSprite )
		{
			m_pSprite->pev->effects &= ~EF_NODRAW;
		}

	}


#endif

	float timedist;

	switch ( m_fireMode )
	{
	case FIRE_NARROW:
#ifndef CLIENT_DLL
		if ( pev->dmgtime < gpGlobals->time )
		{
			// Narrow mode only does damage to the entity it hits
			ClearMultiDamage();
			if (pEntity->pev->takedamage)
			{
				pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgEgonNarrow, vecDir, &tr, DMG_BULLET );
			}
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// multiplayer uses 1 ammo every 1/10th second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.1;
				}
			}
			else
			{
				// single player, use 3 ammo/second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.166;
				}
			}

			pev->dmgtime = gpGlobals->time + GetPulseInterval();
		}
#endif
		timedist = ( pev->dmgtime - gpGlobals->time ) / GetPulseInterval();
		break;
	
	case FIRE_WIDE:
#ifndef CLIENT_DLL
		if ( pev->dmgtime < gpGlobals->time )
		{
			// wide mode does damage to the ent, and radius damage
			ClearMultiDamage();
			if (pEntity->pev->takedamage)
			{
				pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgEgonWide, vecDir, &tr, DMG_BULLET);
			}
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// radius damage a little more potent in multiplayer.
				::RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, gSkillData.plrDmgEgonWide/4, 128, CLASS_NONE, DMG_BULLET );
			}

			if ( !m_pPlayer->IsAlive() )
				return;

			if ( g_pGameRules->IsMultiplayer() )
			{
				//multiplayer uses 5 ammo/second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.2;
				}
			}
			else
			{
				// Wide mode uses 10 charges per second in single player
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.1;
				}
			}

			pev->dmgtime = gpGlobals->time + GetDischargeInterval();
			if ( m_shakeTime < gpGlobals->time )
			{
				UTIL_ScreenShake( tr.vecEndPos, 5.0, 150.0, 0.75, 250.0 );
				m_shakeTime = gpGlobals->time + 1.5;
			}
		}
#endif
		timedist = ( pev->dmgtime - gpGlobals->time ) / GetDischargeInterval();
		break;
	}

	if ( timedist < 0 )
		timedist = 0;
	else if ( timedist > 1 )
		timedist = 1;
	timedist = 1-timedist;

	UpdateEffect( tmpSrc, tr.vecEndPos, timedist );
}


void CEgon::UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend )
{
	if ( !m_pBeam )
	{
		CreateEffect();
	}

	m_pBeam->SetStartPos( endPoint );
	m_pBeam->SetBrightness( 170 - (timeBlend*180) );
	m_pBeam->SetWidth( 45 - (timeBlend*20) );

	if ( m_fireMode == FIRE_WIDE )
		m_pBeam->SetColor( 200 + (25*timeBlend), 3 + (30*timeBlend), 3 + 80*fabs(sin(gpGlobals->time*10)) );
	else
		m_pBeam->SetColor( 150 + (25*timeBlend), 1 + (30*timeBlend), 200 + 80*fabs(sin(gpGlobals->time*10)) );


	UTIL_SetOrigin( m_pSprite->pev, endPoint );
	m_pSprite->pev->frame += 8 * gpGlobals->frametime;
	if ( m_pSprite->pev->frame > m_pSprite->Frames() )
		m_pSprite->pev->frame = 0;

	m_pNoise->SetStartPos( endPoint );

}

void CEgon::CreateEffect( void )
{
	DestroyEffect();

	m_pBeam = CBeam::BeamCreate( EGON_BEAM_SPRITE, 40 );
	m_pBeam->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pBeam->SetFlags( BEAM_FSINE );
	m_pBeam->SetEndAttachment( 1 );
	m_pBeam->pev->spawnflags |= SF_BEAM_TEMPORARY;	// Flag these to be destroyed on save/restore or level transition

	m_pNoise = CBeam::BeamCreate( EGON_BEAM_SPRITE, 55 );
	m_pNoise->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pNoise->SetScrollRate( 128 );
	m_pNoise->SetBrightness( 255 );
	m_pNoise->SetEndAttachment( 1 );
	m_pNoise->pev->spawnflags |= SF_BEAM_TEMPORARY;

	m_pSprite = CSprite::SpriteCreate( EGON_FLARE_SPRITE, pev->origin, FALSE );
	m_pSprite->pev->scale = 1.0;
	m_pSprite->SetTransparency( kRenderGlow, 255, 55, 55, 255, kRenderFxNoDissipation );
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;

	if ( m_fireMode == FIRE_WIDE )
	{
		m_pBeam->SetScrollRate( 60 );
		m_pBeam->SetNoise( 3 );
		m_pNoise->SetColor( 25,34,176 );
		m_pNoise->SetNoise( 12 );
	}
	else
	{
		m_pBeam->SetScrollRate( 7 );
		m_pBeam->SetNoise( 5 );
		m_pNoise->SetColor( 216, 53, 147 );
		m_pNoise->SetNoise( 5 );
	}
}


void CEgon::DestroyEffect( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
	if ( m_pNoise )
	{
		UTIL_Remove( m_pNoise );
		m_pNoise = NULL;
	}
	if ( m_pSprite )
	{
		if ( m_fireMode == FIRE_WIDE )
			m_pSprite->Expand( 10, 500 );
		else
			UTIL_Remove( m_pSprite );
		m_pSprite = NULL;
	}
}



void CEgon::WeaponIdle( void )
{
	ResetEmptySound( );
	
	STOP_SOUND( ENT(pev), CHAN_VOICE, "ambience/alien_minddrill.wav" );
	
	if (pev->ltime != 0.1)
		{
			pev->ltime = 0.1;
			EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart15.wav", 0.0, ATTN_NORM, 1.0, 100 );
		}
	
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if ( m_fireState != FIRE_OFF )
		 EndAttack();
	
	int iAnim;

	float flRand = RANDOM_FLOAT(0,1);

	if ( flRand <= 0.5 )
	{
		iAnim = EGON_IDLE1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
	else 
	{
		iAnim = EGON_FIDGET1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	}

	SendWeaponAnim( iAnim );
	m_deployed = TRUE;
	


	//new code - launch big bomb by pressed Reload key
	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 25) 
		{
		if (  m_pPlayer->m_flNextChatTime10 < gpGlobals->time ) //need delay
			{
				//EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "debris/beamstart8.wav", 0.9, ATTN_NORM); //play sound
				EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "debris/beamstart8.wav", 0.75, ATTN_NORM, 1.0, 90 );
				m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
				Vector vecSrc = m_pPlayer->pev->origin;
				Vector vecThrow = gpGlobals->v_forward * 900; //init and start speed of core v 700, 600

				#ifndef CLIENT_DLL
					CBaseEntity *pSatchel = Create( "weapon_frag", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
					pSatchel->pev->velocity = vecThrow;
					m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
					m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 25;
					m_pPlayer->m_flNextChatTime10 = gpGlobals->time + 1.5;
					m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
				#endif
				return;
			}
		}
	

}



void CEgon::EndAttack( void )
{
	bool bMakeNoise = false;
		
	if ( m_fireState != FIRE_OFF ) //Checking the button just in case!.
		 bMakeNoise = true;

	PLAYBACK_EVENT_FULL( FEV_GLOBAL | FEV_GLOBAL, m_pPlayer->edict(), m_usEgonStop, 0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, bMakeNoise, 0, 0, 0 );

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;

	m_fireState = FIRE_OFF;

	DestroyEffect();
}



class CEgonAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_chainammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_chainammo.mdl");
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
LINK_ENTITY_TO_CLASS( ammo_egonclip, CEgonAmmo );

#endif

////////////////////////////////////////////////
//////////////NEW weapon, <1.30 changed/////////
////////////////////////////////////////////////

class   CBfb : public CBaseEntity
{
        public:

        void    Spawn           (void);
		void Precache 			(void);
        void    MoveThink       (void);
		int m_flDie;
		int m_flDie2;
		int     BeamSprite;
		int m_iSpriteTexture;
		short		m_LaserSprite;
		short g_sModelIndexFireball2;
		int m_iBalls;
		
		
		static CBfb* Create( Vector, Vector, CBaseEntity* );
		void EXPORT Hit         ( CBaseEntity* );
		
};

LINK_ENTITY_TO_CLASS( weapon_frag, CBfb );
LINK_ENTITY_TO_CLASS( func_particle_system, CTes ); //1.30 teslagun




void    CBfb :: Spawn( void )
{
		Precache( );
		
        SET_MODEL( ENT(pev), "models/alt_nuke2.mdl" );
        pev->movetype = MOVETYPE_TOSS;
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector(-1,-1,-1), Vector(1,1,1) );
		pev->angles.x = -(pev->angles.x);
        UTIL_SetOrigin( pev, pev->origin );
        //pev->classname = MAKE_STRING( "BFBomb" );
		pev->classname = MAKE_STRING( "weapon_egon" );
		m_flDie = gpGlobals->time + 10;
		pev->dmg = 100;
		pev->nextthink = gpGlobals->time + 0.3;
		pev->gravity = 0.70;
		pev->effects |= EF_LIGHT;
		
		SetTouch( Hit );
		SetThink(MoveThink);
		EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart15.wav", 0.75, ATTN_NORM, 1.0, pev->ltime );
		
		
}

void CBfb :: Precache( void )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	PRECACHE_MODEL( "models/alt_nuke2.mdl" );
	m_iBalls = PRECACHE_MODEL( "sprites/gradbeam.spr" );
	g_sModelIndexFireball2 = PRECACHE_MODEL( "sprites/FrzBoom1.spr" );

}

///////////////
CBfb* CBfb :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CBfb* Beam = GetClassPtr( (CBfb*)NULL );
        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CBfb :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}

void    CBfb :: Hit( CBaseEntity* Target )
{
	TraceResult TResult;
	Vector      StartPosition;
	pev->enemy = Target->edict( );
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;
	

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					dont_ignore_monsters,
					ENT( pev ),
					&TResult );
					
	CBaseEntity *pEntity = NULL;
		
   
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 225 )) != NULL)
		 {
			UTIL_ScreenShake( pEntity->pev->origin, 1024.0, 1.5, 1.5, 1 );
			pEntity->TakeDamage(pev, VARS( pev->owner ), pev->dmg * 0.5, DMG_MORTAR); //destroy all near thinks
		 }
		
	//full explode after touch with wall
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 750, 50, CLASS_NONE, DMG_BULLET  ); //end blast

	//lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 64 );		// radius * 0.1
		WRITE_BYTE( 128 );		// r
		WRITE_BYTE( 16 );		// g
		WRITE_BYTE( 0 );		// b
		WRITE_BYTE( 128 );		// life * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );

	// balls
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( pev->origin.x + TResult.vecPlaneNormal.x*3 );
		WRITE_COORD( pev->origin.y  + TResult.vecPlaneNormal.y*3 );
		WRITE_COORD( pev->origin.z + TResult.vecPlaneNormal.z*3  );
		WRITE_SHORT( m_iBalls );		// model
		WRITE_BYTE( 20  );				// count
		WRITE_BYTE( 1 );				// life * 10
		WRITE_BYTE( 1 );				// size * 10
		WRITE_BYTE( 64 );				// amplitude * 0.1
		WRITE_BYTE( 2 );				// speed * 100
	MESSAGE_END();
		

	
	// random explosions
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_SHORT( g_sModelIndexFireball2 );
		WRITE_BYTE( pev->dmg ); // * 0.75
		WRITE_BYTE( 16  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();
	
	pev->movetype = MOVETYPE_BOUNCE;
	
	pev->velocity = pev->velocity/2;
	
	pev->dmg -= 20;
	
	if (pev->dmg < 50)
		UTIL_Remove( this );
		
}
/////////////////


void    CBfb :: MoveThink( void )
{
	//set trails
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 40 ); // life
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 128 ); // r, g, b
		WRITE_BYTE( 60 ); // r, g, b
		WRITE_BYTE( 0 ); // r, g, b
		WRITE_BYTE( 24 ); // brightness
	MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	pev->angles = UTIL_VecToAngles (pev->velocity);
	
	
	if (gpGlobals->time >= m_flDie) //full explode and self destroy
		{
			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 50, 50, CLASS_NONE, DMG_BULLET  ); //end blast
			UTIL_Remove( this );
		}
	pev->nextthink = gpGlobals->time + 0.5;

}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void CTes::Spawn( void )
{
	m_LaserSprite = PRECACHE_MODEL( "sprites/bolt1.spr" );
	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_egon" );


	if ( g_flWeaponCheat != 0)
		m_radius = 512;
	else
		m_radius = 1024;
		

	CBaseEntity *pEntity = NULL;
	
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, m_radius )) != NULL) //2048 old
		{
				
		if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict()) && FVisible( pEntity )) //!(pEntity->pev->movetype == MOVETYPE_FLY)
			{
				EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "debris/beamstart15.wav", 0.75, ATTN_NORM, 1.0, RANDOM_LONG(90,100) );
				/////beam ray
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
					WRITE_BYTE( 5 ); // life * 0.1
					WRITE_BYTE( 25 ); // width
					WRITE_BYTE( 10 ); // noise
					WRITE_BYTE( 20 ); // color r,g,b
					WRITE_BYTE( 200 ); // color r,g,b
					WRITE_BYTE( 255 ); // color r,g,b
					WRITE_BYTE( 255 ); // brightness
					WRITE_BYTE( 0 ); // scroll speed
				MESSAGE_END();
				pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(96,102), DMG_SHOCK);	

				//new function, bassed on ::RadiusDamage (<1.30, see combat.cpp)
				pEntity->TeslaExplode(pEntity, pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(96,102), 2048, CLASS_NONE, DMG_MORTAR);
			} 
		}
	UTIL_Remove( this );
}



////////////////////////////////////////////////////////////////////////
///////////////////////////STORM PART///////////////////////////////////
////////////////////////////////////////////////////////////////////////

void CStorm::Spawn( void )
{
	CBaseEntity *pEntity = NULL;
	
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -3, -3, -14), Vector(3, 3, 5) );
	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( ENT(pev), "models/light.mdl" );
	pev->effects = EF_MUZZLEFLASH;
	
	pev->classname = MAKE_STRING( "weapon_egon" );
	
	pev->gravity = 0.025;
	pev->ltime = 0.0;
	
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	m_Sprite = PRECACHE_MODEL( "sprites/WXplo1.spr" );

	
	while (((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 8000 )) != NULL) && pEntity->IsPlayer())
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
		UTIL_ScreenFade( pPlayer, Vector(0,0,0), 30, 0.84, RANDOM_LONG(128,200), FFADE_IN ); //night fade
		UTIL_ShowMessageAll( "The Storm-light created!");
	}
	



	
	
	SetThink(Update);
	pev->nextthink = gpGlobals->time + 3.25;
}

void CStorm::Update( void )
{
	CBaseEntity *pEntity = NULL;
	
	if (pev->effects != EF_LIGHT)
	{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "ambience/sandfall2.wav", 1.0, ATTN_NORM);
		
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
		
		pev->effects |= EF_LIGHT;
		
	}
	
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_FLOAT(-128,128);	// Pick a random direction
	vecEnd.y = RANDOM_FLOAT(-128,128);
	vecEnd.z = RANDOM_FLOAT(-128,128);
	vecEnd = pev->origin + vecEnd.Normalize() * RANDOM_LONG(64,256);
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);

	
	if ( (tr.fAllSolid == NULL) && (tr.fStartSolid == NULL))
	{
		switch( RANDOM_LONG(0,3) )
		{
			case 1:
				{
				//lightings
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
					WRITE_BYTE( TE_BEAMPOINTS );
					WRITE_COORD(pev->origin.x);
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_COORD( vecEnd.x );
					WRITE_COORD( vecEnd.y );
					WRITE_COORD( vecEnd.z );
					WRITE_SHORT( m_LaserSprite );
					WRITE_BYTE( 0 ); // Starting frame
					WRITE_BYTE( 16  ); // framerate * 0.1
					WRITE_BYTE( 1 ); // life * 0.1
					WRITE_BYTE( 8 ); // width
					WRITE_BYTE( 64 ); // noise
					WRITE_BYTE( 200 ); // color r,g,b
					WRITE_BYTE( 200 ); // color r,g,b
					WRITE_BYTE( 255 ); // color r,g,b
					WRITE_BYTE( 175 ); // brightness
					WRITE_BYTE( 8 ); // scroll speed
				MESSAGE_END();
				
				while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 800 )) != NULL)
					{
						if (pEntity && !(pEntity->pev->health <= 3))
						{
							UTIL_ScreenFade( pEntity, Vector(255,255,250), 2, 0.84, 128, FFADE_IN ); //strong flash 
							pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(14,37), DMG_SHOCK);
							::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(3,9), 512, CLASS_NONE, DMG_MORTAR  );
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
								WRITE_BYTE( 250 ); // color r,g,b
								WRITE_BYTE( 250 ); // color r,g,b
								WRITE_BYTE( 255 ); // color r,g,b
								WRITE_BYTE( 160 ); // brightness
								WRITE_BYTE( 256 ); // scroll speed
							MESSAGE_END();
						}
					
					}
				
				CBaseEntity *pSBeam = Create( "asl_charge_flare", tr.vecEndPos, vecEnd, pev->owner ); 
				pSBeam->pev->velocity = gpGlobals->v_forward * RANDOM_LONG(64,512);
				
				pev->velocity.x += RANDOM_LONG(-20,20);
				pev->velocity.y += RANDOM_LONG(-20,20);
				//pev->velocity.z = 3; //jump 
				
				
				// animated sprite
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITE );
					WRITE_COORD( pev->origin.x );
					WRITE_COORD( pev->origin.y );
					WRITE_COORD( pev->origin.z );
					WRITE_SHORT( m_Sprite );
					WRITE_BYTE( 24 ); // scale * 10
					WRITE_BYTE( 200 ); // brightness
				MESSAGE_END();
				
				// lots of smoke
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SMOKE );
					WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -95, 95 ));
					WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -95, 95 ));
					WRITE_COORD( pev->origin.z + RANDOM_FLOAT( -5, 25 ));
					WRITE_SHORT( g_sModelIndexSmoke );
					WRITE_BYTE( 100 ); // scale * 10
					WRITE_BYTE( 10  ); // framerate
				MESSAGE_END();
				
				break;
				}
		}
	}
	
	pev->ltime += 1.0;
	
	if (pev->ltime >= 64.0)
	{
		// animated sprite
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( m_Sprite );
			WRITE_BYTE( 64 ); // scale * 10
			WRITE_BYTE( 200 ); // brightness
		MESSAGE_END();
		
		UTIL_Remove( this );
	}

	pev->nextthink = gpGlobals->time + 0.25;
	
}


void CStormBeam::Spawn( void )
{
	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 0.025;
	pev->friction = 0.15;
	
	pev->solid = SOLID_NOT;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
	UTIL_SetOrigin( pev, pev->origin );
	
	pev->classname = MAKE_STRING( "weapon_egon" );
	
	//sprite 
	SET_MODEL( ENT(pev), "sprites/blueflare2.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/gradbeam.spr" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	//effects
	pev->rendermode = kRenderTransAdd;
	pev->rendercolor.x = 0;  // red
	pev->rendercolor.y = 100;  // green
	pev->rendercolor.z = 100; // blue
	pev->renderamt = 250;
	
	//

	SetThink(Update);
	pev->nextthink = gpGlobals->time + 3.25;
}

void CStormBeam::Update( void )
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
	
	pev->rendercolor.x += 8;
	pev->scale += 0.15;
	
	if (pev->rendercolor.x >= 255)
		{
			while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 612 )) != NULL)
			{
				if (pEntity && !(pEntity->pev->health <= 3))
				{
					::RadiusDamage( pev->origin, pev, VARS( pev->owner ), RANDOM_LONG(15,49), 256, CLASS_NONE, DMG_MORTAR  );
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
						WRITE_BYTE( 250 ); // color r,g,b
						WRITE_BYTE( 250 ); // color r,g,b
						WRITE_BYTE( 255 ); // color r,g,b
						WRITE_BYTE( 160 ); // brightness
						WRITE_BYTE( 256 ); // scroll speed
					MESSAGE_END();
					
					UTIL_ScreenShake( pEntity->pev->origin, 12.0, 90.5, 0.3, 1 );
					UTIL_ScreenFade( pEntity, Vector(255,255,250), 1, 0.84, 128, FFADE_IN ); //flash 
					pEntity->TakeDamage(pev, VARS( pev->owner ), RANDOM_LONG(21,39), DMG_MORTAR);	

				}
			
			}
		// balls
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( pev->origin.x + tr.vecPlaneNormal.x*3 );
			WRITE_COORD( pev->origin.y  + tr.vecPlaneNormal.y*3 );
			WRITE_COORD( pev->origin.z + tr.vecPlaneNormal.z*3  );
			WRITE_SHORT( m_iBalls );		// model
			WRITE_BYTE( 2  );				// count
			WRITE_BYTE( 1 );				// life * 10
			WRITE_BYTE( 1 );				// size * 10
			WRITE_BYTE( 64 );				// amplitude * 0.1
			WRITE_BYTE( 8 );				// speed * 100
		MESSAGE_END();
		
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