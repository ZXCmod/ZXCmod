/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
/*

===== turret.cpp ========================================================

*/

//
// TODO: 
//		Take advantage of new monster fields like m_hEnemy and get rid of that OFFSET() stuff
//		Revisit enemy validation stuff, maybe it's not necessary with the newest monster code
//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "effects.h"

#include "nodes.h"
#include "player.h"
#include "hornet.h"
#include "gamerules.h"

extern Vector VecBModelOrigin( entvars_t* pevBModel );

#define TURRET_SHOTS	4
#define TURRET_RANGE	1400
#define TURRET_SPREAD	Vector( 0.03268, 0.13716, 0.13134 )
#define TURRET_TURNRATE	30		//angles per 0.1 second
#define TURRET_MAXWAIT	15		// seconds turret will stay active w/o a target
#define TURRET_MAXSPIN	5		// seconds turret barrel will spin w/o a target
#define TURRET_MACHINE_VOLUME	0.4

typedef enum
{
	TURRET_ANIM_NONE = 0,
	TURRET_ANIM_FIRE,
	TURRET_ANIM_SPIN,
	TURRET_ANIM_DEPLOY,
	TURRET_ANIM_RETIRE,
	TURRET_ANIM_DIE,
} TURRET_ANIM;

class CBaseTurret : public CBaseMonster
{
public:
	void Spawn(void);
	virtual void Precache(void);
	void KeyValue( KeyValueData *pkvd );
	//void EXPORT TurretUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int	 TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	virtual int	 Classify( );

	int BloodColor( void ) { return DONT_BLEED; }
	void GibMonster( void ) {}	// UNDONE: Throw turret gibs?

	// Think functions

	void EXPORT ActiveThink(void);
	void EXPORT SearchThink(void);
	void EXPORT AutoSearchThink(void);
	void EXPORT TurretDeath(void);

	virtual void EXPORT SpinDownCall(void) { m_iSpin = 0; }
	virtual void EXPORT SpinUpCall(void) { m_iSpin = 1; }

	// void SpinDown(void);
	// float EXPORT SpinDownCall( void ) { return SpinDown(); }

	// virtual float SpinDown(void) { return 0;}
	// virtual float Retire(void) { return 0;}

	void EXPORT Deploy(void);
	void EXPORT Retire(void);
	
	void EXPORT Initialize(void);

	virtual void Ping(void);
	virtual void EyeOn(void);
	virtual void EyeOff(void);

	

	// other functions
	void SetTurretAnim(TURRET_ANIM anim);
	int MoveTurret(void);
	virtual void Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker) { };

	float m_flMaxSpin;		// Max time to spin the barrel w/o a target
	int m_iSpin;

	//CSprite *m_pEyeGlow;
	//int		m_eyeBrightness;

	int	m_iDeployHeight;
	int	m_iRetractHeight;
	int m_iMinPitch;

	int m_iBaseTurnRate;	// angles per second
	float m_fTurnRate;		// actual turn rate
	int m_iOrientation;		// 0 = floor, 1 = Ceiling
	int	m_iOn;
	int m_fBeserk;			// Sometimes this bitch will just freak out
	int m_iAutoStart;		// true if the turret auto deploys when a target
							// enters its range

	Vector m_vecLastSight;
	float m_flLastSight;	// Last time we saw a target
	float m_flMaxWait;		// Max time to seach w/o a target
	int m_iSearchSpeed;		// Not Used!

	// movement
	float	m_flStartYaw;
	Vector	m_vecCurAngles;
	Vector	m_vecGoalAngles;
	//int m_flNextChatTime7;


	float	m_flPingTime;	// Time until the next ping, used when searching
	float	m_flSpinUpTime;	// Amount of time until the barrel should spin down when searching
	float F;
};



class CTurret : public CBaseTurret
{
public:
	void Spawn(void);
	void Precache(void);
	// Think functions
	void SpinUpCall(void);
	void SpinDownCall(void);

	

	// other functions
	void Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker);

private:
	int m_iStartSpin;

};


class CMiniTurret : public CBaseTurret
{
public:
	void Spawn( );
	void Precache(void);
	// other functions
	void Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker);
};


LINK_ENTITY_TO_CLASS( monster_turret, CTurret );
LINK_ENTITY_TO_CLASS( monster_miniturret, CMiniTurret );

void CBaseTurret::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "maxsleep"))
	{
		m_flMaxWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "orientation"))
	{
		m_iOrientation = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;

	}
	else if (FStrEq(pkvd->szKeyName, "searchspeed"))
	{
		m_iSearchSpeed = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;

	}
	else if (FStrEq(pkvd->szKeyName, "turnrate"))
	{
		m_iBaseTurnRate = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "style") ||
			 FStrEq(pkvd->szKeyName, "height") ||
			 FStrEq(pkvd->szKeyName, "value1") ||
			 FStrEq(pkvd->szKeyName, "value2") ||
			 FStrEq(pkvd->szKeyName, "value3"))
		pkvd->fHandled = TRUE;
	else
		CBaseMonster::KeyValue( pkvd );
}


//
// ID as a machine
//
int	CBaseTurret::Classify ( )
{
	// CBaseEntity *pEntity2 = CBaseEntity::Instance(pev->owner);
	// CBasePlayer *pPlayer = (CBasePlayer *)pEntity2;
	// int index = g_pGameRules->GetTeamIndex( pPlayer->TeamID() );
	
	
	if (m_iOn || m_iAutoStart)
	{
		// if (g_pGameRules->IsTeamplay() && index == 0)  // Blue
			// return	CLASS_MACHINE;
		// if (g_pGameRules->IsTeamplay() && index == 1)  // Red
			// return	CLASS_ALIEN_MONSTER;
		return	CLASS_MACHINE;
	}
		
	
	return 1;
}

