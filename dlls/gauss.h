

#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged
#define BLASTER_BEAM_RANDOMNESS RANDOM_LONG( 1, 24 )



//nuke class
class   CGaussNuke : public CGrenade
{
        public:
        void    Spawn           ( );
        void    MoveThink       ( );
        void 	EXPORT Hit      ( CBaseEntity* );
        void    Explode         ( TraceResult*, int );
        static 	CGaussNuke* Create( Vector, Vector, CBaseEntity* );
		CBasePlayer *pPlayer;
		
		private:
        int     BeamSprite;
		float 	m_flDie;
		float 	m_flDie2;
		float 	m_flDie3;
		void    Ef              ( );
		float 	dmge;
		int 	rad;
		int 	m_iSpriteTexture2;
		int 	m_sp;
};

//Radiation by nuke 
class   CRadiation : public CBaseEntity
{
        public:

        void    Spawn           ( );
        void    MoveThink       ( );
        void    Explode         ( );
		int m_expl;
		
		private:
		int m_flDie10;
		int m_iSpriteTexture2;
		int m_iSpriteTexture22;
		int	m_iBodyGibs;
		
};
LINK_ENTITY_TO_CLASS( trigger_killmonster, CRadiation );




//Gauss cannon 1.31 [3rd]
class   CGCannon : public CBaseEntity
{
        public:

        void    	Spawn           	   ( void );
        void    	EXPORT MoveThink       ( void );
		void 		EXPORT MoveTouch	   ( CBaseEntity *pOther );
		
		private:
		short		m_Sprite;
		short		m_SpriteExp;
		short		m_iSpriteTexture;
		float       m_timer;
		int 		m_iBalls;
};
LINK_ENTITY_TO_CLASS( virtual_hull, CGCannon );

//Gauss particle 1.31 [2nd]
class   CGParticle : public CBaseEntity
{

        public:
		
        void    	Spawn           	   ( void );
		void    	EXPORT MoveThink       ( void );
		
		private:
		short		m_Sprite;
		
};
LINK_ENTITY_TO_CLASS( env_beamtrail, CGParticle );



//Gauss particle 2 1.31 [2nd]
class   CGParticle2 : public CBaseEntity
{

        public:
        void    	Spawn           	   ( void );
		
		private:
		short		m_Sprite;
		
};
LINK_ENTITY_TO_CLASS( item_nvg, CGParticle2 );

/* //Gauss particle 2 1.31 [2nd]
class   CExpl : public CBaseEntity
{

        public:
        void    	Spawn           	   ( void );
		void    	MoveThink       	   ( void );
		
		private:
		short		m_Sprite;
		
};
LINK_ENTITY_TO_CLASS( gr_shell, CExpl );
 */


/////end new weap's/////

//original linker
LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );