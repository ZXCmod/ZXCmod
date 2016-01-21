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
#include "gamerules.h"
#include "decals.h"
#include "shake.h"

////INIT CRYSTAL

#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"


///class crystal
class   CBlasterBeam4 : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void    EXPORT Hit         ( CBaseEntity* );
        void    Explode         (int);
        static  CBlasterBeam4* Create( Vector, Vector, CBaseEntity* );
		
		private:
        int     BeamSprite;
		int 	m_iSpriteTexture;
		float 	m_flDie;
		float 	m_flDie2;

};




enum satchel_e {
	SATCHEL_IDLE1 = 0,
	SATCHEL_FIDGET1,
	SATCHEL_DRAW,
	SATCHEL_DROP
};

enum satchel_radio_e {
	SATCHEL_RADIO_IDLE1 = 0,
	SATCHEL_RADIO_FIDGET1,
	SATCHEL_RADIO_DRAW,
	SATCHEL_RADIO_FIRE,
	SATCHEL_RADIO_HOLSTER
};



class CSatchelCharge : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	void BounceSound( void );

	void EXPORT SatchelSlide( CBaseEntity *pOther );
	void EXPORT SatchelThink( void );

public:
	void Deactivate( void );
};
LINK_ENTITY_TO_CLASS( monster_satchel, CSatchelCharge );

//1.31 new wp
class CSatchelCharge2 : public CGrenade
{
	void Spawn( void );
	void EXPORT SatchelSlide( CBaseEntity *pOther );
	void EXPORT SatchelThink( void );

public:
	void Deactivate( void );
};
LINK_ENTITY_TO_CLASS( monster_pipebomb, CSatchelCharge2 );

//=========================================================
// Deactivate - do whatever it is we do to an orphaned 
// satchel when we don't want it in the world anymore.
//=========================================================
void CSatchelCharge::Deactivate( void )
{
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;
	UTIL_Remove( this );
}

void CSatchelCharge2::Deactivate( void )
{
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;
	
	UTIL_Remove( this );
}

void CSatchelCharge :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_medkit.mdl");
	//UTIL_SetSize(pev, Vector( -16, -16, -4), Vector(16, 16, 32));	// Old box -- size of headcrab monsters/players get blocked by this
	UTIL_SetSize(pev, Vector( -4, -4, -4), Vector(4, 4, 4));	// Uses point-sized, and can be stepped over
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( SatchelSlide );
	SetUse( DetonateUse );
	SetThink( SatchelThink );
	pev->nextthink = gpGlobals->time + 0.1;

	pev->gravity = 0.5;
	pev->friction = 0.8;


	pev->dmg = gSkillData.plrDmgSatchel;
	// ResetSequenceInfo( );
	pev->sequence = 1;
	
	pev->health = 150;
	pev->takedamage = DAMAGE_AIM;
}


void CSatchelCharge::SatchelSlide( CBaseEntity *pOther )
{
	entvars_t	*pevOther = pOther->pev;

	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// pev->avelocity = Vector (300, 300, 300);
	pev->gravity = 1;// normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin - Vector(0,0,10), ignore_monsters, edict(), &tr );

	if ( tr.flFraction < 1.0 )
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
		// play sliding sound, volume based on velocity
	}
	if ( !(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10 )
	{
		BounceSound();
	}
	StudioFrameAdvance( );
	
}


void CSatchelCharge :: SatchelThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove( this );
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
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}
	else
	{
		pev->velocity.z -= 8;
	}	
	
	

}

void CSatchelCharge :: Precache( void )
{
	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
	PRECACHE_SOUND("zxc/crystal_heal.wav");
	PRECACHE_MODEL( "sprites/shrinkf.spr" );
	
	PRECACHE_MODEL( "models/crystal.mdl" );
	PRECACHE_SOUND("zxc/warningbell1.wav");
	PRECACHE_SOUND("zxc/sg_24.wav");
	
	
	PRECACHE_MODEL( "sprites/xflare1.spr" );
}

void CSatchelCharge :: BounceSound( void )
{
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce1.wav", 1, ATTN_NORM);	break;
	case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce2.wav", 1, ATTN_NORM);	break;
	case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce3.wav", 1, ATTN_NORM);	break;
	}
}




LINK_ENTITY_TO_CLASS( weapon_satchel, CSatchel );


//=========================================================
// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
//=========================================================
int CSatchel::AddDuplicate( CBasePlayerItem *pOriginal )
{
	CSatchel *pSatchel;

#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		pSatchel = (CSatchel *)pOriginal;

		if ( pSatchel->m_chargeReady != 0 )
		{
			// player has some satchels deployed. Refuse to add more.
			return FALSE;
		}
	}

	return CBasePlayerWeapon::AddDuplicate ( pOriginal );
}

//=========================================================
//=========================================================
int CSatchel::AddToPlayer( CBasePlayer *pPlayer )
{
	int bResult = CBasePlayerItem::AddToPlayer( pPlayer );

	pPlayer->pev->weapons |= (1<<m_iId);
	m_chargeReady = 0;// this satchel charge weapon now forgets that any satchels are deployed by it.

	if ( bResult )
	{
		return AddWeapon( );
	}
	return FALSE;
}

void CSatchel::Spawn( )
{
	Precache( );
	m_iId = WEAPON_SATCHEL;
	SET_MODEL(ENT(pev), "models/w_satchel.mdl");

	m_iDefaultAmmo = SATCHEL_DEFAULT_GIVE;
	m_flNextChatTime2 = gpGlobals->time; //start timer
	m_flNextChatTime4 = 0;
	FallInit();// get ready to fall down.
}


void CSatchel::Precache( void )
{
	PRECACHE_MODEL("models/v_satchel.mdl");
	PRECACHE_MODEL("models/v_satchel_radio.mdl");
	PRECACHE_MODEL("models/w_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel.mdl");
	PRECACHE_MODEL("models/p_satchel_radio.mdl");
	PRECACHE_MODEL("models/w_medkit.mdl");
	PRECACHE_MODEL("models/snapbug.mdl");
	
	PRECACHE_SOUND("weapons/dryfire1.wav");
	PRECACHE_SOUND("weapons/pl_gun1.wav");
	
	
	

	UTIL_PrecacheOther( "monster_satchel" );
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
	//creload = 0;
	return 1;
}

//=========================================================
//=========================================================
BOOL CSatchel::IsUseable( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return TRUE;
	}

	if ( m_chargeReady != 0 )
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}

	return FALSE;
}

BOOL CSatchel::CanDeploy( void )
{
	if ( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 ) 
	{
		// player is carrying some satchels
		return TRUE;
	}

	if ( m_chargeReady != 0 )
	{
		// player isn't carrying any satchels, but has some out
		return TRUE;
	}

	return FALSE;
}

BOOL CSatchel::Deploy( )
{
	g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 285 );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );

	if ( m_chargeReady )
		return DefaultDeploy( "models/v_satchel_radio.mdl", "models/p_satchel_radio.mdl", SATCHEL_RADIO_DRAW, "hive" );
	else
		return DefaultDeploy( "models/v_satchel.mdl", "models/p_satchel.mdl", SATCHEL_DRAW, "trip" );

	
	return TRUE;
}


void CSatchel::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	
	if ( m_chargeReady )
	{
		SendWeaponAnim( SATCHEL_RADIO_HOLSTER );
	}
	else
	{
		SendWeaponAnim( SATCHEL_DROP );
	}
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "common/null.wav", 1.0, ATTN_NORM);

	if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && !m_chargeReady )
	{
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_SATCHEL);
		SetThink( DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}