void CBaseTurret::Spawn()
{ 
//CBaseEntity *pEntity = NULL;
	
	
	Precache( );
	pev->nextthink		= gpGlobals->time + 1;
	pev->movetype		= MOVETYPE_STEP;
	pev->sequence		= 0;
	pev->frame			= 0;
	pev->solid			= SOLID_SLIDEBOX;
	pev->takedamage		= DAMAGE_AIM;
	pev->ltime  		= 0;
	
	Classify2 = Classify();
	
	pev->max_health = pev->health;
	
	

	SetBits (pev->flags, FL_MONSTER);
	m_iAutoStart = TRUE;
	ResetSequenceInfo( );
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );
	m_flFieldOfView = VIEW_FIELD_FULL;
	

	
	if (!IsInWorld())
		TakeDamage(pev, pev, 10000, 1 );

	
}


void CBaseTurret::Precache( )
{
	PRECACHE_SOUND ("turret/tu_fire1.wav");
	PRECACHE_SOUND ("turret/tu_ping.wav");
	PRECACHE_SOUND ("turret/tu_active2.wav");
	PRECACHE_SOUND ("turret/tu_die.wav");
	PRECACHE_SOUND ("turret/tu_die2.wav");
	PRECACHE_SOUND ("turret/tu_die3.wav");
	PRECACHE_SOUND ("turret/tu_deploy.wav");
	PRECACHE_SOUND ("turret/tu_spinup.wav");
	PRECACHE_SOUND ("turret/tu_spindown.wav");
	PRECACHE_SOUND ("turret/tu_search.wav");
	PRECACHE_SOUND ("turret/tu_alert.wav");
	PRECACHE_SOUND ("ambience/ffdeepamb.wav");
	
}

//#define TURRET_GLOW_SPRITE "sprites/flare3.spr"

void CTurret::Spawn()
{ 

	Precache( );
	SET_MODEL(ENT(pev), "models/turret.mdl");
	pev->health			= 100;
	pev->dmg			= 6;
	//pev->max_health = pev->health;
	m_HackedGunPos		= Vector( 0, 0, 12.75 );
	m_flMaxSpin =		TURRET_MAXSPIN;
	pev->view_ofs.z = 12.75;

	CBaseTurret::Spawn( );

	m_iRetractHeight = 16;
	m_iDeployHeight = 32;
	m_iMinPitch	= -15;
	UTIL_SetSize(pev, Vector(-32, -32, -m_iRetractHeight+16), Vector(32, 32, m_iRetractHeight-16));
	
	SetThink(Initialize);	

}

void CTurret::Precache()
{
	CBaseTurret::Precache( );
	PRECACHE_MODEL ("models/turret.mdl");	
	//PRECACHE_MODEL (TURRET_GLOW_SPRITE);
}

void CMiniTurret::Spawn()
{ 
	Precache( );
	
	UTIL_Remove( this );
	
	SET_MODEL(ENT(pev), "models/miniturret.mdl");
	pev->health			= 100;
	//pev->max_health = pev->health;
	m_HackedGunPos		= Vector( 0, 0, 12.75 );
	m_flMaxSpin = 0;
	pev->view_ofs.z = 12.75;
	pev->dmg = 5;
	

	CBaseTurret::Spawn( );
	m_iRetractHeight = 16;
	m_iDeployHeight = 32;
	m_iMinPitch	= -15;
	UTIL_SetSize(pev, Vector(-16, -16, -m_iRetractHeight), Vector(16, 16, m_iRetractHeight));

	SetThink(Initialize);	
	
	

	pev->nextthink = gpGlobals->time + 0.3; 
	SetThink(Deploy);
}


void CMiniTurret::Precache()
{
	CBaseTurret::Precache( );
	PRECACHE_MODEL ("models/miniturret.mdl");	
	PRECACHE_SOUND("weapons/hks1.wav");
	PRECACHE_SOUND("weapons/hks2.wav");
	PRECACHE_SOUND("weapons/hks3.wav");
}

void CBaseTurret::Initialize(void)
{

	m_iOn = 0;
	m_fBeserk = 0;
	m_iSpin = 0;

	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );

	if (m_iBaseTurnRate == 0) m_iBaseTurnRate = TURRET_TURNRATE;
	if (m_flMaxWait == 0) m_flMaxWait = TURRET_MAXWAIT;
	m_flStartYaw = pev->angles.y;
	if (m_iOrientation == 1)
	{
		pev->idealpitch = 180;
		pev->angles.x = 180;
		pev->view_ofs.z = -pev->view_ofs.z;
		pev->effects |= EF_INVLIGHT;
		pev->angles.y = pev->angles.y + 180;
		if (pev->angles.y > 360)
			pev->angles.y = pev->angles.y - 360;
	}

	m_vecGoalAngles.x = 0;

	if (m_iAutoStart)
	{
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink(Deploy);		
		pev->nextthink = gpGlobals->time + .1;
	}
	else
		SetThink(SUB_DoNothing);
}
/* 
void CBaseTurret::TurretUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{

	if ( !ShouldToggle( useType, m_iOn ) )
		return;

	if (m_iOn)
	{
		m_hEnemy = NULL;
		pev->nextthink = gpGlobals->time + 0.1;
		m_iAutoStart = FALSE;// switching off a turret disables autostart
		//!!!! this should spin down first!!BUGBUG
		SetThink(Retire);
	}
	else 
	{
		pev->nextthink = gpGlobals->time + 0.1; // turn on delay

		// if the turret is flagged as an autoactivate turret, re-enable it's ability open self.
		if ( pev->spawnflags & SF_MONSTER_TURRET_AUTOACTIVATE )
		{
			m_iAutoStart = TRUE;
		}
		
		SetThink(Deploy);
	}
	
}
 */

