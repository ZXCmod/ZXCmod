// Flash can

class   CFreeze : public CBaseEntity
{
        public:
        void    Spawn				( );
		void 	Precache			( );
        void 	EXPORT MoveThink	( );
		void 	EXPORT MoveTouch	( CBaseEntity *pOther );
		
		private:
		int 	m_flDie;
		int 	m_flDie2;
		int     BeamSprite;
		int 	m_iSpriteTexture;
		unsigned short	m_LaserSprite;
};

// 1.28 new Freeze bomb. 

class   CFreezeBomb : public CGrenade
{
        public:

        void    Spawn          		   ( );
        void    EXPORT MoveThink       ( void );
        void    Explode         	   ( void );
		void 	EXPORT Touch( CBaseEntity *pOther );
		void  	Exp( void );
		
		private:
		int 	m_flDie;
		
		
};

LINK_ENTITY_TO_CLASS( weapon_clip_generic, CFreeze );
LINK_ENTITY_TO_CLASS( player_freeze, CFreezeBomb);
