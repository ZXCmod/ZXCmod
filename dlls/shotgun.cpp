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
#include "decals.h"
// special deathmatch shotgun spreads
#define VECTOR_CONE_DM_SHOTGUN	Vector( 0.08716, 0.02131, 0.00  )// 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector( 0.17365, 0.04362, 0.00 ) // 20 degrees by 5 degrees

extern float g_flWeaponCheat;



enum shotgun_e {
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

//new weapon when reload attack
 class   CPhase : public CBaseEntity
{
        public:
		
        void    Spawn              ( );
		void    Count              ( void );
        void    Precache           ( );
        void    Motion             ( );
		void 	EXPORT Update      ( );
		
		private:
		int 	m_flDie;
		int 	m_flDie2;
		int 	m_iBalls;
		unsigned short	m_Sprite;
}; 
//result of recharge flare
 class   CPhase2 : public CBaseEntity
{
        public:
		
        void    	Spawn           		 ( );
        void    	Motion       			 ( );
		void    	EXPORT IgniteThink       ( void );
		
		private:
		unsigned short		m_LaserSprite;
		int 		m_flDie;
		int 		value;
		unsigned short		m_Sprite_2;
		unsigned short		m_Sprite_3;
		int 		m_value;
		int 		m_loop;
		BOOL 		m_loop_2;
		

}; 

class   CSCannon : public CGrenade
{
        public:

        void    	Spawn           	   ( void );
        void    	EXPORT MoveThink       ( void );
		void 		EXPORT MoveTouch	   ( CBaseEntity *pOther );
	
		private:
		unsigned short		m_Sprite;
		unsigned short		m_SpriteExp;
		unsigned short		m_iSpriteTexture;
		int         m_timer;
		int 		m_iBodyGibs;
};

LINK_ENTITY_TO_CLASS( weapon_tacgun, CSCannon );
LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );

void CShotgun::Spawn( )
{
	Precache( );
	m_iId = WEAPON_SHOTGUN;
	SET_MODEL(ENT(pev), "models/w_shotgun.mdl");

	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;
	float m_flNextChatTime12 = gpGlobals->time; //delay
	FallInit();// get ready to fall
}


void CShotgun::Precache( void )
{
	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/w_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");
	PRECACHE_MODEL("models/clustergrenade.mdl" );
	
	PRECACHE_SOUND("debris/beamstart10.wav");  
	m_iShell = PRECACHE_MODEL ("models/shotgunshell.mdl");// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/dbarrel1.wav");//shotgun
	PRECACHE_SOUND ("weapons/sbarrel1.wav");//shotgun

	PRECACHE_SOUND ("weapons/reload1.wav");	// shotgun reload
	PRECACHE_SOUND ("weapons/reload3.wav");	// shotgun reload
	
	PRECACHE_SOUND ("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND ("weapons/scock1.wav");	// cock gun
	PRECACHE_SOUND ("ambience/sandfall1.wav");
	PRECACHE_SOUND ("weapons/egon_windup2.wav");
	PRECACHE_SOUND ("zxc/particle.wav");
	
	PRECACHE_SOUND ("zxc/explode3.wav");
	PRECACHE_SOUND ("zxc/explode4.wav");
	PRECACHE_SOUND ("zxc/explode5.wav");
	PRECACHE_SOUND ("ambience/biggun2.wav");
	
	
	
	
	
	BSpr = PRECACHE_MODEL("sprites/laserbeam.spr");

	m_usSingleFire = PRECACHE_EVENT( 1, "events/shotgun1.sc" );
	m_usDoubleFire = PRECACHE_EVENT( 1, "events/shotgun2.sc" );
	PRECACHE_MODEL( "sprites/bigflare1.spr" );
	PRECACHE_MODEL( "sprites/cnt1.spr" );
	PRECACHE_MODEL( "sprites/flare6.spr" );
	PRECACHE_MODEL( "sprites/bldet.spr" );
	PRECACHE_MODEL( "sprites/xflare2.spr" );
	PRECACHE_MODEL( "models/cluster.mdl" );
	
	PRECACHE_MODEL( "sprites/explode1.spr" );
	PRECACHE_MODEL( "sprites/mushroom.spr" );
	PRECACHE_MODEL( "sprites/xenobeam.spr" );
	
	PRECACHE_MODEL( "models/metalplategibs_green.mdl" );
	
	

}

int CShotgun::AddToPlayer( CBasePlayer *pPlayer )
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


int CShotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;

	return 1;
}