void CBaseTurret::Ping( void )
{
	//1.30 select weapon
	char *txt; // init string
	
	static s;
	
	Vector direction = Vector(0,0,1); 
	
	// pev->owner = NULL;
	
	if (pev->health < pev->max_health)
		{
		pev->health ++; //UTIL_Sparks( pev->origin); };
		//spark effects
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_STREAK_SPLASH );
			WRITE_COORD( pev->origin.x );		// origin
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( direction.x );	// direction
			WRITE_COORD( direction.y );
			WRITE_COORD( direction.z );
			WRITE_BYTE( 8 );	// Streak color 6
			WRITE_SHORT( 3 );	// count
			WRITE_SHORT( 128 );
			WRITE_SHORT( 128 );	// Random velocity modifier
		MESSAGE_END();
		s=1;
		
		if (s==1)
			EMIT_SOUND(ENT(pev), CHAN_BODY, "ambience/ffdeepamb.wav", 1.0, ATTN_NORM);
		else
			STOP_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav" );
		}
	else
		s=0;
	
	while ((m_hEnemy = UTIL_FindEntityInSphere( m_hEnemy, pev->origin, 50 )) != NULL)
	{
	if ((m_hEnemy->pev->button & IN_USE) && m_hEnemy->edict() == pev->owner && IsFacing2( VARS(pev->owner), pev->origin )) // cone view
			{
			
				pev->ltime ++;
				
				if (pev->ltime >= 9) //loop
					pev->ltime = 0; // zerro is default anti invisers
						
				if (pev->ltime==0)
					txt = "Type 0: Sensor";
				if (pev->ltime==1)
					txt = "Type 1: Linear Rocket";
				if (pev->ltime==2)
					txt = "Type 2: Chaos Rocket";
				if (pev->ltime==3)
					txt = "Type 3: BFBomb";
				if (pev->ltime==4)
					txt = "Type 4: Arrow";
				if (pev->ltime==5)
					txt = "Type 5: MP5 Plasma";
				if (pev->ltime==6)
					txt = "Type 6: Gauss Cannon";
				if (pev->ltime==7)
					txt = "Type 7: Shotgun Cannon";
				if (pev->ltime==8)
					txt = "Type 8: HECapsule";
				
					
				//print texts
				char  szText[64];
				hudtextparms_t hText;
				sprintf(szText, "%s .\n", txt ); //game text
				memset(&hText, 0, sizeof(hText));
				hText.channel = 29;
				hText.x = 0.90; //range by 0.0 to 1.0
				hText.y = 0.70;
				hText.effect = 1; // Fade in/out
				hText.r1 = hText.g1 = hText.b1 = 255;
				hText.a1 = 255;
				hText.r2 = hText.g2 = hText.b2 = 255;
				hText.a2 = 255;
				hText.fadeinTime = 0.5;
				hText.fadeoutTime = 0.1;
				hText.holdTime = 5.0;
				hText.fxTime = 0.5;
				//if (  m_hEnemy->edict() == pev->owner )
					UTIL_HudMessage(m_hEnemy, hText, szText);
					
				EMIT_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav", 1.0, ATTN_NORM);
				//visual set
				pev->renderfx = kRenderFxGlowShell;
				pev->rendercolor.x = 16*pev->ltime;  // red
				pev->rendercolor.y = 16*pev->ltime;  // green
				pev->rendercolor.z = 128; // blue
				pev->renderamt = RANDOM_LONG(64,255);
				pev->nextthink = gpGlobals->time + 0.5;
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(SearchThink);
				return;
			}
	}

	// make the pinging noise every second while searching
	if (m_flPingTime == 0)
	{
		m_flPingTime = gpGlobals->time + 2;
	}
	else if (m_flPingTime <= gpGlobals->time)
	{
		m_flPingTime = gpGlobals->time + 2;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "turret/tu_ping.wav", 0.25, ATTN_NORM);
		EyeOn( );
		//visual unset
		pev->renderfx = kRenderFxNone;
		pev->renderamt = 0;
	}

}


void CBaseTurret::EyeOn( )
{

	
}


void CBaseTurret::EyeOff( )
{

}


