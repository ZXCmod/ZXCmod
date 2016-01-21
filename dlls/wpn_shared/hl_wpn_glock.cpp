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

enum glock_e {
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

class   CGB : public CBaseEntity
{
        public:

        void    Spawn           ( void );
		void 	Precache 		( void );
        void    MoveThink       ( void );
		
		
		private:
		
		int 	m_flDie;
		int 	m_flDie2;
		int     BeamSprite;
		int 	m_iSpriteTexture;
		short	m_LaserSprite;
		int 	m_iBalls;
		
		
		static CGB* Create( Vector, Vector, CBaseEntity* );
		void EXPORT Hit         ( CBaseEntity* );
		
};

LINK_ENTITY_TO_CLASS( weapon_minigun, CGB );
LINK_ENTITY_TO_CLASS( weapon_glock, CGlock );
LINK_ENTITY_TO_CLASS( weapon_9mmhandgun, CGlock );


void CGlock::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_9mmhandgun"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_GLOCK;
	SET_MODEL(ENT(pev), "models/w_9mmhandgun.mdl");

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;
	
	m_typeG = 0;

	FallInit();// get ready to fall down.
}


void CGlock::Precache( void )
{
	PRECACHE_MODEL("models/v_9mmhandgun.mdl");
	PRECACHE_MODEL("models/w_9mmhandgun.mdl");
	PRECACHE_MODEL("models/p_9mmhandgun.mdl");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");
	PRECACHE_MODEL("sprites/blflare.spr");
	PRECACHE_MODEL("sprites/blueflare2.spr");
	
	

	PRECACHE_SOUND ("weapons/pl_gun1.wav");//silenced handgun
	PRECACHE_SOUND ("weapons/pl_gun2.wav");//silenced handgun
	PRECACHE_SOUND ("weapons/pl_gun3.wav");//handgun
	//1.27
	PRECACHE_SOUND ("zxc/GaussGun.wav");
	PRECACHE_SOUND ("zxc/Build1.wav");
	PRECACHE_SOUND ("zxc/Build2.wav");
	PRECACHE_SOUND ("zxc/Build3.wav");
	PRECACHE_SOUND ("zxc/Build4.wav");
	PRECACHE_SOUND ("zxc/LsrExpl2.wav");
	//
	m_usFireGlock1 = PRECACHE_EVENT( 1, "events/glock1.sc" );
	m_usFireGlock2 = PRECACHE_EVENT( 1, "events/glock2.sc" );
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



void CGlock::Reload( void )
{
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 17, GLOCK_RELOAD, 1.25 );
	else
		iResult = DefaultReload( 20, GLOCK_RELOAD_NOT_EMPTY, 1.25 );

	if (iResult)
	{
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
	}
	
	// if ( m_pPlayer->pev->fov != 0 )
	// {
		// m_fInZoom = FALSE;
		// m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
	// }
}


BOOL CGlock::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 280 );

	// pev->body = 1;
	return DefaultDeploy( "models/v_9mmhandgun.mdl", "models/p_9mmhandgun.mdl", GLOCK_DRAW, "onehanded", /*UseDecrement() ? 1 : 0*/ 0 );
}

void CGlock::SecondaryAttack( void )
{
	GlockFire( 0.07, 0.2 );
}

void CGlock::PrimaryAttack( void )
{
	GlockFire( 0.035, 0.3 ); //0.25
}

