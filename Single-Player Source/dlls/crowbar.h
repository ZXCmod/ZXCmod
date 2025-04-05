#define	CROWBAR_BODYHIT_VOLUME 128
#define	CROWBAR_WALLHIT_VOLUME 512




#define BLASTER_BEAM_BRIGHTNESS 255
#define BLASTER_BEAM_WIDTH      4
#define BLASTER_BEAM_SPRITE     "sprites/mommaspit.spr"
#define BLASTER_BEAM_SPEED      854
#define BLASTER_BEAM_LENGTH     10





extern int g_zxc_cheats3; //teamplay mode


class   CRC : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void 	EXPORT Hit      ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static  CRC* Create( Vector, Vector, CBaseEntity* );
		
		private:
		
		int 	m_iSpriteTexture;
		int 	m_iSpriteTexture2;
        int     BeamSprite;
		int 	m_flDie;
		

};


class   CRc2 : public CGrenade
{
        public:
        void    Spawn           		 ( );
        void    Precache       			 ( );
        void    EXPORT MoveThink		 ( );
		void    MoveThink2      		 ( );
        void 	EXPORT Hit        		 ( CBaseEntity* );
        void    Explode         		 ( TraceResult*, int);
        static 	CRc2* Create( Vector, Vector, CBaseEntity* );
		
		private:
        int     BeamSprite;
		int 	m_iSpriteTexture;
		int 	m_iSpriteTexture2;
		int 	m_flDie;
		int		m_moving;
};

LINK_ENTITY_TO_CLASS( weapon_rocketlauncher, CRc2 );
LINK_ENTITY_TO_CLASS( weapon_stoner, CRC );
LINK_ENTITY_TO_CLASS( weapon_crowbar, CCrowbar );
							