void CBaseTurret::ActiveThink(void)
{
/* 	if ( m_flNextChatTime7 < gpGlobals->time )
	{
		pev->health -= 1000;
		SetThink( SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1;
	} */

	
	

	int fAttack = 0;
	Vector vecDirToEnemy;

	pev->nextthink = gpGlobals->time + 0.2;
	StudioFrameAdvance( );

	if ((!m_iOn) || (m_hEnemy == NULL))
	{
		m_hEnemy = NULL;
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink(SearchThink);
		return;
	}
	
	// if it's dead, look for something new
	if ( !m_hEnemy->IsAlive() )
	{
		if (!m_flLastSight)
		{
			m_flLastSight = gpGlobals->time + 0.5; // continue-shooting timeout
		}
		else
		{
			if (gpGlobals->time > m_flLastSight)
			{	
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(SearchThink);
				return;
			}
		}
	}

	Vector vecMid = pev->origin + pev->view_ofs;
	Vector vecMidEnemy = m_hEnemy->BodyTarget( vecMid );

	// Look for our current enemy
	int fEnemyVisible = FBoxVisible(pev, m_hEnemy->pev, vecMidEnemy );



	vecDirToEnemy = vecMidEnemy - vecMid;	// calculate dir and dist to enemy
	float flDistToEnemy = vecDirToEnemy.Length();

	Vector vec = UTIL_VecToAngles(vecMidEnemy - vecMid);	

	// Current enmey is not visible.
	if (!fEnemyVisible || (flDistToEnemy > TURRET_RANGE))
	{
		if (!m_flLastSight)
			m_flLastSight = gpGlobals->time + 0.5;
		else
		{
			// Should we look for a new target?
			if (gpGlobals->time > m_flLastSight)
			{
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink(SearchThink);
				return;
			}
		}
		fEnemyVisible = 0;
	}
	else
	{
		m_vecLastSight = vecMidEnemy;
	}

	UTIL_MakeAimVectors(m_vecCurAngles);	


	
	Vector vecLOS = vecDirToEnemy; //vecMid - m_vecLastSight;
	vecLOS = vecLOS.Normalize();


	
	if (DotProduct(vecLOS, gpGlobals->v_forward) <= 0.866) // 30 degree slop
		fAttack = FALSE;
	else
		fAttack = TRUE;

		
		

		
		
		


		
	// fire the gun
if (m_iSpin && ((fAttack) || (m_fBeserk)))
	{
	
	if (F > 0.0)
		F -= 0.1; //float timer
		
	if (TripleShot == 1 && F <= 0.1)
		TripleShot = 0;

	
	if (pev->ltime == 0) // sensor
		{
			Vector vecSrc, vecAng;
			GetAttachment( 0, vecSrc, vecAng );

			// SetTurretAnim(TURRET_ANIM_FIRE);
			// Shoot(vecSrc, gpGlobals->v_forward, NULL, VARS( pev->owner ) ); 
			
			// self effect
			// MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				// WRITE_BYTE( TE_TELEPORT );
				// WRITE_COORD( pev->origin.x );
				// WRITE_COORD( pev->origin.y );
				// WRITE_COORD( pev->origin.z );
			// MESSAGE_END();
			// target
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_TELEPORT );
				WRITE_COORD( m_hEnemy->pev->origin.x );
				WRITE_COORD( m_hEnemy->pev->origin.y );
				WRITE_COORD( m_hEnemy->pev->origin.z );
			MESSAGE_END();
			
			// follow line to targets >1.36
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD(vecSrc.x);
				WRITE_COORD(vecSrc.y);
				WRITE_COORD(vecSrc.z);
				WRITE_COORD( m_hEnemy->pev->origin.x ); //tr.vecEndPos.
				WRITE_COORD( m_hEnemy->pev->origin.y );
				WRITE_COORD( m_hEnemy->pev->origin.z );
				WRITE_SHORT( g_sModelIndexLaser ); //sprite
				WRITE_BYTE( 1 ); // Starting frame
				WRITE_BYTE( 0  ); // framerate * 0.1
				WRITE_BYTE( 1 ); // life * 0.1
				WRITE_BYTE( 10 ); // width
				WRITE_BYTE( 0 ); // noise
				WRITE_BYTE( 100 ); // color r,g,b
				WRITE_BYTE( 100 ); // color r,g,b
				WRITE_BYTE( 150 ); // color r,g,b
				WRITE_BYTE( 128 ); // brightness
				WRITE_BYTE( 16 ); // scroll speed
			MESSAGE_END();
			
			if (m_hEnemy->pev->effects == EF_NODRAW)
			{
				m_hEnemy->pev->velocity.x = m_hEnemy->pev->velocity.x * 0.34; 
				m_hEnemy->pev->velocity.y = m_hEnemy->pev->velocity.y * 0.34; 
			}
			else
				m_hEnemy->TakeDamage(pev, VARS( pev->owner ), 1, DMG_SHOCK);
				
			F = 0.1;
		}
	if (pev->ltime == 1 && F <= 0) // linear rocket
		{
					SetTurretAnim(TURRET_ANIM_FIRE);
					CBaseEntity *pRocket1 = CBaseEntity::Create( "weapon_stoner", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket1->pev->dmg = 90;
					pRocket1->pev->ltime = 80;
					pRocket1->pev->angles.z += 90; 
					pRocket1->pev->velocity = gpGlobals->v_forward * 700;
					F = 0.8;
		}
	if (pev->ltime == 2 && F <= 0) // Random Rocket
		{
					SetTurretAnim(TURRET_ANIM_FIRE);
					CBaseEntity *pRocket2 = CBaseEntity::Create( "weapon_rocketlauncher", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket2->pev->dmg = 48;
					pRocket2->pev->ltime = 80;
					pRocket2->pev->velocity = gpGlobals->v_forward * 700;
					F = 1.1;
		}
	if (pev->ltime == 3 && F <= 0) // heavy BfBmb
		{
					SetTurretAnim(TURRET_ANIM_FIRE);
					CBaseEntity *pRocket3 = CBaseEntity::Create( "weapon_frag", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket3->pev->dmg = 48;
					pRocket3->pev->gravity = 0.1;
					pRocket3->pev->ltime = 80;
					pRocket3->pev->velocity = gpGlobals->v_forward * 700;
					F = 1.0;
					
		}
	if (pev->ltime == 4 && F <= 0) // arrow
		{
					SetTurretAnim(TURRET_ANIM_FIRE);
					CBaseEntity *pRocket4 = CBaseEntity::Create( "bone_follow", pev->origin + gpGlobals->v_forward * 50 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket4->pev->dmg = 50;
					pRocket4->pev->velocity = gpGlobals->v_forward * 2000;
					F = 1.0;
					
		}
	if (pev->ltime == 5 && F <= 0) // mp5 plasma
		{
					SetTurretAnim(TURRET_ANIM_FIRE);
					CBaseEntity *pRocket5 = CBaseEntity::Create( "weapon_laser_rifle", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket5->pev->velocity = gpGlobals->v_forward * 2048;
					
					if (pev->health > (pev->max_health*0.5))
						F = 0.05;
					else
						F = 0.1;
					
					//play sounds
					switch(RANDOM_LONG(0,2))
						{
						case 0: 
							EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/2plasma_fire1.wav", 1.0, ATTN_NORM); //play sound
						break;
						case 1: 
							EMIT_SOUND(ENT(pev), CHAN_BODY, "zxc/2plasma_fire3.wav", 1.0, ATTN_NORM); //play sound
						break;
						case 2: 
							EMIT_SOUND(ENT(pev), CHAN_STATIC, "zxc/2plasma_fire6.wav", 1.0, ATTN_NORM); //play sound
						break;
						}
					
		}
	if (pev->ltime == 6 && F <= 0) // gauss cannon
		{
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
					SetTurretAnim(TURRET_ANIM_FIRE);
					EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/electro4.wav", 1.0, ATTN_NORM);
					CBaseEntity *pRocket6 = CBaseEntity::Create( "virtual_hull", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket6->pev->velocity = gpGlobals->v_forward * 1200;
					pRocket6->pev->dmg = 90;
					F = 1.5;
		}
	if (pev->ltime == 7 && F <= 0) // shotgun cannon
		{
					SetTurretAnim(TURRET_ANIM_FIRE);
					EMIT_SOUND(ENT(pev), CHAN_VOICE, "ambience/biggun2.wav", 1.0, ATTN_NORM);
					CBaseEntity *pRocket7 = CBaseEntity::Create( "weapon_tacgun", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket7->pev->velocity = gpGlobals->v_forward * 1200;
					pRocket7->pev->gravity = 0.1;
					F = 0.8;
		}
	if (pev->ltime == 8 && F <= 0) // magnum think
		{
		
					SetTurretAnim(TURRET_ANIM_FIRE);
					EMIT_SOUND(ENT(pev), CHAN_VOICE, "zxc/bemlsr2.wav", 1.0, ATTN_NORM);
					CBaseEntity *pRocket8 = CBaseEntity::Create( "halo_base", pev->origin + gpGlobals->v_forward * 40 + gpGlobals->v_up * 40, pev->angles, pev->owner );
					pRocket8->pev->velocity = gpGlobals->v_forward * 700;
					pRocket8->pev->gravity = 0.1;
					pRocket8->pev->dmg = 69;
					F = 1.7;
		}

	}


	 
	
	
	else
	{
		SetTurretAnim(TURRET_ANIM_SPIN);
	}
			SetTurretAnim(TURRET_ANIM_SPIN);

	if (fEnemyVisible)
	{
	

			
		if (vec.y > 360)
			vec.y -= 360;

		if (vec.y < 0)
			vec.y += 360;

		//ALERT(at_console, "[%.2f]", vec.x);
		
		if (vec.x < -180)
			vec.x += 360;

		if (vec.x > 180)
			vec.x -= 360;

		// now all numbers should be in [1...360]
		// pin to turret limitations to [-90...15]

		if (m_iOrientation == 0)
		{
			if (vec.x > 90)
				vec.x = 90;
			else if (vec.x < m_iMinPitch)
				vec.x = m_iMinPitch;
		}
		else
		{
			if (vec.x < -90)
				vec.x = -90;
			else if (vec.x > -m_iMinPitch)
				vec.x = -m_iMinPitch;
		}

		// ALERT(at_console, "->[%.2f]\n", vec.x);

		m_vecGoalAngles.y = vec.y;
		m_vecGoalAngles.x = vec.x;

	}
//}
	
	SpinUpCall();
	MoveTurret();

}

//turret fired VARS( pev->owner )
void CTurret::Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker)
{
	FireBullets( 1, vecSrc, vecDirToEnemy, TURRET_SPREAD, TURRET_RANGE, 5, 1, 1, VARS( pev->owner ) );
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "turret/tu_fire1.wav", 1, 0.6);
	pev->effects = pev->effects | EF_MUZZLEFLASH;
}


void CMiniTurret::Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker)
{




	FireBullets( 1, vecSrc, vecDirToEnemy, TURRET_SPREAD, TURRET_RANGE, BULLET_MONSTER_9MM, 1 );

	switch(RANDOM_LONG(0,2))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/hks1.wav", 1, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/hks2.wav", 1, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/hks3.wav", 1, ATTN_NORM); break;
	}
	pev->effects = pev->effects | EF_MUZZLEFLASH;
}


void CBaseTurret::Deploy(void)
{
	pev->nextthink = gpGlobals->time + 0.1;
	StudioFrameAdvance( );

	if (pev->sequence != TURRET_ANIM_DEPLOY)
	{
		m_iOn = 1;
		SetTurretAnim(TURRET_ANIM_DEPLOY);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_deploy.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
		SUB_UseTargets( this, USE_ON, 0 );
	}

	if (m_fSequenceFinished)
	{
		pev->maxs.z = m_iDeployHeight;
		pev->mins.z = -m_iDeployHeight;
		UTIL_SetSize(pev, pev->mins, pev->maxs);

		m_vecCurAngles.x = 0;

		if (m_iOrientation == 1)
		{
			m_vecCurAngles.y = UTIL_AngleMod( pev->angles.y + 180 );
		}
		else
		{
			m_vecCurAngles.y = UTIL_AngleMod( pev->angles.y );
		}

		SetTurretAnim(TURRET_ANIM_SPIN);
		pev->framerate = 0;
		SetThink(SearchThink);
	}

	m_flLastSight = gpGlobals->time + m_flMaxWait;
}

void CBaseTurret::Retire(void)
{
	// make the turret level
	m_vecGoalAngles.x = 0;
	m_vecGoalAngles.y = m_flStartYaw;

	pev->nextthink = gpGlobals->time + 0.1;

	StudioFrameAdvance( );

	EyeOff( );

	
	

	
	if (!MoveTurret())
	{
		if (m_iSpin)
		{
			SpinDownCall();
		}
		else if (pev->sequence != TURRET_ANIM_RETIRE)
		{
			SetTurretAnim(TURRET_ANIM_RETIRE);
			EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "turret/tu_deploy.wav", TURRET_MACHINE_VOLUME, ATTN_NORM, 0, 120);
			SUB_UseTargets( this, USE_OFF, 0 );
		}
		else if (m_fSequenceFinished) 
		{	
			m_iOn = 0;
			m_flLastSight = 0;
			SetTurretAnim(TURRET_ANIM_NONE);
			pev->maxs.z = m_iRetractHeight;
			pev->mins.z = -m_iRetractHeight;
			UTIL_SetSize(pev, pev->mins, pev->maxs);
			
			

			if (m_iAutoStart)
			{
				SetThink(AutoSearchThink);		
				pev->nextthink = gpGlobals->time + .1;
			}
			else
				SetThink(SUB_DoNothing);
		}
	}
	else
	{
		SetTurretAnim(TURRET_ANIM_SPIN);
	}
	

	
	
	
}


void CTurret::SpinUpCall(void)
{


	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	// Are we already spun up? If not start the two stage process.
	if (!m_iSpin)
	{
		SetTurretAnim( TURRET_ANIM_SPIN );
		// for the first pass, spin up the the barrel
		if (!m_iStartSpin)
		{
			pev->nextthink = gpGlobals->time + 1.0; // spinup delay
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_spinup.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
			m_iStartSpin = 1;
			pev->framerate = 0.1;
		}
		// after the barrel is spun up, turn on the hum
		else if (pev->framerate >= 1.0)
		{
			pev->nextthink = gpGlobals->time + 0.1; // retarget delay
			EMIT_SOUND(ENT(pev), CHAN_STATIC, "turret/tu_active2.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
			SetThink(ActiveThink);
			m_iStartSpin = 0;
			m_iSpin = 1;
		} 
		else
		{
			pev->framerate += 0.175;
		}
	}

	if (m_iSpin)
	{
		SetThink(ActiveThink);
	}
}


void CTurret::SpinDownCall(void)
{
	if (m_iSpin)
	{
		SetTurretAnim( TURRET_ANIM_SPIN );
		if (pev->framerate == 1.0)
		{
			EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "turret/tu_active2.wav", 0, 0, SND_STOP, 100);
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "turret/tu_spindown.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
		}
		pev->framerate -= 0.02;
		if (pev->framerate <= 0)
		{
			pev->framerate = 0;
			m_iSpin = 0;
		}
	}
}


void CBaseTurret::SetTurretAnim(TURRET_ANIM anim)
{
	if (pev->sequence != anim)
	{
		switch(anim)
		{
		case TURRET_ANIM_FIRE:
		case TURRET_ANIM_SPIN:
			if (pev->sequence != TURRET_ANIM_FIRE && pev->sequence != TURRET_ANIM_SPIN)
			{
				pev->frame = 0;
			}
			break;
		default:
			pev->frame = 0;
			break;
		}

		pev->sequence = anim;
		ResetSequenceInfo( );

		switch(anim)
		{
		case TURRET_ANIM_RETIRE:
			pev->frame			= 255;
			pev->framerate		= -1.0;
			break;
		case TURRET_ANIM_DIE:
			pev->framerate		= 1.0;
			break;
		}
		//ALERT(at_console, "Turret anim #%d\n", anim);
	}
}


//
// This search function will sit with the turret deployed and look for a new target. 
// After a set amount of time, the barrel will spin down. After m_flMaxWait, the turret will
// retact.
//
void CBaseTurret::SearchThink(void)
{

	// ensure rethink
	SetTurretAnim(TURRET_ANIM_SPIN);
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;


	
	if (m_flSpinUpTime == 0 && m_flMaxSpin)
		m_flSpinUpTime = gpGlobals->time + m_flMaxSpin;

	Ping( );

	// If we have a target and we're still healthy
	if (m_hEnemy != NULL)
	{
		if (!m_hEnemy->IsAlive() )
		
			m_hEnemy = NULL;// Dead enemy forces a search for new one
	}


	// Acquire Target
	if (m_hEnemy == NULL)
	{
		Look(TURRET_RANGE);
		m_hEnemy = BestVisibleEnemy();
	}

	// If we've found a target, spin up the barrel and start to attack
	if (m_hEnemy != NULL)
	{
		m_flLastSight = 0;
		m_flSpinUpTime = 0;
		SetThink(ActiveThink);
	}
	else
	{
		// Are we out of time, do we need to retract?
 		if (gpGlobals->time > m_flLastSight)
		{
			//Before we retrace, make sure that we are spun down.
			m_flLastSight = 0;
			m_flSpinUpTime = 0;
			SetThink(Retire);
		}
		// should we stop the spin?
		else if ((m_flSpinUpTime) && (gpGlobals->time > m_flSpinUpTime))
		{
			SpinDownCall();
		}
		
		// generic hunt for new victims
		m_vecGoalAngles.y = (m_vecGoalAngles.y + 0.1 * m_fTurnRate);
		if (m_vecGoalAngles.y >= 360)
			m_vecGoalAngles.y -= 360;
		MoveTurret();
	}
	


}


// 
// This think function will deploy the turret when something comes into range. This is for
// automatically activated turrets.
//
void CBaseTurret::AutoSearchThink(void)
{



	// ensure rethink
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.3;

	// If we have a target and we're still healthy

	if (m_hEnemy != NULL)
	{
		if (!m_hEnemy->IsAlive() )
			m_hEnemy = NULL;// Dead enemy forces a search for new one
	}

	// Acquire Target

	if (m_hEnemy == NULL)
	{
		Look( TURRET_RANGE );
		m_hEnemy = BestVisibleEnemy();
	}

	if (m_hEnemy != NULL)
	{
		SetThink(Deploy);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_alert.wav", TURRET_MACHINE_VOLUME, ATTN_NORM);
	}
}

void CBaseTurret ::	TurretDeath( void )
{
	CBaseEntity *pEntity = NULL;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );

	STOP_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav" );


	BOOL iActive = FALSE;
	
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.25;

	if (pev->deadflag != DEAD_DEAD)
	{
		pev->deadflag = DEAD_DEAD;

		float flRndSound = RANDOM_FLOAT ( 0 , 1 );

		if ( flRndSound <= 0.33 )
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_die.wav", 1.0, ATTN_NORM);
		else if ( flRndSound <= 0.66 )
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_die2.wav", 1.0, ATTN_NORM);
		else 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_die3.wav", 1.0, ATTN_NORM);

		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "turret/tu_active2.wav", 0, 0, SND_STOP, 100);

		if (m_iOrientation == 0)
			m_vecGoalAngles.x = -15;
		else
			m_vecGoalAngles.x = -90;

		SetTurretAnim(TURRET_ANIM_DIE); 

		EyeOn( );	
	}

	EyeOff( );

	if (pev->dmgtime + RANDOM_FLOAT( 0, 2 ) > gpGlobals->time)
	{
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ) );
			WRITE_COORD( RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ) );
			WRITE_COORD( pev->origin.z - m_iOrientation * 64 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 25 ); // scale * 10
			WRITE_BYTE( 10 - m_iOrientation * 5); // framerate
		MESSAGE_END();
	}
	
	if (pev->dmgtime + RANDOM_FLOAT( 0, 5 ) > gpGlobals->time)
	{
		Vector vecSrc = Vector( RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ), RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ), 0 );
		if (m_iOrientation == 0)
			vecSrc = vecSrc + Vector( 0, 0, RANDOM_FLOAT( pev->origin.z, pev->absmax.z ) );
		else
			vecSrc = vecSrc + Vector( 0, 0, RANDOM_FLOAT( pev->absmin.z, pev->origin.z ) );

		UTIL_Sparks( vecSrc );
	}

	if (m_fSequenceFinished && !MoveTurret( ) && pev->dmgtime + 5 < gpGlobals->time)
	{
		pev->framerate = 0;
		SetThink( NULL );
	}
		//1.28 limit reset for big guns
		//bug - crash without initaly player


	if (pl != NULL && (pl->edict() == pev->owner))
		pl->m_flNextChatTime13 -= 1;
		
	UTIL_Remove( this );
	//SetThink( SUB_Remove );
}



