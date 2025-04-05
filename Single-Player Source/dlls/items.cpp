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
/*

===== items.cpp ========================================================

  functions governing the selection/use of weapons for players

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "skill.h"
#include "items.h"
#include "gamerules.h"
#include "game.h"

extern int gmsgItemPickup;

class CWorldItem : public CBaseEntity
{
public:
	void	KeyValue(KeyValueData *pkvd ); 
	void	Spawn( void );
	int		m_iType;
};

LINK_ENTITY_TO_CLASS(world_items, CWorldItem);

void CWorldItem::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "type"))
	{
		m_iType = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CWorldItem::Spawn( void )
{
	CBaseEntity *pEntity = NULL;

	switch (m_iType) 
	{
	case 44: // ITEM_BATTERY:
		pEntity = CBaseEntity::Create( "item_battery", pev->origin, pev->angles );
		break;
	case 42: // ITEM_ANTIDOTE:
		pEntity = CBaseEntity::Create( "item_antidote", pev->origin, pev->angles );
		break;
	case 43: // ITEM_SECURITY:
		pEntity = CBaseEntity::Create( "item_security", pev->origin, pev->angles );
		break;
	case 45: // ITEM_SUIT:
		pEntity = CBaseEntity::Create( "item_suit", pev->origin, pev->angles );
		break;
	}

	if (!pEntity)
	{
		// ALERT( at_console, "unable to create world_item %d\n", m_iType );
	}
	else
	{
		pEntity->pev->target = pev->target;
		pEntity->pev->targetname = pev->targetname;
		pEntity->pev->spawnflags = pev->spawnflags;
	}

	REMOVE_ENTITY(edict());
}


void CItem::Spawn( void )
{


	pev->movetype = MOVETYPE_TOSS;
	pev->gravity = 1.0;
	pev->friction = 1.0;
	pev->health = 100;
	pev->takedamage = 1;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 10));
	SetTouch(ItemTouch);
	
	tmp = pev->origin;

	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove( this );
		return;
	}

	// TraceResult tr2;
	// UTIL_TraceHull(pev->origin + Vector( 0, 0, 12 ), pev->origin + Vector(0,0,44), dont_ignore_monsters, human_hull, ENT(pev), &tr2);
	// if ( tr2.flFraction == 1.0)
	{
		//CBaseEntity *pMonstro = CBaseEntity::Create("monster_human_grunt", pev->origin+Vector(0, 0, 0), pev->angles, edict()); // monster_human_grunt monster_alien_grunt
	}
}



void CItem::ItemTouch( CBaseEntity *pOther )
{
		// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
	{
		return;
	}
	

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// ok, a player is touching this item, but can he have it?
	if ( !g_pGameRules->CanHaveItem( pPlayer, this ) )
	{
		// no? Ignore the touch.
		return;
	}

	if (MyTouch( pPlayer ))
	{

		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		SetTouch( NULL );
		
		// if (pev->ltime == 1)
			// UTIL_Remove( this );

		// player grabbed the item. 
		g_pGameRules->PlayerGotItem( pPlayer, this );
		if ( g_pGameRules->ItemShouldRespawn( this ) == GR_ITEM_RESPAWN_YES )
		{
			Respawn(); 
		}
		else
		{
			UTIL_Remove( this );
		}
		Respawn(); 

	}
	// else if (gEvilImpulse101) 
	// {
		// UTIL_Remove( this );
	// }
}

CBaseEntity* CItem::Respawn( void )
{

	SetTouch( NULL );
	pev->effects |= EF_NODRAW;
 
	UTIL_SetOrigin( pev, tmp );// blip to whereever you should respawn.

	SetThink ( Materialize );
	pev->nextthink = g_pGameRules->FlItemRespawnTime( this ); 
	DROP_TO_FLOOR ( ENT(pev) );
	return this;
}


int CItem::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if ( !pev->takedamage )
		return 0;
	if (pevAttacker == NULL)
		return 0;

	pev->health -= flDamage;
	if (pev->velocity.z > 1)
		pev->angles = UTIL_VecToAngles (pev->velocity);
	else
	{
		pev->angles.x = 0;
		pev->angles.z = 0;
	}
	
	if (pev->health <= 0)
	{
		pev->nextthink = g_pGameRules->FlItemRespawnTime( this ); 
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_TELEPORT );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
		MESSAGE_END();
		
		//lights
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_BYTE( 24 );		// radius * 0.1
			WRITE_BYTE( 250 );		// r
			WRITE_BYTE( 250 );		// g
			WRITE_BYTE( 150 );		// b
			WRITE_BYTE( 128 );		// time * 10
			WRITE_BYTE( 16 );		// decay * 0.1
		MESSAGE_END( );
		
		//smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( pev->dmg ); // smoke scale * 10
			WRITE_BYTE( 24  ); // framerate
		MESSAGE_END();
					
		
		pev->movetype = MOVETYPE_NONE;
		pev->takedamage = 0;
		pev->effects |= EF_NODRAW;
		SetThink ( Materialize );
		SetTouch( NULL );
		return 0;
	}


	return 1;
}


void CItem::Materialize( void )
{

		
	if ( pev->effects & EF_NODRAW )
	{
			
		
		
		UTIL_SetOrigin( pev, tmp ); 
		pev->health = 100;
		pev->angles.x = 0;
		pev->angles.z = 0;
		
		pev->takedamage = 1;
		pev->solid = SOLID_TRIGGER;
		pev->movetype = MOVETYPE_TOSS;
		DROP_TO_FLOOR ( ENT(pev) );
		pev->flags &= ~FL_ONGROUND;
		
		
		// changing from invisible state to visible.
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
		pev->nextthink = gpGlobals->time + 0.2;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_TELEPORT );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
		MESSAGE_END();
	}

	SetTouch( ItemTouch );
}

#define SF_SUIT_SHORTLOGON		0x0001

class CItemSuit : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_suit.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_suit.mdl");
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->weapons & (1<<WEAPON_SUIT) )
			return FALSE;

		if ( pev->spawnflags & SF_SUIT_SHORTLOGON )
			EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
		else
			EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon

		pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);


// armor
class CItemBattery : public CItem
{
	void Spawn( void )
	{ 
		switch(RANDOM_LONG(0,3)) // spawn battery separate
		{
			case 0: Create( "shield_battery", pev->origin, pev->angles, edict() ); return; break;
			case 1: Create( "tombstone", pev->origin, pev->angles, edict() ); return; break; // dual elect
			case 2: Create( "tombstone", pev->origin, pev->angles, edict() ); return; break;
		}
		
		Precache( );
		SET_MODEL(ENT(pev), "models/w_battery.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_battery.mdl");
		PRECACHE_SOUND( "items/gunpickup2.wav" );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
		{
			return FALSE;
		}

		if ( (pPlayer->pev->armorvalue < MAX_NORMAL_BATTERY) )
		{

			pPlayer->pev->armorvalue += 15.0;
			pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, MAX_NORMAL_BATTERY);

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();


			return TRUE;		
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);

// shield 
class CItemBattery2 : public CItem
{
	void Spawn( void )
	{ 

		Precache( );
		SET_MODEL(ENT(pev), "models/w_battery3.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_battery3.mdl");
		PRECACHE_MODEL ("models/w_battery3t.mdl");
		PRECACHE_SOUND( "items/gunpickup2.wav" );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
		{
			return FALSE;
		}

		if ( (pPlayer->pev->fuser1 < MAX_NORMAL_BATTERY2))
		{

			pPlayer->pev->fuser1 += 15.0;
			pPlayer->pev->fuser1 = min(pPlayer->pev->fuser1, MAX_NORMAL_BATTERY2);

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();

			return TRUE;		
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(shield_battery, CItemBattery2);


// combine
class CItemBattery3 : public CItem
{
	void Spawn( void )
	{ 

		Precache( );
		SET_MODEL(ENT(pev), "models/w_battery2.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_battery2.mdl");
		PRECACHE_MODEL ("models/w_battery2t.mdl");
		PRECACHE_SOUND( "items/gunpickup2.wav" );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
		{
			return FALSE;
		}

		if ( (pPlayer->pev->armorvalue < MAX_NORMAL_BATTERY) || (pPlayer->pev->fuser1 < MAX_NORMAL_BATTERY2))
		{

			pPlayer->pev->armorvalue += 10.0;
			pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, MAX_NORMAL_BATTERY);
			pPlayer->pev->fuser1 += 10.0; 
			pPlayer->pev->fuser1 = min(pPlayer->pev->fuser1, MAX_NORMAL_BATTERY2);
			

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			
			return TRUE;		
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(tombstone, CItemBattery3);







class CItemAntidote : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_antidote.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_antidote.mdl");
		PRECACHE_SOUND( "zxc/Build1.wav" );
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		pPlayer->TakeHealth( 200.0, DMG_SHOCK );

		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "zxc/Build1.wav", 1.0, ATTN_NORM, 0, 90);
		pPlayer->TripleShot += 3; 
		pPlayer->Shield += 3;
		
		//pPlayer->m_rgItems[ITEM_ANTIDOTE] += 1;
		
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_antidote, CItemAntidote);


class CItemSecurity : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_security.mdl");
		CItem::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_security.mdl");
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		pPlayer->m_rgItems[ITEM_SECURITY] += 1;
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_security, CItemSecurity);

class CItemLongJump : public CItem
{
	void Spawn( void )
	{ 

			
		Precache( );
		SET_MODEL(ENT(pev), "models/w_longjump.mdl");
		CItem::Spawn( );
		
/* 			if ( RANDOM_LONG( 0, 1 ) == 0 )
			{
				EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/rocket1.wav", 1, ATTN_NORM );
			}
			else
			{
				EMIT_SOUND( ENT(pev), CHAN_BODY, "common/bodydrop4.wav", 1, ATTN_NORM );
			} */
			//EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/rocket1.wav", 1, ATTN_NORM );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_longjump.mdl");
	}
	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->m_fLongJump )
		{
			return FALSE;
		}

		if ( ( pPlayer->pev->weapons & (1<<WEAPON_SUIT) ) )
		{
			pPlayer->m_fLongJump = TRUE;// player now has longjump module

			g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "1" );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			//EMIT_SOUND( ENT(pev), CHAN_BODY, "weapons/mine_deploy.wav", 0.5, ATTN_NORM );
			EMIT_SOUND_SUIT( pPlayer->edict(), "!HEV_A1" );	// Play the longjump sound UNDONE: Kelly? correct sound?
			return TRUE;		
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );
