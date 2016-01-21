/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
********************************************************************************
*	Generic Gauss firing routines
*	Written by David Flor (dflor@mach3.com), (c) 1999 Mach III Enterprises
*		http://www.planethalflife.com/mach3/
*
*	Virtually IDENTICAL to "CGauss::Fire" from "gauss.cpp"...
********************************************************************************/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gauss_fire.h"

void GaussFireExt(entvars_t *pSrc, edict_t *pIgnore, Vector vecOrigSrc, 
				  Vector vecDir, float flDamage, gaussfire_s *gf_data )
{
	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;
	TraceResult tr, beam_tr;
	float flMaxFrac = 14.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 10;
	int	nMaxHits = 260;

	int m_iBeam = PRECACHE_MODEL( "sprites/smoke.spr" );
	int m_iGlow = PRECACHE_MODEL( "sprites/hotglow.spr" );
	int m_iBalls = PRECACHE_MODEL( "sprites/hotglow.spr" );

	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		// ALERT( at_console, "." );
		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pIgnore, &tr);

		if (tr.fAllSolid)
			break;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (pEntity == NULL)
			break;

		// draw beam
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecSrc );
			WRITE_BYTE( TE_BEAMPOINTS);
			WRITE_COORD( vecSrc.x);
			WRITE_COORD( vecSrc.y);
			WRITE_COORD( vecSrc.z);
			WRITE_COORD( tr.vecEndPos.x);
			WRITE_COORD( tr.vecEndPos.y);
			WRITE_COORD( tr.vecEndPos.z);
			WRITE_SHORT( m_iBeam );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 3 ); // life
			WRITE_BYTE( gf_data->beam_width );  // width
			WRITE_BYTE( 8 );   // noise
			// primary shot always looks full intensity
			WRITE_BYTE( RANDOM_LONG(3,255) );   // r, g, b
			WRITE_BYTE( gf_data->green );   // r, g, b
			WRITE_BYTE( gf_data->blue );   // r, g, b
			WRITE_BYTE( gf_data->brightness );	// brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();
		nTotal += 26;

		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, vecSrc, NORMAL_EXPLOSION_VOLUME, 3.0 );
		DecalGunshot( &tr, BULLET_MONSTER_12MM );
		nTotal += 19;

		// bounce wall glow
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
			WRITE_BYTE( TE_GLOWSPRITE );
			WRITE_COORD( tr.vecEndPos.x);		// pos
			WRITE_COORD( tr.vecEndPos.y);
			WRITE_COORD( tr.vecEndPos.z);
			WRITE_SHORT( m_iGlow );				// model
			WRITE_BYTE( 60 );	// life * 10
			WRITE_BYTE( 10 );					// size * 10
			WRITE_BYTE( flDamage );			// brightness
		MESSAGE_END();
		nTotal += 13;

		// balls
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
			WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
			WRITE_COORD( tr.vecEndPos.x );
			WRITE_COORD( tr.vecEndPos.y );
			WRITE_COORD( tr.vecEndPos.z );
			WRITE_COORD( tr.vecEndPos.x + tr.vecPlaneNormal.x );
			WRITE_COORD( tr.vecEndPos.y + tr.vecPlaneNormal.y );
			WRITE_COORD( tr.vecEndPos.z + tr.vecPlaneNormal.z );
			WRITE_SHORT( m_iBalls );		// model
			WRITE_BYTE( 7  );				// count
			WRITE_BYTE( 10 );				// life * 10
			WRITE_BYTE( RANDOM_LONG( 1, 2 ) );				// size * 10
			WRITE_BYTE( 10 );				// amplitude * 0.1
			WRITE_BYTE( 1 );				// speed * 100
		MESSAGE_END();
		nTotal += 21;

		EMIT_SOUND_DYN(ENT(pSrc), CHAN_WEAPON, "weapons/gauss2.wav", 
			0.5 + flDamage * (1.0 / 400.0), ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f)); 

		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( pSrc, flDamage, vecDir, &tr, DMG_BULLET );
			ApplyMultiDamage(pSrc, pSrc);
		}

		vecSrc = tr.vecEndPos + vecDir;
		pIgnore = ENT( pEntity->pev );
	}
}

void GaussFire(entvars_t *pSrc, edict_t *pIgnore, Vector vecOrigSrc, 
			   Vector vecDir, float flDamage )
{
	gaussfire_s	gf_data;

	memset(&gf_data, 0, sizeof(gf_data));
	gf_data.red = 255;
	gf_data.green = 0;
	gf_data.blue = 0;
	gf_data.brightness = 128;
	gf_data.beam_width = 20;

	GaussFireExt(pSrc, pIgnore, vecOrigSrc, vecDir, flDamage, &gf_data);
}