//updated in v1.33 with both types
void CGlock::ThirdAttack( void )
{
	int iResult;

	if (m_iClip <= 4)
		iResult = DefaultReload( 20, GLOCK_RELOAD_NOT_EMPTY, 1.25 );
		
	if (  m_iClip >= 4 ) //need delay
		{
		
		SendWeaponAnim( GLOCK_SHOOT );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		Vector vecSrc = m_pPlayer->pev->origin;
		
		if (  m_typeG == 0 ) //normal
			{
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/GaussGun.wav", 1.0, ATTN_NORM); //play sound
				Vector vecThrow = gpGlobals->v_forward * 700; //init and start speed of core, 540
				#ifndef CLIENT_DLL
					CBaseEntity *pGlockCore = Create( "weapon_minigun", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
					pGlockCore->pev->velocity = vecThrow;
					m_iClip-=4;
					m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
					m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
					m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
				#endif
				return;
			}
		if (  m_typeG == 1 ) //normal // typed as 1 (red trails)
		{
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "zxc/GaussGun.wav", 1.0, ATTN_NORM); //play sound
				Vector vecThrow = gpGlobals->v_forward * 1200; //init and start speed of core, 540
				#ifndef CLIENT_DLL
					CBaseEntity *pGlockCore = Create( "weapon_minigun", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
					pGlockCore->pev->velocity = vecThrow;
					pGlockCore->pev->dmg = 112; 
					m_iClip-=5;
					m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1;
					m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.1;
					m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.1;
				#endif
				return;
		}
		
		
		
		
		}

}

void CGlock::FourthAttack( void )
{
	if ( m_pPlayer->pev->fov != 0 )
	{
		m_fInZoom = FALSE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
		m_flNextSecondaryAttack = 0.3;
		m_typeG = 0;
		//return;
	}
	else if ( m_pPlayer->pev->fov != 60 )
	{
		m_fInZoom = TRUE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 60;
		m_flNextSecondaryAttack = 0.3;
		m_typeG = 1;
		//return;
	}
	PlayEmptySound( );
	m_flNextSecondaryAttack = 0.3;
}

void CGlock::GlockFire( float flSpread , float flCycleTime )
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

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;



	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// silenced
	if (pev->body == 1)
	{
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	}
	else
	{
		// non-silenced
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	}

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	

	vecAiming = gpGlobals->v_forward;
	

#ifndef CLIENT_DLL
	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

	PLAYBACK_EVENT_FULL( FEV_GLOBAL, m_pPlayer->edict(), vecDir ? m_usFireGlock1 : m_usFireGlock2, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, ( m_iClip == 0 ) ? 1 : 0, 0 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}



void CGlock::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.

	if ( m_pPlayer->pev->fov != 0 )
	{
		m_fInZoom = FALSE;
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
		m_flNextSecondaryAttack = 0.3;
		//return;
	}

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	SendWeaponAnim( 3 );
}


void CGlock::WeaponIdle( void )
{
	ResetEmptySound( );

	//m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

		if (flRand <= 0.3 + 0 * 0.75)
		{
			iAnim = GLOCK_IDLE3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 16;
		}
		else if (flRand <= 0.6 + 0 * 0.875)
		{
			iAnim = GLOCK_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
		}
		else
		{
			iAnim = GLOCK_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
		}
		SendWeaponAnim( iAnim, 1 );
	}
	

}





class CGlockAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_9mmclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_9mmclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_GLOCKCLIP_GIVE, "9mm", _9MM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_glockclip, CGlockAmmo );
LINK_ENTITY_TO_CLASS( ammo_9mmclip, CGlockAmmo );








///////////////////
///////////////////







void    CGB :: Spawn( )
{
	Precache( );
	
	SET_MODEL( ENT(pev), "sprites/blflare.spr" );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
	pev->angles.x = -(pev->angles.x);
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_9mmhandgun" );
	m_flDie = gpGlobals->time + 3;
	pev->dmg = 72; //dynamyc value, 85
	pev->takedamage = DAMAGE_YES;
	pev->nextthink = gpGlobals->time + 0.1;
	
	pev->rendermode = kRenderTransAdd; //kRenderTransAlpha
	pev->renderamt = 195;
	
	SetTouch( Hit );
	SetThink( MoveThink );
	pev->health			= 256; 
}

void CGB :: Precache( void )
{
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/shockwave.spr" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/gradbeam.spr" );
}

///////////////
CGB* CGB :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
	CGB* Beam = GetClassPtr( (CGB*)NULL );
	UTIL_SetOrigin( Beam->pev, Pos );
	Beam->pev->angles = Aim;
	Beam->Spawn( );
	Beam->SetTouch( CGB :: Hit );
	Beam->pev->owner = Owner->edict( );
	return Beam;
}

void    CGB :: Hit( CBaseEntity* Target )
{
	TraceResult TResult;
	Vector      StartPosition;
	pev->enemy = Target->edict( );
	StartPosition = pev->origin - pev->velocity.Normalize() * 32;

	UTIL_TraceLine( StartPosition,
					StartPosition + pev->velocity.Normalize() * 64,
					ignore_monsters,
					ENT( pev ),
					&TResult );
   
		
		
	//full explode after touch with wall
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 26, 128, CLASS_NONE, DMG_MORTAR  ); //end blast

	//sprite 
	pev->model = MAKE_STRING("sprites/blueflare2.spr");
	CSprite *pSprite = CSprite::SpriteCreate( "sprites/blueflare2.spr", pev->origin, TRUE );
	if ( pSprite )
		{
			pSprite->pev->nextthink = gpGlobals->time + 2.4;
			pSprite->pev->scale = pev->dmg/40; /// 85 / 40 = 2.125
			pSprite->SetThink( SUB_Remove );
			pSprite->SetTransparency( kRenderTransAdd, 128, 128, 128, 200, kRenderFxGlowShell );
		}
		
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/LsrExpl2.wav", 1.0, ATTN_NORM);
	pev->takedamage = DAMAGE_NO;
	SetThink( SUB_Remove );
}