void CBaseTurret :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( ptr->iHitgroup == 10 )
	{
		// hit armor
		if ( pev->dmgtime != gpGlobals->time || (RANDOM_LONG(0,10) < 1) )
		{
			UTIL_Ricochet( ptr->vecEndPos, RANDOM_FLOAT( 1, 9) );
			pev->dmgtime = gpGlobals->time;
		}

		flDamage = 0.1;// don't hurt the monster much, but allow bits_COND_LIGHT_DAMAGE to be generated
	}

	if ( !pev->takedamage )
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

// take damage. bitsDamageType indicates type of damage sustained, ie: DMG_BULLET

int CBaseTurret::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if ( !pev->takedamage )
		return 0;

	//if (!m_iOn)
	//	flDamage /= 10.0;
		Vector vecSrc, vecAng;
		GetAttachment( 1, vecSrc, vecAng );
		UTIL_Sparks( vecSrc );
		UTIL_BloodStream( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_YELLOW, 16 );
		pev->health -= flDamage;
		
	if (pev->health <= 0)
	{
		Vector vecSrc, vecAng;
		GetAttachment( 1, vecSrc, vecAng );
		pev->health = -100;
		UTIL_Sparks( vecSrc );
		pev->takedamage = DAMAGE_NO;
		pev->dmgtime = gpGlobals->time;

		ClearBits (pev->flags, FL_MONSTER); // why are they set in the first place???

		//SetUse(NULL);
		SetThink(TurretDeath);
		SUB_UseTargets( this, USE_ON, 0 ); // wake up others
		pev->nextthink = gpGlobals->time + 0.1;
		//SUB_Remove( );
		return 0;
	}

	if (pev->health <= 10)
	{
		if (m_iOn && (1 || RANDOM_LONG(0, 0x7FFF) > 800))
		{
			m_fBeserk = 1;
			SetThink(SearchThink);
		}
	}

	return 1;
}

