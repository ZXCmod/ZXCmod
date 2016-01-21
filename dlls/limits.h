//here be only limit variables


/*
example of limit next think!!!


1. init in the class:
int m_flNext;




2. in the spawn:

	m_flNext = gpGlobals->time;




2.

if (  m_pPlayer->m_flNext < gpGlobals->time )
{
	m_pPlayer->m_flNext = gpGlobals->time + 10;
	return;
}
















////////////////////////
pev->movetype

This is used to tell the physics engine how to move your entity around. You can have:

      MOVETYPE_NONE
          Never Moves
      MOVETYPE_WALK
          (Player only)
      MOVETYPE_STEP
          Gravity, special edge handling, monsters use this
      MOVETYPE_FLY
          No gravity, but still collides with stuff.
      MOVETYPE_TOSS
          gravity/collisions
      MOVETYPE_PUSH
          no clip to world, push and crush
      MOVETYPE_NOCLIP
          no gravity, no collisions, still do velocity/avelocity.
      MOVETYPE_FLYMISSILE
          extra size to monsters
      MOVETYPE_BOUNCE
          Just like toss, but reflect velocity when contacting surfaces.
      MOVETYPE_BOUNCEMISSILE
          Bounce without gravity
      MOVETYPE_FOLLOW
          track movement of aiment
      MOVETYPE_PUSHSTEP
          BSP model that needs physics/world collisions (uses nearest hull for world collisions.) 

		  
		  
		  
		  
		  
		  
		  
		  
SET_VIEW( pActivator->edict(), edict() );
		  
		if (m_hPlayer->IsAlive( ))
		{
			SET_VIEW( m_hPlayer->edict(), m_hPlayer->edict() );
			((CBasePlayer *)((CBaseEntity *)m_hPlayer))->EnableControl(TRUE);
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
///from plugin




		new Float:distanceNadePl = vector_distance(entOrigin, hitPointOrigin)

		// Stuff on ground AND below explosion are "placed" a distance above explosion Y-wise ([2]), so that they fly off ground etc.
		if (entity_is_on_ground(entList[j]) && entOrigin[2] < hitPointOrigin[2])
			entOrigin[2] = hitPointOrigin[2] + distanceNadePl

		entity_get_vector(entList[j], EV_VEC_velocity, velocity)

		cOrigin[0] = (entOrigin[0] - hitPointOrigin[0]) * radius / distanceNadePl + hitPointOrigin[0]
		cOrigin[1] = (entOrigin[1] - hitPointOrigin[1]) * radius / distanceNadePl + hitPointOrigin[1]
		cOrigin[2] = (entOrigin[2] - hitPointOrigin[2]) * radius / distanceNadePl + hitPointOrigin[2]

		velocity[0] += (cOrigin[0] - entOrigin[0]) * power
		velocity[1] += (cOrigin[1] - entOrigin[1]) * power
		velocity[2] += (cOrigin[2] - entOrigin[2]) * power

		entity_set_vector(entList[j], EV_VEC_velocity, velocity)
		
		
		
		
		
		
	pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_forward * 400;
	pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_up * 400;
		
		
		
		
		
						UTIL_ScreenShake( pHurt->pev->origin, 25.0, 1.5, 0.7, 2 );

					if ( pHurt->IsPlayer() )
					{
						UTIL_MakeVectors( pev->angles );
						pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * 500 + gpGlobals->v_up * 300;
					}
					
					
					
					
					
					
					
					
					
					
					
					/////////////////////////////////////
					//////////////////////////////////
					///////////////////////////////
		
		
		
		
		
					CBaseEntity *ent = NULL;
			while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 128 )) != NULL )
			{
				// if ent is a client, kill em (unless they are ourselves)
				if ( ent->IsPlayer() && !(ent->edict() == player) )
					ent->TakeDamage( VARS(INDEXENT(0)), VARS(INDEXENT(0)), 300, DMG_GENERIC );
			}
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
//////////////THE MAIN CODe!!!!
int CBaseMonster :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	float	flTake;
	Vector	vecDir;

	if (!pev->takedamage)
		return 0;

	if ( !IsAlive() )
	{
		return DeadTakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
	}

	if ( pev->deadflag == DEAD_NO )
	{
		// no pain sound during death animation.
		PainSound();// "Ouch!"
	}

	//!!!LATER - make armor consideration here!
	flTake = flDamage;

	// set damage type sustained
	m_bitsDamageType |= bitsDamageType;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector( 0, 0, 0 );
	if (!FNullEnt( pevInflictor ))
	{
		CBaseEntity *pInflictor = CBaseEntity :: Instance( pevInflictor );
		if (pInflictor)
		{
			vecDir = ( pInflictor->Center() - Vector ( 0, 0, 10 ) - Center() ).Normalize();
			vecDir = g_vecAttackDir = vecDir.Normalize();
		}
	}

	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// todo: remove after combining shotgun blasts?
	if ( IsPlayer() )
	{
		if ( pevInflictor )
			pev->dmg_inflictor = ENT(pevInflictor);

		pev->dmg_take += flTake;

		// check for godmode or invincibility
		if ( pev->flags & FL_GODMODE )
		{
			return 0;
		}
	}

	// if this is a player, move him around!
	if ( ( !FNullEnt( pevInflictor ) ) && (pev->movetype == MOVETYPE_WALK) && (!pevAttacker || pevAttacker->solid != SOLID_TRIGGER) )
	{
		pev->velocity = pev->velocity + vecDir * -DamageForce( flDamage*2 );
	}

	// do the damage
	pev->health -= flTake;

	
	// HACKHACK Don't kill monsters in a script.  Let them break their scripts first
	if ( m_MonsterState == MONSTERSTATE_SCRIPT )
	{
		SetConditions( bits_COND_LIGHT_DAMAGE );
		return 0;
	}

	if ( pev->health <= 0 )
	{
		g_pevLastInflictor = pevInflictor;

		if ( bitsDamageType & DMG_ALWAYSGIB )
		{
			Killed( pevAttacker, GIB_ALWAYS );
		}
		else if ( bitsDamageType & DMG_NEVERGIB )
		{
			Killed( pevAttacker, GIB_NEVER );
		}
		else
		{
			Killed( pevAttacker, GIB_NORMAL );
		}

		g_pevLastInflictor = NULL;

		return 0;
	}

	// react to the damage (get mad)
	if ( (pev->flags & FL_MONSTER) && !FNullEnt(pevAttacker) )
	{
		if ( pevAttacker->flags & (FL_MONSTER | FL_CLIENT) )
		{// only if the attack was a monster or client!
			
			// enemy's last known position is somewhere down the vector that the attack came from.
			if (pevInflictor)
			{
				if (m_hEnemy == NULL || pevInflictor == m_hEnemy->pev || !HasConditions(bits_COND_SEE_ENEMY))
				{
					m_vecEnemyLKP = pevInflictor->origin;
				}
			}
			else
			{
				m_vecEnemyLKP = pev->origin + ( g_vecAttackDir * 64 ); 
			}

			MakeIdealYaw( m_vecEnemyLKP );

			// add pain to the conditions 
			// !!!HACKHACK - fudged for now. Do we want to have a virtual function to determine what is light and 
			// heavy damage per monster class?
			if ( flDamage > 0 )
			{
				SetConditions(bits_COND_LIGHT_DAMAGE);
			}

			if ( flDamage >= 20 )
			{
				SetConditions(bits_COND_HEAVY_DAMAGE);
			}
		}
	}

	return 1;
}
			
			

*/