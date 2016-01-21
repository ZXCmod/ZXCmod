

#define BLASTER_BEAM_RED                RANDOM_LONG( 1, 64 )
#define BLASTER_BEAM_GREEN              RANDOM_LONG( 1, 64 )
#define BLASTER_BEAM_BLUE               RANDOM_LONG( 128, 240 )
#define BLASTER_BEAM_BRIGHTNESS 255
#define BLASTER_BEAM_WIDTH      RANDOM_LONG( 1, 3 )
#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"
#define BLASTER_BEAM_SPEED      854
#define BLASTER_DAMAGE          RANDOM_LONG( 43, 49 )
#define BLASTER_BEAM_LENGTH     RANDOM_LONG( 2, 22 )
#define BLASTER_BEAM_RANDOMNESS 1
#define BLASTER_OFFSET_FORWARD  0
#define BLASTER_OFFSET_RIGHT    7
#define BLASTER_OFFSET_UP               0
int m_iSpriteTexture;

class   CBlasterBeam : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void EXPORT Hit         ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static CBlasterBeam* Create( Vector, Vector, CBaseEntity* );

        int     BeamSprite;
		

};