BOOL CShotgun::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 270 );
	
	return DefaultDeploy( "models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun" );
}

void CShotgun::PrimaryAttack()
{




	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}
//if ( !(m_pPlayer->pev->button & IN_USE)) //dont shot, if (E) pressed
	//{
	if (m_iClip <= 0)
	{
		Reload( );
		if (m_iClip == 0)
			PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif


	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir;


	vecDir = m_pPlayer->FireBulletsPlayer( 15, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );



	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSingleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );


	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
//	m_flNextThirdAttack = UTIL_WeaponTimeBase() + 0.75;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
	m_fInSpecialReload = 0;
	//}
	//,CLIENT_WEAPONS
}



void CShotgun::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if (m_iClip <= 1)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 2;


	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_GLOBAL;
#else
	flags = 0;
#endif

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir;
	
	vecDir = m_pPlayer->FireBulletsPlayer( 30, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

		
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usDoubleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.95;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6.0;
	else
		m_flTimeWeaponIdle = 1.5;

	m_fInSpecialReload = 0;

}

void CShotgun::ThirdAttack( void )
{
//phase_pulse
//new code
	if (m_iClip <= 0)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	if (  m_pPlayer->m_flNextChatTime12 < gpGlobals->time ) //need delay
		{
		//EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "debris/beamstart4.wav", 0.9, ATTN_NORM); //play sound
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "debris/beamstart10.wav", 0.75, ATTN_NORM, 1.0, 100 );
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		//Vector vecSrc = m_pPlayer->pev->origin;
		

		CBaseEntity *pEntity;
		TraceResult	tr;	
		Vector vecSrc;
		//Vector vecSrc = m_pPlayer->GetGunPosition( );
		vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_right * 9 + gpGlobals->v_up * -10;
		Vector vecDir = gpGlobals->v_forward;
		//UTIL_MakeAimVectors( pev->angles );
		UTIL_TraceLine(vecSrc, vecSrc + vecDir * 4096, dont_ignore_monsters, m_pPlayer->edict(), &tr);
		pEntity = CBaseEntity::Instance(tr.pHit); //trace hit to entity
		
		#ifndef CLIENT_DLL
			CBaseEntity *pSatchel = Create( "phase_pulse", tr.vecEndPos, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
			m_iClip--;
			
			/////beam ray
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD(vecSrc.x);
				WRITE_COORD(vecSrc.y);
				WRITE_COORD(vecSrc.z);
				WRITE_COORD( tr.vecEndPos.x );
				WRITE_COORD( tr.vecEndPos.y );
				WRITE_COORD( tr.vecEndPos.z );
				WRITE_SHORT( BSpr ); //sprite
				WRITE_BYTE( 0 ); // Starting frame
				WRITE_BYTE( 0  ); // framerate * 0.1
				WRITE_BYTE( 2 ); // life * 0.1
				WRITE_BYTE( 38 ); // width
				WRITE_BYTE( 5 ); // noise
				WRITE_BYTE( 120 ); // color r,g,b
				WRITE_BYTE( 255 ); // color r,g,b
				WRITE_BYTE( 120 ); // color r,g,b
				WRITE_BYTE( 225 ); // brightness
				WRITE_BYTE( 100 ); // scroll speed
			MESSAGE_END();
		#endif
		

		
		while ((pEntity = UTIL_FindEntityInSphere( pEntity, tr.vecEndPos, 30 )) != NULL)
		{
			
			if (FClassnameIs( pEntity->pev, "weapon_shotgun"))
			{
				pEntity->pev->ltime+=345; //increase flare energy
				//pEntity->SUB_Remove();
				//return;
			}
		}
	}

}