void    CGB :: MoveThink( )
{

	//set trails
	if (pev->dmg <= 100)
	{
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 24 ); // life
		WRITE_BYTE( 2 ); // width
		WRITE_BYTE( 10 ); // r, g, b
		WRITE_BYTE( 200 ); // r, g, b
		WRITE_BYTE( 10 ); // r, g, b
		WRITE_BYTE( 36 ); // brightness
		MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
	}
	else
	{
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex()); // entity
		WRITE_SHORT(g_sModelIndexSmokeTrail ); // model
		WRITE_BYTE( 30 ); // life
		WRITE_BYTE( 2 ); // width
		WRITE_BYTE( 200 ); // r, g, b
		WRITE_BYTE( 10 ); // r, g, b
		WRITE_BYTE( 10 ); // r, g, b
		WRITE_BYTE( 30 ); // brightness
		MESSAGE_END(); // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)
	}

	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_FLOAT(-360,360);	// Pick a random direction
	vecEnd.y = RANDOM_FLOAT(-360,360);
	vecEnd.z = RANDOM_FLOAT(-360,360);
	vecEnd = pev->origin + vecEnd.Normalize() * pev->dmg/2;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);


	//sounds
	switch(RANDOM_LONG(0,8))
	{
		case 0: EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build1.wav", 0.4, ATTN_NORM); break;
		case 3: EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build2.wav", 0.5, ATTN_NORM); break;
		case 5: EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build3.wav", 0.6, ATTN_NORM); break;
		case 8: EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/Build4.wav", 0.7, ATTN_NORM); break;
	}

	//lightings 1
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS );
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_SHORT( m_LaserSprite );
		WRITE_BYTE( 0 ); // Starting frame
		WRITE_BYTE( 16  ); // framerate * 0.1
		WRITE_BYTE( 1 ); // life * 0.1
		WRITE_BYTE( 3 ); // width
		WRITE_BYTE( 128 ); // noise
		WRITE_BYTE( 64 ); // color r,g,b
		WRITE_BYTE( 128 ); // color r,g,b
		WRITE_BYTE( 255 ); // color r,g,b
		WRITE_BYTE( 140 ); // brightness
		WRITE_BYTE( 16 ); // scroll speed
	MESSAGE_END();
		
	//capture targets
	CBaseEntity *pEntity = NULL;
	Vector	vecDir;
	vecDir = Vector( 0, 0, 0 );
	

	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, pev->dmg )) != NULL)
		{
			if ((pEntity->edict() != pev->owner) && pEntity->pev->takedamage && (pEntity->edict() != edict()) && pEntity->pev->health >= 3) //!(pEntity->pev->movetype == MOVETYPE_FLY)
				{
					if ( FBitSet(pEntity->pev->flags, FL_DUCKING) && (pEntity->pev->flags & FL_ONGROUND) ) 
					{
					::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg/2, pev->dmg, CLASS_NONE, DMG_SHOCK  );
					}
					else
					{
					pEntity->pev->velocity.x = ( (( pev->velocity.x + pev->origin.x) - pEntity->pev->origin.x));
					pEntity->pev->velocity.y = ( (( pev->velocity.y + pev->origin.y) - pEntity->pev->origin.y));
					pEntity->pev->velocity.z = ( (( pev->velocity.z + pev->origin.z) - pEntity->pev->origin.z));
					::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg/3, pev->dmg, CLASS_NONE, DMG_SHOCK  );
					}
				} 
		}

	if (gpGlobals->time >= m_flDie) //full explode and self destroy
		{
			::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 200, CLASS_NONE, DMG_MORTAR  ); //end blast
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/LsrExpl2.wav", 1.0, ATTN_NORM);
			pev->takedamage = DAMAGE_NO;
			SetThink( SUB_Remove );
		}
		
	pev->nextthink = gpGlobals->time + 0.12;
}





