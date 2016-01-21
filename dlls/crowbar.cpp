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
#include "gamerules.h"
#include "soundent.h"
#include "shake.h"
#include "func_break.h"
#include "decals.h"
#include "game.h"

#define	CROWBAR_BODYHIT_VOLUME 128
#define	CROWBAR_WALLHIT_VOLUME 512



#define BLASTER_BEAM_RED                30
#define BLASTER_BEAM_GREEN              30
#define BLASTER_BEAM_BLUE               255
#define BLASTER_BEAM_BRIGHTNESS 255
#define BLASTER_BEAM_WIDTH      RANDOM_LONG( 2, 3 )
#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"
#define BLASTER_BEAM_SPEED      854
#define BLASTER_BEAM_LENGTH     16
#define BLASTER_BEAM_RANDOMNESS 1
#define BLASTER_OFFSET_FORWARD  0
#define BLASTER_OFFSET_RIGHT    7
#define BLASTER_OFFSET_UP               0


class   CBlasterBeam : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void EXPORT Hit         ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static CBlasterBeam* Create( Vector, Vector, CBaseEntity* );
		int m_iSpriteTexture;
		int m_iSpriteTexture2;
        int     BeamSprite;
		int m_flDie;
		

};


class   CRc2 : public CGrenade
{
        public:
        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
		void    MoveThink2      ( );
        void 	EXPORT Hit         ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static 	CRc2* Create( Vector, Vector, CBaseEntity* );
        int     BeamSprite;
		int m_iSpriteTexture;
		int m_iSpriteTexture2;
		int m_flDie;
};

LINK_ENTITY_TO_CLASS( weapon_rocketlauncher, CRc2 );
LINK_ENTITY_TO_CLASS( weapon_crowbar, CCrowbar );



enum gauss_e {
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW,
	CROWBAR_HOLSTER,
	CROWBAR_ATTACK1HIT,
	CROWBAR_ATTACK1MISS,
	CROWBAR_ATTACK2MISS,
	CROWBAR_ATTACK2HIT,
	CROWBAR_ATTACK3MISS,
	CROWBAR_ATTACK3HIT
};


void CCrowbar::Spawn( )
{
	Precache( );
	m_flNextChatTime11 = gpGlobals->time;
	m_iId = WEAPON_CROWBAR;
	SET_MODEL(ENT(pev), "models/w_crowbar.mdl");
	FallInit();// get ready to fall down.
	
}



void CCrowbar::Precache( void )
{
	PRECACHE_MODEL("models/v_crowbar.mdl");
	PRECACHE_MODEL("models/w_crowbar.mdl");
	PRECACHE_MODEL("models/p_crowbar.mdl");
	PRECACHE_SOUND("weapons/cbar_hit1.wav");
	PRECACHE_SOUND("weapons/cbar_hit2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod1.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod2.wav");
	PRECACHE_SOUND("weapons/cbar_hitbod3.wav");
	PRECACHE_SOUND("weapons/cbar_miss1.wav");
	PRECACHE_MODEL( "sprites/shock.spr" );
	PRECACHE_SOUND("debris/beamstart7.wav");
	PRECACHE_MODEL( "sprites/blast.spr" );
	PRECACHE_MODEL( "sprites/cexplo.spr" );

	m_usCrowbar = PRECACHE_EVENT ( 1, "events/crowbar.sc" );
}

int CCrowbar::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = NULL;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP; //GLOCK_MAX_CLIP
	p->iSlot = 0;
	p->iPosition = 0;
	p->iId = WEAPON_CROWBAR;
	p->iWeight = CROWBAR_WEIGHT;
	return 1;
}

BOOL CCrowbar::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 304 );
	return DefaultDeploy( "models/v_crowbar.mdl", "models/p_crowbar.mdl", CROWBAR_DRAW, "crowbar" );
}

void CCrowbar::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( CROWBAR_HOLSTER );
}


void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void CCrowbar::Reload( void )
{

}


void CCrowbar::PrimaryAttack()
{
	if (! Swing( 1 ))
	{
	SetThink( SwingAgain );
	pev->nextthink = gpGlobals->time + 0.1;
	}
}


