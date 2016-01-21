
class   CGrav1 : public CBaseEntity
{
        private:

        void    Spawn            ( void );
		void 	Precache 		 ( void );
        void    EXPORT MoveThink ( );
        void    Explode          ( void );
		
		private:
		int 	m_flDie;
		int 	m_flDie2;
		int     BeamSprite;
		int 	m_iSpriteTexture;
		short	m_LaserSprite;
		short	m_Sprite;
};

class   CSmoke : public CBaseEntity
{
        private:

        void    Spawn            ( void );
		void 	Precache 		 ( void );
        void    EXPORT MoveThink ( );
		
		private:
		int 	m_flDie;
		short	m_Sprite;
};


LINK_ENTITY_TO_CLASS( weapon_saa, CGrav1 );
LINK_ENTITY_TO_CLASS( weapon_canister, CSmoke );