void CSatchel::PrimaryAttack()
{
	switch (m_chargeReady)
	{
	case 0:
		{
			Throw( );
		}
		break;
	case 1:
		{
			SendWeaponAnim( SATCHEL_RADIO_FIRE );

			edict_t *pPlayer = m_pPlayer->edict( );

			CBaseEntity *pSatchel = NULL;

			while ((pSatchel = UTIL_FindEntityInSphere( pSatchel, m_pPlayer->pev->origin, 8192 )) != NULL)
			{
				if (FClassnameIs( pSatchel->pev, "monster_satchel") || FClassnameIs( pSatchel->pev, "monster_pipebomb"))
				{
					//if (FClassnameIs( pSatchel->pev, "weapon_pipebomb"))
					//	pSatchel->pev->classname = MAKE_STRING( "monster_satchel" );
					if (pSatchel->pev->owner == pPlayer)
					{
						m_pPlayer->m_flNextChatTime4 = 0;
						pSatchel->Use( m_pPlayer, m_pPlayer, USE_ON, 0 );
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


void CSatchel::SecondaryAttack( void )
{
	if (  m_pPlayer->m_flNextChatTime4 < 10 ) //now player can throw only 10 satchel's
	{

		if ( m_chargeReady != 2 )
		{
			Throw( );
		}
		
	}
	
}



void CSatchel::ThirdAttack( void )
{
//Invisible weapon
//new code for 1.26

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 4)
		{
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( m_pPlayer->pev ); //get weapon

		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "debris/beamstart1.wav", 0.9, ATTN_NORM); //play sound
	
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
		m_pPlayer->pev->health = 1; //invisible hurt

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
		//m_iClip -= 6;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 4;
		//invisible effect
		m_pPlayer->pev->rendermode = kRenderTransTexture;
        m_pPlayer->pev->renderamt = 1;
		pl->m_pActiveItem->m_iId = WEAPON_CROWBAR;
		UTIL_ScreenFade( m_pPlayer, Vector(0,0,200), 1.95, 3.5, 70, FFADE_IN );
		return;
		}
}

void CSatchel::FourthAttack()
{
		
	//throw
	if ( (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 2) && m_chargeReady != 2)
	{
		Vector vecSrc = m_pPlayer->pev->origin;
		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

		#ifndef CLIENT_DLL
		CBaseEntity *pSatchel = Create( "monster_pipebomb", vecSrc, Vector( 0, 0, 0), m_pPlayer->edict() );
		pSatchel->pev->velocity = vecThrow;
		#endif

		//SendWeaponAnim( SATCHEL_RADIO_DRAW );
		SendWeaponAnim( SATCHEL_RADIO_FIRE );
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel_radio.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");

		
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_chargeReady = 1;
		
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=2;
		
	}
		


	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}





void CSatchel::Throw( void )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		Vector vecSrc = m_pPlayer->pev->origin;

		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

		m_pPlayer->m_flNextChatTime4 ++;
		
		
#ifndef CLIENT_DLL
		CBaseEntity *pSatchel = Create( "monster_satchel", vecSrc, Vector( 0, 0, 0), m_pPlayer->edict() );
		pSatchel->pev->velocity = vecThrow;
		pSatchel->pev->avelocity.y = 400;

		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel_radio.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel_radio.mdl");
#else
		LoadVModel ( "models/v_satchel_radio.mdl", m_pPlayer );
#endif

		SendWeaponAnim( SATCHEL_RADIO_DRAW );

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_chargeReady = 1;
		
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
	}
}


void CSatchel::WeaponIdle( void )
{

if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 6) // && creload==0
	
	if (  m_pPlayer->m_flNextChatTime2 < gpGlobals->time )
{

	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
		m_pPlayer->m_flNextChatTime2 = gpGlobals->time + 90;
		return;
	}
}

else
{
	if ( m_pPlayer->pev->button & IN_RELOAD && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 6) 
	{
	PlayEmptySound();
	return;
	}
}

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	switch( m_chargeReady )
	{
	case 0:
		SendWeaponAnim( SATCHEL_FIDGET1 );
		// use tripmine animations
		strcpy( m_pPlayer->m_szAnimExtention, "trip" );
		break;
	case 1:
		SendWeaponAnim( SATCHEL_RADIO_FIDGET1 );
		// use hivehand animations
		strcpy( m_pPlayer->m_szAnimExtention, "hive" );
		break;
	case 2:
		if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			m_chargeReady = 0;
			RetireWeapon();
			return;
		}

#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_satchel.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel.mdl");
#else
		LoadVModel ( "models/v_satchel.mdl", m_pPlayer );
#endif

		SendWeaponAnim( SATCHEL_DRAW );

		// use tripmine animations
		strcpy( m_pPlayer->m_szAnimExtention, "trip" );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = 0;
		break;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
}