void CCrowbar::SecondaryAttack()
{ 
if (allowmonsters3.value != 1)
	return;

	//if pev->health==100
	//{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
	//m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	if (! Swing( 1 ))
	{
        UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
        Vector GunPosition = m_pPlayer->GetGunPosition( );
        GunPosition = GunPosition + gpGlobals->v_forward * BLASTER_OFFSET_FORWARD;
        GunPosition = GunPosition + gpGlobals->v_right   * BLASTER_OFFSET_RIGHT;
        GunPosition = GunPosition + gpGlobals->v_up      * BLASTER_OFFSET_UP;
        CBlasterBeam* Beam = CBlasterBeam :: Create( GunPosition,
                                                 m_pPlayer->pev->v_angle,
                                                 m_pPlayer );


		float RandomX = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS, BLASTER_BEAM_RANDOMNESS );
		float RandomY = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS, BLASTER_BEAM_RANDOMNESS );
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right * RandomX;
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up    * RandomY;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=3;
		
		
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1;
		SetThink( SwingAgain );
		
	}
	}
	
}



void CCrowbar::ThirdAttack()
{
//new weapon: teleporter. Target is spawn points (info_deathmatch). Linked with subs.cpp 
//1.27
if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]<=0)
	return;

	entvars_t* pevToucher = m_pPlayer->pev; //player object
	edict_t	*pentTarget = NULL; //teleport target

	for ( int i = RANDOM_LONG(1,10); i > 0; i-- )
	pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(5) ); //find targetname at "5"
	
	
	if (FNullEnt(pentTarget)) //not execute, if singleplayer
	   return;	
	
	Vector tmp = VARS( pentTarget )->origin; //teleport to the point

/* 	if ( m_pPlayer->IsPlayer() ) //now need?
	{
		tmp.z -= m_pPlayer->pev->mins.z;// make origin adjustments in case the teleportee is a player. (origin in center, not at feet)
	} */

	tmp.z++;

	pevToucher->flags &= ~FL_ONGROUND;
	
	UTIL_SetOrigin( pevToucher, tmp );

	pevToucher->angles = pentTarget->v.angles;

	if ( m_pPlayer->IsPlayer() )
	{
		pevToucher->v_angle = pentTarget->v.angles;
	}


	pevToucher->velocity = pevToucher->basevelocity = g_vecZero;
 
    
    //delays and sound
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=10;
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_BODY, "debris/beamstart7.wav", 0.9, ATTN_NORM); //play sound
	
}






void CCrowbar::Smack( )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}


void CCrowbar::SwingAgain( void )
{
	Swing( 0 );
}

int CCrowbar::Swing( int fFirst )
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

#ifndef CLIENT_DLL
	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
				vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	PLAYBACK_EVENT_FULL( FEV_GLOBAL| FEV_RELIABLE, m_pPlayer->edict(), m_usCrowbar, 
	0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, 0,
	0.0, 0, 0.0 );


	if ( tr.flFraction >= 1.0 )
	{
		if (fFirst)
		{
			// miss
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
			
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		switch( ((m_iSwing++) % 2) + 1 )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		
		
		//second
	if ( tr.flFraction >= 1.0 )
	{
		if (fFirst)
		{
			// miss
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2;
			
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		switch( ((m_iSwing++) % 2) + 1 )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	}
#ifndef CLIENT_DLL

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage( );

		if ( (m_flNextPrimaryAttack = m_flNextSecondaryAttack + 1 < UTIL_WeaponTimeBase() ) || g_pGameRules->IsMultiplayer() && allowmonsters3.value != 1 )
		{
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgCrowbar*2, gpGlobals->v_forward, &tr, DMG_SLASH|DMG_CRUSH|DMG_MORTAR ); 
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgCrowbar, gpGlobals->v_forward, &tr, DMG_SLASH|DMG_CRUSH|DMG_MORTAR ); 
		}	
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if ( pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE )
			{
				// play thwack or smack sound
				switch( RANDOM_LONG(0,2) )
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hitbod1.wav", 1, ATTN_NORM); break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hitbod2.wav", 1, ATTN_NORM); break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hitbod3.wav", 1, ATTN_NORM); break;
				}
				m_pPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;
				if ( !pEntity->IsAlive() )
					  return TRUE;
				else
					  flVol = 0.1;

				fHitWorld = FALSE;
			}
		}
		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd-vecSrc)*2, BULLET_PLAYER_CROWBAR);

			if ( g_pGameRules->IsMultiplayer() )
			{
				fvolbar = 1;
			}
			switch( RANDOM_LONG(0,1) )
			{
			case 0:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			}
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;
#endif
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
		
		SetThink( Smack );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;
	}
	return fDidHit;
}