int CBaseTurret::MoveTurret(void)
{
	int state = 0;
	// any x movement?
	
	if (m_vecCurAngles.x != m_vecGoalAngles.x)
	{
		float flDir = m_vecGoalAngles.x > m_vecCurAngles.x ? 1 : -1 ;

		m_vecCurAngles.x += 0.1 * m_fTurnRate * flDir;

		// if we started below the goal, and now we're past, peg to goal
		if (flDir == 1)
		{
			if (m_vecCurAngles.x > m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		} 
		else
		{
			if (m_vecCurAngles.x < m_vecGoalAngles.x)
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}

		if (m_iOrientation == 0)
			SetBoneController(1, -m_vecCurAngles.x);
		else
			SetBoneController(1, m_vecCurAngles.x);
		state = 1;
	}

	if (m_vecCurAngles.y != m_vecGoalAngles.y)
	{
		float flDir = m_vecGoalAngles.y > m_vecCurAngles.y ? 1 : -1 ;
		float flDist = fabs(m_vecGoalAngles.y - m_vecCurAngles.y);
		
		if (flDist > 180)
		{
			flDist = 360 - flDist;
			flDir = -flDir;
		}
		if (flDist > 30)
		{
			if (m_fTurnRate < m_iBaseTurnRate * 10)
			{
				m_fTurnRate += m_iBaseTurnRate;
			}
		}
		else if (m_fTurnRate > 45)
		{
			m_fTurnRate -= m_iBaseTurnRate;
		}
		else
		{
			m_fTurnRate += m_iBaseTurnRate;
		}

		m_vecCurAngles.y += 0.1 * m_fTurnRate * flDir;

		if (m_vecCurAngles.y < 0)
			m_vecCurAngles.y += 360;
		else if (m_vecCurAngles.y >= 360)
			m_vecCurAngles.y -= 360;

		if (flDist < (0.05 * m_iBaseTurnRate))
			m_vecCurAngles.y = m_vecGoalAngles.y;

		//ALERT(at_console, "%.2f -> %.2f\n", m_vecCurAngles.y, y);
		if (m_iOrientation == 0)
			SetBoneController(0, m_vecCurAngles.y - pev->angles.y );
		else 
			SetBoneController(0, pev->angles.y - 180 - m_vecCurAngles.y );
		state = 1;
	}

	if (!state)
		m_fTurnRate = m_iBaseTurnRate;

	//ALERT(at_console, "(%.2f, %.2f)->(%.2f, %.2f)\n", m_vecCurAngles.x, 
	//	m_vecCurAngles.y, m_vecGoalAngles.x, m_vecGoalAngles.y);
	return state;
}





//=========================================================
// Sentry gun - smallest turret, placed near grunt entrenchments
//=========================================================
class CSentry : public CBaseTurret
{
public:
	void Spawn( );
	void Precache(void);
	// other functions
	void Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker);
	int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	void EXPORT SentryTouch( CBaseEntity *pOther );
	void EXPORT SentryDeath( void );
	float m_flSize;

EHANDLE m_hOwner;
};