void CShotgun::FourthAttack( void )
{
	if (m_iClip <= 1)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}
	
	if ( m_iClip >= 2) 
		{
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

		Vector vecThrow = gpGlobals->v_forward * 1600; //800

		#ifndef CLIENT_DLL
			CBaseEntity *pSCannon = Create( "weapon_tacgun", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
			pSCannon->pev->velocity = vecThrow;
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			SendWeaponAnim( 1 );
			m_iClip-=2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
			m_flNextSecondaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "ambience/biggun2.wav", 1.0, ATTN_NORM);
		#endif
		}


}

void CShotgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( SHOTGUN_START_RELOAD );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if (RANDOM_LONG(0,1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

		SendWeaponAnim( SHOTGUN_RELOAD );

		m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void CShotgun::WeaponIdle( void )
{
	ResetEmptySound( );

	//m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
		m_flPumpTime = 0;
	}
	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		}
		else if (m_fInSpecialReload != 0)
		{
			if (m_iClip != 8 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( SHOTGUN_PUMP );
				
				// play cocking sound
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = SHOTGUN_IDLE_DEEP;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}

}





class CShotgunAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_shotbox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_shotbox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_BUCKSHOTBOX_GIVE, "buckshot", BUCKSHOT_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunAmmo );
//new
LINK_ENTITY_TO_CLASS( phase_pulse, CPhase );
LINK_ENTITY_TO_CLASS( item_artifact_super_damage, CPhase2 );








////////////////////
////SHOTGUN CLUSTERS
////////////////////

void CPhase::Spawn( )
{
	Precache();
	SET_MODEL( ENT(pev), "models/clustergrenade.mdl" );
	//
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_shotgun" );
	pev->nextthink = gpGlobals->time + 0.35;
	pev->dmg = 1;
	pev->health = 100;
	//effects
	pev->rendermode = kRenderTransTexture;
	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor.x = 100;  // red
	pev->rendercolor.y = 100;  // green
	pev->rendercolor.z = 255; // blue
	pev->renderamt = 200;
	//
	pev->takedamage = DAMAGE_YES;
	m_flDie = gpGlobals->time + 10;
	pev->ltime = 80;
	SetThink( Update );
	

	//destroy new clusters, if not in radius
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 30 )) != NULL)
		{
			if (FClassnameIs( pEntity->pev, "weapon_shotgun") && (pEntity->edict() != edict()))
			{

				SUB_Remove();
				return;
			}
		}
}



void CPhase::Precache( void )
{

	m_Sprite = PRECACHE_MODEL( "sprites/flare6.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/cnt1.spr" );
	Count( );
}

void CPhase::Count( void  )
{
	TraceResult tr, beam_tr;
	// balls
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z  );
		WRITE_SHORT( m_iBalls );		// model
		WRITE_BYTE( 5  );				// count
		WRITE_BYTE( 5 );				// life * 10
		WRITE_BYTE( RANDOM_LONG( 1, 2 ) );				// size * 10
		WRITE_BYTE( 90 );				// amplitude * 0.1
		WRITE_BYTE( 2 );				// speed * 100
	MESSAGE_END();
}

void CPhase::Motion( void )
{

}




