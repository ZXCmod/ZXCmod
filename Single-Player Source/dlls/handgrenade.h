
class   CGrav1 : public CBaseEntity
{
        public:

        void    Spawn            ( void );
		void 	Precache 		 ( void );
        void    EXPORT MoveThink ( );
        void    EXPORT Touch	( CBaseEntity *pOther );
		
		private:
		int     BeamSprite;
		int 	m_iSpriteTexture;
		short	m_LaserSprite;
		short	m_Sprite;
		int 	radius; // add more radius per charge
		CBaseEntity *pEntity;
		TraceResult tr;
};

class   CSmoke : public CBaseEntity
{
        public:

        void    Spawn            ( void );
		void 	Precache 		 ( void );
        void    EXPORT MoveThink ( );
		
		private:
		int 	m_flDie;
		short	m_Sprite;
};


LINK_ENTITY_TO_CLASS( weapon_saa, CGrav1 );
LINK_ENTITY_TO_CLASS( weapon_canister, CSmoke );

