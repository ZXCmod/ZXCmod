
class CTripmineGrenade : public CGrenade
{

	public:
		void Spawn( void );
		void Precache( void );

		int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );

		virtual int		Save( CSave &save );
		virtual int		Restore( CRestore &restore );
		static	TYPEDESCRIPTION m_SaveData[];
		
		void EXPORT WarningThink( void );
		void EXPORT PowerupThink( void );
		void EXPORT BeamBreakThink( void );
		void EXPORT DelayDeathThink( void );
		void Killed( entvars_t *pevAttacker, int iGib );

		void MakeBeam( void );
		void KillBeam( void );

	
		edict_t		*m_pRealOwner;
		Vector		m_TelevecEnd;

		edict_t *Owner( void ) { return m_pRealOwner; };

	private:

		float		m_flPowerUp;
		Vector		m_vecDir;
		Vector		m_vecEnd;
		float		m_flBeamLength;

		EHANDLE		m_hOwner;
		CBeam		*m_pBeam;
		Vector		m_posOwner;
		Vector		m_angleOwner;
	
};