void CPhase::Update( )
{

	pev->ltime -= 3;
	
	if (pev->ltime < 5)
		pev->ltime = 7;
	
	
	// animated sprite
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( m_Sprite );
		WRITE_BYTE( pev->ltime/50 ); // scale * 10
		WRITE_BYTE( pev->ltime/2 ); // brightness
	MESSAGE_END();
	
	
	if (pev->ltime >= 2500) //limit of enegry, when create great beam. Self-destroy too with beam
	{
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 16  ); // scale * 10
			WRITE_BYTE( 30  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		CBaseEntity *pFlare = Create( "item_artifact_super_damage", pev->origin, Vector( 0, 0, 0 ), pev->owner );
		pev->health = -1000;
		SUB_Remove();
	}


	pev->nextthink = gpGlobals->time + 0.3;


	if (gpGlobals->time >= m_flDie || pev->health <= 0 && pev->ltime < 2500) //Simple self destroy without beam
	{
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 15  ); // scale * 10
			WRITE_BYTE( 16  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();

		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 85, 256, CLASS_NONE, DMG_BURN  );
		pev->takedamage = DAMAGE_NO;
		pev->ltime = 0;
		SUB_Remove();
	}

}

//////////////////////////////
//////////////////////////////
//////////////////////////////


void CPhase2::Spawn( )
{
	//CBaseEntity *pEntity;
	CBaseEntity *pEntity = NULL;
	
	SET_MODEL( ENT(pev), "models/clustergrenade.mdl" );
	m_LaserSprite = PRECACHE_MODEL( "sprites/laserbeam.spr" );
	//
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->classname = MAKE_STRING( "weapon_shotgun" );
	pev->nextthink = gpGlobals->time + 3.6;
	pev->dmg = 1;
	pev->effects = EF_MUZZLEFLASH;
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 800, 128, CLASS_NONE, DMG_BURN  ); //destroy all near copyes
	pev->effects |= EF_LIGHT;
	pEntity = CBaseEntity::Instance(pev->owner);
	m_Sprite_2 = PRECACHE_MODEL( "sprites/bldet.spr" );
	m_Sprite_3 = PRECACHE_MODEL( "sprites/xflare2.spr" );
	
	
	EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/egon_windup2.wav", 0.4, ATTN_NORM);

	//find nearest clusters for deleting. No overflow in result.
	if (pEntity != NULL)
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
		
		if (g_flWeaponCheat != 0.0)
			pPlayer->m_flNextChatTime12 = gpGlobals->time + 240; //set reload timer x2
		else
			pPlayer->m_flNextChatTime12 = gpGlobals->time + 120; //set reload timer normal
		
		UTIL_ShowMessageAll( "Shotgun gravity-beam created!"  );
	}
	
	m_value = 0;
	m_loop = 17;
	m_loop_2 = TRUE;
	value = 800; //delta radius
	m_flDie = gpGlobals->time + 22; //destroy timer
	SetThink( IgniteThink );
}

void CPhase2::Motion( void )
{

}