LINK_ENTITY_TO_CLASS( monster_sentry, CSentry );

void CSentry::Precache()
{
	CBaseTurret::Precache( );
	PRECACHE_MODEL ("models/sentry.mdl");	
}

void CSentry::Spawn()
{ 

	Precache( );

	SET_MODEL(ENT(pev), "models/sentry.mdl");
	pev->health			= 75;

	//pev->max_health = pev->health;

	UTIL_SetSize(pev, Vector( -5, -5, -1), Vector(5, 5, 3));
	//UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	UTIL_SetOrigin( pev, pev->origin );
	pev->view_ofs.z		= 48;
	m_flMaxWait = 1E6;
	m_flMaxSpin	= 1E6;

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	
	// Classify2 == CLASS_MACHINE;
	
	CBaseTurret::Spawn();
	m_iRetractHeight = 64;
	m_iDeployHeight = 64;
	m_iMinPitch	= -60;
	//UTIL_SetSize(pev, Vector(-16, -16, -m_iRetractHeight), Vector(16, 16, m_iRetractHeight));

	SetTouch(SentryTouch);
	SetThink(Initialize);	
	pev->nextthink = gpGlobals->time + 0.3; 
	
	//if ( pev->owner )
	m_hOwner = Instance( pev->owner );
	pev->gravity		= 1;
	pev->friction		= 1;
	pev->dmg			= 5;

	pev->flags		|= FL_MONSTER;
	m_flSize = gpGlobals->time + 1; //set normal bounding size while time out

}

