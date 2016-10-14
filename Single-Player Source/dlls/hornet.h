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
//=========================================================
// Hornets
//=========================================================

//=========================================================
// Hornet Defines
//=========================================================
#define HORNET_TYPE_RED			0
#define HORNET_TYPE_ORANGE		1
#define HORNET_RED_SPEED		(float)800 //600
#define HORNET_ORANGE_SPEED		(float)1000 //800
#define	HORNET_BUZZ_VOLUME		(float)0.8

extern int iHornetPuff;

//=========================================================
// Hornet - this is the projectile that the Alien Grunt fires.
//=========================================================
class CHornet : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int	 Classify (  );
	int  IRelationship ( CBaseEntity *pTarget );


	void IgniteTrail( void );
	void EXPORT StartTrack ( void );
	void EXPORT StartDart ( void );
	void EXPORT TrackTarget ( void );
	void EXPORT TrackTouch ( CBaseEntity *pOther );
	void EXPORT DartTouch( CBaseEntity *pOther );
	void EXPORT DieTouch ( CBaseEntity *pOther );
	
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );

	private:
	float			m_flStopAttack;
	int				m_iHornetType;
	float			m_flFlySpeed;
	float 			m_flDie;
};