void CPhase2::IgniteThink( void )
{
	
	//init vectors
	CBaseEntity *pEntity = NULL;
	Vector	vecDir;
	vecDir = Vector( 0, 0, 0 );
	Vector vecDirToEnemy;
	
	
	
	// Make a lightning strike
	Vector vecEnd;
	TraceResult tr;
	vecEnd.x = RANDOM_FLOAT(-128,128);	// Pick a random direction
	vecEnd.y = RANDOM_FLOAT(-128,128);
	vecEnd.z = RANDOM_FLOAT(-128,128);
	vecEnd = pev->origin + vecEnd.Normalize() * 2048;
	UTIL_TraceLine( pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);
	
	
	if (gpGlobals->time <= m_flDie - 8) //14 sec 
	{
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 800 )) != NULL)
		{
			//init distance (1.28)
			Vector vecMid = pev->origin; // get self
			Vector vecMidEnemy = pEntity->BodyTarget( vecMid );	//get target
			vecDirToEnemy = vecMidEnemy - vecMid;	// calculate dir and dist to enemy
			float flDistToEnemy = vecDirToEnemy.Length();
			
			if (pEntity->IsPlayer() && !(pEntity->pev->health <= 3))
				{
				
				#ifndef CLIENT_DLL
					vecDir = ( pEntity->Center() - Vector ( 0, 0, 40 ) - Center() ).Normalize();
					pEntity->pev->velocity = pEntity->pev->velocity + vecDir * -(pev->dmg + 72 + value/flDistToEnemy*44); //600 700/16
				#endif
				::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 40, 40, CLASS_NONE, DMG_MORTAR  );
				
				} 
		}
	}
	
	//loop sounds and sprite
	if (m_loop >= 17 && m_loop_2 == TRUE) //12
		{
			EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "zxc/particle.wav", 0.75, ATTN_NORM, 1.0, 100 );
			// animated sprite
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( pev->origin.x );
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_SHORT( m_Sprite_3 );
				WRITE_BYTE( RANDOM_LONG(14,26) ); // scale * 10
				WRITE_BYTE( 200 ); // brightness
			MESSAGE_END();

			m_loop = 0;
		}
		
	if (m_loop_2 == FALSE)
		STOP_SOUND( ENT(pev), CHAN_VOICE, "zxc/particle.wav" );
	
	
	//switch type to lighting
	if (pev->dmg >= 84)
	{

		if (m_value == 0)
		{
			// animated sprite
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( pev->origin.x );
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_SHORT( m_Sprite_2 );
				WRITE_BYTE( 64 ); // scale * 10
				WRITE_BYTE( 200 ); // brightness
			MESSAGE_END();
			
			EMIT_SOUND(ENT(pev), CHAN_BODY, "ambience/sandfall1.wav", 1.0, ATTN_NORM);
			EMIT_SOUND(ENT(pev), CHAN_BODY, "ambience/sandfall1.wav", 1.0, ATTN_NORM);
			m_loop_2 = FALSE;
			m_value = 1;
		}
	
		//lightings
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
			WRITE_BYTE( 8 ); // width
			WRITE_BYTE( 64 ); // noise
			WRITE_BYTE( 200 ); // color r,g,b
			WRITE_BYTE( 200 ); // color r,g,b
			WRITE_BYTE( 255 ); // color r,g,b
			WRITE_BYTE( 175 ); // brightness
			WRITE_BYTE( 8 ); // scroll speed
		MESSAGE_END();

		UTIL_Sparks( tr.vecEndPos );
		::RadiusDamage( tr.vecEndPos, pev, VARS( pev->owner ), 1280, 128, CLASS_NONE, DMG_MORTAR  ); //end blast
		//TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, BULLET_MONSTER_12MM);
		UTIL_DecalTrace( &tr, DECAL_SMALLSCORCH1 + RANDOM_LONG(0,2) );
		
		//lights
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(tr.vecEndPos.x);	// X
			WRITE_COORD(tr.vecEndPos.y);	// Y
			WRITE_COORD(tr.vecEndPos.z);	// Z
			WRITE_BYTE( 8 );		// radius * 0.1
			WRITE_BYTE( 92 );		// r
			WRITE_BYTE( 16 );		// g
			WRITE_BYTE( 16 );		// b
			WRITE_BYTE( 10 );		// life * 10
			WRITE_BYTE( 256 );		// decay * 0.1
		MESSAGE_END( );
	}
	
	
	if (gpGlobals->time >= m_flDie) //full explode and self destroy
		{
		// random explosions
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 35  ); // scale * 10
			WRITE_BYTE( RANDOM_LONG(8,10)  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
		
		::RadiusDamage( pev->origin, pev, VARS( pev->owner ), 100, 300, CLASS_NONE, DMG_BURN  );
		//SUB_Remove();
		UTIL_Remove( this );
		return;
		}

	pev->dmg += 1; //increase grav. power
	m_loop += 1; //sound loop
	pev->nextthink = gpGlobals->time + 0.1; //update rate
	
	
}









////////////////////////////
////////////////////////////
////////////////////////////


void CSCannon::Spawn( void )
{
	SET_MODEL( ENT(pev), "models/cluster.mdl" );
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	UTIL_SetSize( pev, Vector( -4, -4, -4), Vector(4, 4, 4) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->dmg = 150;
	pev->gravity = 0.75;
	m_timer = 0;
	
	pev->classname = MAKE_STRING( "weapon_shotgun" );
	
	m_Sprite    = PRECACHE_MODEL( "sprites/explode1.spr" );
	m_SpriteExp = PRECACHE_MODEL( "sprites/mushroom.spr" );
	m_iBodyGibs = PRECACHE_MODEL( "models/metalplategibs_green.mdl" );
	m_iBodyGibs = PRECACHE_MODEL( "models/metalplategibs_green.mdl" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/xenobeam.spr" );
	
	//trails
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( entindex() );	// entity
		WRITE_SHORT( m_iSpriteTexture );	// model
		WRITE_BYTE( RANDOM_LONG(30,46) ); // life
		WRITE_BYTE( 3 );  // width
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 100 );   // r, g, b
		WRITE_BYTE( 128 );	// brightness
	MESSAGE_END();

	SetTouch( MoveTouch );
	SetThink( MoveThink );
	pev->nextthink = gpGlobals->time + 0.1; 
}

void CSCannon::MoveThink( )
{
	// animated sprite
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( m_Sprite );
		WRITE_BYTE( 3 ); // scale
		WRITE_BYTE( 172 ); // brightness
	MESSAGE_END();


	m_timer += 1;
	if (m_timer >= 40)
	{
		SetTouch( NULL );
		UTIL_Remove( this );
	}

	pev->nextthink = gpGlobals->time + 0.1; 
}

void CSCannon::MoveTouch( CBaseEntity *pOther )
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
		WRITE_BYTE( 60 ); // scale
		WRITE_BYTE( 172 ); // brightness
	MESSAGE_END();
	
	//not worked?
	if ( (pev->flags & FL_ONGROUND) )
	{
		// animated sprite
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( m_SpriteExp );
			WRITE_BYTE( 60 ); // scale
			WRITE_BYTE( 172 ); // brightness
		MESSAGE_END();
	}
	
	//break metals
	Vector vecSpot = pev->origin + (pev->mins + pev->maxs);
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
		WRITE_BYTE( TE_BREAKMODEL);
		WRITE_COORD( vecSpot.x );
		WRITE_COORD( vecSpot.y );
		WRITE_COORD( vecSpot.z + 64);
		WRITE_COORD( 200 );
		WRITE_COORD( 200 );
		WRITE_COORD( 128 );
		WRITE_COORD( 0 ); 
		WRITE_COORD( 0 );
		WRITE_COORD( 200 );
		WRITE_BYTE( 20 ); 
		WRITE_SHORT( m_iBodyGibs );	//model id#
		WRITE_BYTE( 10 );
		WRITE_BYTE( 10 );// 1.0 seconds
		WRITE_BYTE( BREAK_METAL );
	MESSAGE_END();
	
	//lights
	Vector vecSrc = pev->origin + gpGlobals->v_right * 2;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecSrc.x);	// X
		WRITE_COORD(vecSrc.y);	// Y
		WRITE_COORD(vecSrc.z);	// Z
		WRITE_BYTE( 24 );		// radius * 0.1
		WRITE_BYTE( 250 );		// r
		WRITE_BYTE( 250 );		// g
		WRITE_BYTE( 150 );		// b
		WRITE_BYTE( 128 );		// time * 10
		WRITE_BYTE( 16 );		// decay * 0.1
	MESSAGE_END( );
	
	//play sounds
	switch(RANDOM_LONG(0,2))
		{
		case 0: 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/explode3.wav", 1.0, ATTN_NORM); //play sound
		break;
		case 1: 
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/explode4.wav", 1.0, ATTN_NORM); //play sound
		break;
		case 2: 
			EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/explode5.wav", 1.0, ATTN_NORM); //play sound
		break;
		}
		
	UTIL_DecalTrace( &TResult, DECAL_SMALLSCORCH1 + RANDOM_LONG(0,2) );

	
	::RadiusDamage( pev->origin, pev, VARS( pev->owner ), pev->dmg, pev->dmg, CLASS_NONE, DMG_BURN  ); 

	SetTouch( NULL );
	UTIL_Remove( this );

}