void CSatchel :: Reload( void )
{


        UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
Vector vecThrow = gpGlobals->v_forward;
        Vector GunPosition = m_pPlayer->GetGunPosition( );
        CBlasterBeam4* Beam = CBlasterBeam4 :: Create( GunPosition,
                                                 vecThrow,
                                                 m_pPlayer );
	m_fInAttack = 0;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-= 6;
	
	//creload = 1;
}













//=========================================================
// DeactivateSatchels - removes all satchels owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateSatchels( CBasePlayer *pOwner ) ///wha
{
	edict_t *pFind; 

	pFind = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_satchel" );

	while ( !FNullEnt( pFind ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind );
		CSatchelCharge *pSatchel = (CSatchelCharge *)pEnt;

		if ( pSatchel )
		{
			if ( pSatchel->pev->owner == pOwner->edict() )
			{
				pSatchel->Deactivate();
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME( pFind, "monster_satchel" );
	}
	
	///////////////
	edict_t *pFind2; 
	pFind2 = FIND_ENTITY_BY_CLASSNAME( NULL, "monster_pipebomb" );
	while ( !FNullEnt( pFind2 ) )
	{
		CBaseEntity *pEnt = CBaseEntity::Instance( pFind2 );
		CSatchelCharge *pSatchel = (CSatchelCharge *)pEnt;

		if ( pSatchel )
		{
			if ( pSatchel->pev->owner == pOwner->edict() )
			{
				pSatchel->Deactivate();
			}
		}

		pFind2 = FIND_ENTITY_BY_CLASSNAME( pFind2, "monster_pipebomb" );
	}
}

#endif

//////New code


#define SQUEEK_DETONATE_DELAY	90.0

void    CBlasterBeam4 :: Spawn( )
{


        Precache( );
		SET_MODEL(ENT(pev), "models/crystal.mdl");
        //SET_MODEL( ENT(pev), "models/crystal.mdl" );
        pev->movetype = MOVETYPE_TOSS; //So gravity affects it a *tad*
        pev->solid = SOLID_BBOX;

        UTIL_SetSize( pev, Vector(-4,-4,-5), Vector(4,4,5) );//Point sized bounding box
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "Crystal" );

        SetThink( MoveThink );
        SetTouch( Hit );

        //pev->angles.x = 0;
        pev->velocity = gpGlobals->v_forward;
        pev->angles.x = -(pev->angles.x);

        //pev->gravity = 0.1;//A SMIDGEN of gravity. Can you HAVE a smidgen of gravity??
        pev->nextthink = gpGlobals->time + 0.1;//10 times a second
        pev->dmg = -70;
		
		pev->effects = EF_MUZZLEFLASH;
		m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;

		pev->gravity = 0;
		pev->friction = 1;

		EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/sg_24.wav", 1.0, ATTN_NORM);
}

void    CBlasterBeam4 :: Precache( )
{
        BeamSprite = PRECACHE_MODEL( BLASTER_BEAM_SPRITE );
        PRECACHE_MODEL( "models/crystal.mdl" );
		PRECACHE_SOUND ("weapons/rocket1.wav");
		m_iSpriteTexture = PRECACHE_MODEL( "sprites/shrinkf.spr" );
		//shockwave.spr
}
//We hit something (yay)
void    CBlasterBeam4 :: Hit( CBaseEntity* Target )
{

}

void    CBlasterBeam4:: Explode(int DamageType)
{
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere( pEntity, pev->origin, 250 )) != NULL)
       	{
		if (pEntity->pev->friction != 1.0) //1.30a antifriction
			pEntity->pev->friction = 1.0;
		
		if ((pEntity->pev->takedamage==DAMAGE_AIM || pEntity->pev->movetype==MOVETYPE_WALK) && (FVisible( pEntity ))) ///check only players
			{
				pEntity->TakeHealth(7, DMG_GENERIC); //give health all around
				pEntity->pev->armorvalue += 1; //give more armor all (v 1.21)
			}
		}
	
	EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/crystal_heal.wav", 0.8, ATTN_NORM); //play sound
	pev->nextthink = gpGlobals->time + 1.0;
	SetThink(MoveThink);
}