void CSentry::Shoot(Vector &vecSrc, Vector &vecDirToEnemy, CBaseEntity *pOther, entvars_t *pevAttacker)
{

	FireBullets( 1, vecSrc, vecDirToEnemy, Vector( 0.01716, 0.03716, 0.06864 ), TURRET_RANGE, BULLET_MONSTER_MP5, 1, 5, pevAttacker );

	//////////////fix fire rate for internet///////////////
	//switch(RANDOM_LONG(0,2))
	//{
	//case 0: 
	//CBaseEntity *pHornet = CBaseEntity::Create( "hornet", vecSrc, vecDirToEnemy, pevAttacker->edict() );
	//pHornet->pev->velocity = gpGlobals->v_forward * 300;
	//break;
	//}
	switch(RANDOM_LONG(0,2))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/hks1.wav", 0.3, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/hks2.wav", 0.3, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/hks3.wav", 0.3, ATTN_NORM); break;
	}
	pev->effects = pev->effects | EF_MUZZLEFLASH;
}

int CSentry::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
//sparks
	UTIL_BloodStream( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_YELLOW, 16 );
	//UTIL_BloodDrips( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, RANDOM_LONG(100, 200) );

		Vector vecSrc, vecAng;
		GetAttachment( 1, vecSrc, vecAng );
		UTIL_Sparks( vecSrc );
//



	if ( !pev->takedamage )
		return 0;

	if (!m_iOn)
	{
		SetThink( Deploy );
		//SetUse( NULL );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	pev->health -= flDamage;
	if (pev->health <= 0)
	{
		pev->health = 0;
		pev->takedamage = DAMAGE_NO;
		pev->dmgtime = gpGlobals->time;

		ClearBits (pev->flags, FL_MONSTER); // why are they set in the first place???

		//SetUse(NULL);
		SetThink(SentryDeath);
		SUB_UseTargets( this, USE_ON, 0 ); // wake up others
		pev->nextthink = gpGlobals->time + 0.1;
//VARS( pev->owner )
		return 0;
	}

	return 1;
}


void CSentry::SentryTouch( CBaseEntity *pOther )
{
	//if spawn outside
	if (!IsInWorld()) //test
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
		return;
	}
	
	//if on ground
	if (pev->flags & FL_ONGROUND || (gpGlobals->time >= m_flSize))
		UTIL_SetSize(pev, Vector(-16, -16, -m_iRetractHeight+10), Vector(16, 16, m_iRetractHeight-10));
	else
	{
		UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
		m_flSize = 0;
	}
		
	//take dmg
	if ( pOther && (pOther->IsPlayer() || (pOther->pev->flags & FL_MONSTER)) )
	{
		TakeDamage(pOther->pev, pOther->pev, 1, 1 );
	}
}


void CSentry ::	SentryDeath( void )
{
	//1.28 limit reset 
	CBaseEntity *pEntity = NULL;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev->owner );
	
	STOP_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav" );
	
	BOOL iActive = FALSE;

	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->deadflag != DEAD_DEAD)
	{
		pev->deadflag = DEAD_DEAD;

		float flRndSound = RANDOM_FLOAT ( 0 , 1 );

		if ( flRndSound <= 0.33 )
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_die.wav", 1.0, ATTN_NORM);
		else if ( flRndSound <= 0.66 )
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_die2.wav", 1.0, ATTN_NORM);
		else 
			EMIT_SOUND(ENT(pev), CHAN_BODY, "turret/tu_die3.wav", 1.0, ATTN_NORM);

		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, "turret/tu_active2.wav", 0, 0, SND_STOP, 100);

		SetBoneController( 0, 0 );
		SetBoneController( 1, 0 );

		SetTurretAnim(TURRET_ANIM_DIE); 

		pev->solid = SOLID_NOT;
		pev->angles.y = UTIL_AngleMod( pev->angles.y + RANDOM_LONG( 0, 2 ) * 120 );
		EyeOn( );
		UTIL_Remove( this );
		//SetThink( SUB_Remove );
	}

	EyeOff( );

	Vector vecSrc, vecAng;
	GetAttachment( 1, vecSrc, vecAng );

	if (pev->dmgtime + RANDOM_FLOAT( 0, 2 ) > gpGlobals->time)
	{
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSrc.x);
			WRITE_COORD( vecSrc.y);
			WRITE_COORD( vecSrc.z - 32 );
			WRITE_SHORT( g_sModelIndexSmoke ); //PRECACHE_MODEL ("sprites/steam1.spr");// smoke
			WRITE_BYTE( 15 ); // scale * 10
			WRITE_BYTE( 10 ); // framerate
		MESSAGE_END();
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSrc.x);
			WRITE_COORD( vecSrc.y);
			WRITE_COORD( vecSrc.z - 32 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 55 ); // scale * 10
			WRITE_BYTE( 8 ); // framerate
		MESSAGE_END();
	}
	
	if (pev->dmgtime + RANDOM_FLOAT( 0, 8 ) > gpGlobals->time)
	{
		UTIL_Sparks( vecSrc );
	}

	if (m_fSequenceFinished && pev->dmgtime + 5 < gpGlobals->time)
	{
		pev->framerate = 0;
		SetThink( NULL );
	}
	
	if (pl != NULL && (pl->edict() == pev->owner))
		pl->m_flNextChatTime13 -= 1;
	
}

