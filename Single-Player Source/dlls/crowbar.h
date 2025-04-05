#define	CROWBAR_BODYHIT_VOLUME 128
#define	CROWBAR_WALLHIT_VOLUME 512




#define BLASTER_BEAM_BRIGHTNESS 255
#define BLASTER_BEAM_WIDTH      4
#define BLASTER_BEAM_SPRITE     "sprites/mommaspit.spr"
#define BLASTER_BEAM_SPEED      854
#define BLASTER_BEAM_LENGTH     10





extern int g_zxc_cheats3; //teamplay mode


class   CRocketlinear : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    EXPORT MoveThink       ( );
        void 	EXPORT Hit      ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static  CRocketlinear* Create( Vector, Vector, CBaseEntity* );
		
		private:
		
		int 	m_iSpriteTexture;
		int 	m_iSpriteTexture2;
        int     BeamSprite;
		int 	m_flDie;
		

};


class   Crowbar_rocket_chaos : public CGrenade
{
        public:
        void    Spawn           		 ( );
        void    Precache       			 ( );
        void    EXPORT MoveThink		 ( );
        void 	EXPORT Hit        		 ( CBaseEntity* );
        void    Explode         		 ( TraceResult*, int);
        static 	Crowbar_rocket_chaos* Create( Vector, Vector, CBaseEntity* );
		
		private:
        int     BeamSprite;
		int 	m_iSpriteTexture;
		int 	m_iSpriteTexture2;
		int 	m_flDie;
		int		m_moving;
};

LINK_ENTITY_TO_CLASS( rocket_chaos, Crowbar_rocket_chaos );
LINK_ENTITY_TO_CLASS( weapon_rocketlinear, CRocketlinear );
LINK_ENTITY_TO_CLASS( weapon_crowbar, CCrowbar );
							
