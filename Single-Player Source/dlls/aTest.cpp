void CRpg::SecondaryAttack()
{
	if (m_iClip) 
	{
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		TraceResult tr;
		Vector trace_origin;
		trace_origin = m_pPlayer->pev->origin;
		if (m_pPlayer->pev->flags & FL_DUCKING)
		{
			trace_origin = trace_origin - (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
		}

		if (m_pPlayer->m_flNextTurretsLimit < m_limit)
		{
			UTIL_TraceHull(trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, ignore_monsters, head_hull, edict(), &tr);
			if (!tr.fStartSolid)
			{
				m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
				m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

#ifndef CLIENT_DLL
				m_pPlayer->SetAnimation(PLAYER_ATTACK1);
				UTIL_MakeVectors(m_pPlayer->pev->v_angle);
				Vector vecThrow = gpGlobals->v_forward; 
				CBaseEntity::Create("monster_sentry", m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, vecThrow, m_pPlayer->edict());
#endif
				PLAYBACK_EVENT(FEV_GLOBAL, m_pPlayer->edict(), m_usRpg);
				m_iClip--;
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
				m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
				m_pPlayer->m_flNextTurretsLimit++;
			}
		}
		else
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		}
		UpdateSpot();
	}
}