//new void (<1.26)
void CCrowbar::WeaponIdle( void )
{

if (allowmonsters3.value != 1)
	return;
//
if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 6) 
	{
	if (  m_pPlayer->m_flNextChatTime11 < gpGlobals->time ) //need delay
	{
        //UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
        Vector GunPosition = m_pPlayer->GetGunPosition( );
        GunPosition = GunPosition + gpGlobals->v_forward * 0;
        GunPosition = GunPosition + gpGlobals->v_right   * 7;
        GunPosition = GunPosition + gpGlobals->v_up      * 0;

	
	
		int flags;
		#ifdef CLIENT_WEAPONS
			flags = FEV_NOTHOST;
		#else
			flags = 0;
		#endif

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
		m_pPlayer->m_flNextChatTime11 = gpGlobals->time + 2;
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		#ifndef CLIENT_DLL
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecThrow = gpGlobals->v_forward;
		CBaseEntity *pHornet = CBaseEntity::Create( "weapon_rocketlauncher", GunPosition, vecThrow, m_pPlayer->edict() );
		#endif

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 6;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;
		//m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		//SendWeaponAnim( CROWBAR_ATTACK1HIT );
		switch( RANDOM_LONG(0,2) )
		{
		case 0:
			SendWeaponAnim( CROWBAR_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( CROWBAR_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( CROWBAR_ATTACK3HIT ); break;
		}
	return;
	}
	}
//reload completed
}




void    CBlasterBeam :: Spawn( )
{
        Precache( );
        SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
        pev->movetype = MOVETYPE_FLY; //So gravity affects it a *tad*
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector(0,0,0), Vector(8,1,1) );//Point sized bounding box
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "Crowbar-rocket" );
        SetThink( MoveThink );
        SetTouch( Hit );
        //pev->angles.x = 0;
        pev->velocity = gpGlobals->v_forward * BLASTER_BEAM_SPEED;
        pev->angles.x = -(pev->angles.x);
        //pev->gravity = 0.1;//A SMIDGEN of gravity. Can you HAVE a smidgen of gravity??
        pev->nextthink = gpGlobals->time + 0.1;//10 times a second
        pev->dmg = 70;
		pev->effects = EF_MUZZLEFLASH;
		//pev->takedamage = DAMAGE_YES;

}

void    CBlasterBeam :: Precache( )
{
        BeamSprite = PRECACHE_MODEL( BLASTER_BEAM_SPRITE );
        PRECACHE_MODEL( "models/rpgrocket.mdl" );
		PRECACHE_SOUND ("weapons/rocket1.wav");
		m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
		m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/blast.spr" );
		//shockwave.spr
}
//We hit something (yay)
void    CBlasterBeam :: Hit( CBaseEntity* Target )
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
        Explode( &TResult, DMG_SLASH|DMG_CRUSH|DMG_MORTAR|DMG_BURN );
}

void    CBlasterBeam :: Explode( TraceResult* TResult, int DamageType )
{
        RadiusDamage( pev,
                      VARS( pev->owner ),
                      pev->dmg,
                      CLASS_NONE,
                      DamageType );
					  
        if( TResult->fAllSolid ) return;
        UTIL_DecalTrace( TResult, DECAL_GUNSHOT2 );
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( m_iSpriteTexture2 ); //other sprite
			WRITE_BYTE( 40  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 350 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 2 ); // life
			WRITE_BYTE( RANDOM_LONG(2,6) );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( RANDOM_LONG(3,255) );   // r, g, b
			WRITE_BYTE( RANDOM_LONG(3,64) );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();
		//pev->takedamage = DAMAGE_NO;
		SUB_Remove( );
}

CBlasterBeam* CBlasterBeam :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CBlasterBeam* Beam = GetClassPtr( (CBlasterBeam*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CBlasterBeam :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}

void    CBlasterBeam :: MoveThink( )
{
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
        MESSAGE_END    		( );
		
		//delete object after 2.5 sec
		pev->nextthink = gpGlobals->time + 2.5;
		SetThink( SUB_Remove );
		
		
}



////////////////////////new/////////



















void    CRc2 :: Spawn( )
{
        Precache( );
		//CBasePlayer *pPlayer;
		//pPlayer = (CBasePlayer *)GET_PRIVATE(pev->owner);
        SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
        pev->movetype = MOVETYPE_FLY;
        pev->solid = SOLID_BBOX;
        UTIL_SetSize( pev, Vector(0,0,0), Vector(8,1,1) );
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "Crowbar-rocket_2" );
        SetThink( MoveThink );
        SetTouch( Hit );
        pev->angles.x = -(pev->angles.x);
        pev->nextthink = gpGlobals->time + 0.1;
		pev->effects = EF_MUZZLEFLASH;
		m_flDie = gpGlobals->time + 10;
		// make rocket sound
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav", 1, 0.4 );
		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMFOLLOW );
			WRITE_SHORT(entindex());	// entity
			WRITE_SHORT(m_iSpriteTexture );	// model
			WRITE_BYTE( RANDOM_LONG(30,46) ); // life
			WRITE_BYTE( 5 );  // width
			WRITE_BYTE( RANDOM_LONG(100,255 ));   // r, g, b
			WRITE_BYTE( RANDOM_LONG(100,255 ));   // r, g, b
			WRITE_BYTE( RANDOM_LONG(100,255 ));   // r, g, b
			WRITE_BYTE( RANDOM_LONG(200,255) );	// brightness
		MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
		
		pev->rendermode = kRenderNormal;
        pev->renderfx = kRenderFxGlowShell;
        pev->rendercolor.x = RANDOM_LONG(100,255 );  // red
        pev->rendercolor.y = 255;  // green
        pev->rendercolor.z = 255; // blue
        pev->renderamt = 100;
		
		//if (pEntity != NULL)
        //pev->velocity = gpGlobals->v_forward * 300 *(pPlayer->pev->health / 50); //*350 test
		pev->velocity = gpGlobals->v_forward * 475; //*350
		//pev->takedamage = DAMAGE_YES;

}

void    CRc2 :: Precache( )
{
		m_iSpriteTexture = PRECACHE_MODEL( "sprites/shock.spr" );
		m_iSpriteTexture2 = PRECACHE_MODEL( "sprites/cexplo.spr" );
}

void    CRc2 :: Hit( CBaseEntity* Target )
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
        Explode( &TResult, DMG_CRUSH|DMG_MORTAR );
		
}

void    CRc2 :: Explode( TraceResult* TResult, int DamageType )
{
STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );
	
//Vector vecSrc = pev->origin + 1.5 * (gpGlobals->v_forward * 21 );
			
			
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 300, CLASS_NONE, DMG_MORTAR|DMG_BULLET  ); //DMG

        if( TResult->fAllSolid ) return;
        UTIL_DecalTrace( TResult, DECAL_GARGSTOMP1 );
		
		//explode
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y + 16);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( m_iSpriteTexture2 );
			WRITE_BYTE( 48  ); // scale
			WRITE_BYTE( 24  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		//beam
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x+120);
			WRITE_COORD( pev->origin.y + 120);
			WRITE_COORD( pev->origin.z + 200 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 16 ); // framerate
			WRITE_BYTE( 2 ); // life
			WRITE_BYTE( 500 );  // width
			WRITE_BYTE( 64 );   // noise
			WRITE_BYTE( 55 );   // r, g, b
			WRITE_BYTE( 55 );   // r, g, b
			WRITE_BYTE( 55 );   // r, g, b
			WRITE_BYTE( 55 ); // brightness
			WRITE_BYTE( 8 );		// speed
		MESSAGE_END();
		pev->takedamage = DAMAGE_NO;
		SUB_Remove( );
}

CRc2* CRc2 :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CRc2* Beam = GetClassPtr( (CRc2*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CRc2 :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;
}

void    CRc2 :: MoveThink( )
{
if (gpGlobals->time >= m_flDie) //time out 10 s
	{
		//explode
		STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 10 + 3  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 400, CLASS_NONE, DMG_MORTAR|DMG_BULLET  ); //DMG
		SUB_Remove( );
	}

			Vector	vecSpitOffset;
			Vector	vecSpitDir;
			vecSpitOffset = ( pev->origin );
			
			vecSpitDir = ( pev->angles );
			vecSpitDir.x = 0;
			vecSpitDir.y = 0;
			vecSpitDir.z = 0;


			
			MoveThink2( );
			
}

void    CRc2 :: MoveThink2( )
{
		pev->velocity.x = (pev->velocity.x + RANDOM_LONG(-90,90));
		//pev->velocity.y = RANDOM_LONG(-100,100);
		pev->velocity.z = (pev->velocity.z + RANDOM_LONG(-20,20));
		pev->angles = UTIL_VecToAngles (pev->velocity);

        SetThink( MoveThink );
        pev->nextthink = gpGlobals->time + 0.1;

}