CBlasterBeam4* CBlasterBeam4 :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CBlasterBeam4* Beam = GetClassPtr( (CBlasterBeam4*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CBlasterBeam4 :: Hit );
        Beam->pev->owner = Owner->edict( );
        return Beam;

}

void    CBlasterBeam4 :: MoveThink( )
{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 550 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 3 ); // life
			WRITE_BYTE( RANDOM_LONG(30,70) );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 0 );   // r, g, b
			WRITE_BYTE( RANDOM_LONG(3,255) );   // r, g, b
			WRITE_BYTE( RANDOM_LONG(3,255) );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();
		

	Explode(DMG_GENERIC);
		

	if (gpGlobals->time >= m_flDie)
		{
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

		SetThink( SUB_Remove );
		}
}







////////////////////////
////////////////////////
///////////////////////
/////////////////////
void CSatchelCharge2 :: Spawn( void )
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/snapbug.mdl");
	UTIL_SetSize(pev, Vector( -4, -4, -4), Vector(4, 4, 4));	// Uses point-sized, and can be stepped over
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( SatchelSlide );
	SetUse( DetonateUse2 );
	SetThink( SatchelThink );
	pev->nextthink = gpGlobals->time + 0.9;
	//relision
	//rezzorhv
	//sg4
	pev->gravity = 0.5;
	pev->friction = 0.5;

	pev->dmg = 206;
	pev->sequence = 1;
	
	pev->health = 150;
	pev->takedamage = DAMAGE_YES;
	

}


void CSatchelCharge2::SatchelSlide( CBaseEntity *pOther )
{
	entvars_t	*pevOther = pOther->pev;

	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	// pev->avelocity = Vector (300, 300, 300);
	pev->gravity = 1;// normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin - Vector(0,0,10), ignore_monsters, edict(), &tr );

	if ( tr.flFraction < 1.0 )
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
		// play sliding sound, volume based on velocity
	}
	
	StudioFrameAdvance( );
	
}


void CSatchelCharge2 :: SatchelThink( void )
{


	if (!IsInWorld())
	{
		UTIL_Remove( this );
		return;
	}

	
	if (pev->velocity.Length2D() < 3 && (pev->flags & FL_ONGROUND))
	{
	
		pev->velocity.x += RANDOM_FLOAT ( -450, 450 );
		pev->velocity.y += RANDOM_FLOAT ( -450, 450 );
		pev->velocity.z += 160;

	}
	
	//angle
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		{
		//EMIT_SOUND_DYN( ENT(pev), CHAN_BODY, "weapons/dryfire1.wav", 0.35, ATTN_NORM, 1.0, RANDOM_LONG(90,100) );
		pev->avelocity.y += 90;
		pev->angles.y += 90;
		break;
		}
	case 1:	
		{
		EMIT_SOUND_DYN( ENT(pev), CHAN_BODY, "weapons/pl_gun1.wav", 0.35, ATTN_NORM, 1.0, RANDOM_LONG(85,100) );
		pev->avelocity.y -= 90;
		pev->angles.y -= 90;
		break;
		}
	}
	
	//FireBullets( 8, pev->origin, gpGlobals->v_forward , VECTOR_CONE_10DEGREES, 768, 35, 1, 70, VARS( pev->owner ) );

	
	pev->nextthink = gpGlobals->time + 1.5;

}

