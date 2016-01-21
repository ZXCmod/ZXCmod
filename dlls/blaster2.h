#define BLASTER_BEAM_RED                RANDOM_LONG( 128, 255 )
#define BLASTER_BEAM_GREEN              RANDOM_LONG( 1, 3 )
#define BLASTER_BEAM_BLUE               RANDOM_LONG( 1, 3 )
#define BLASTER_BEAM_BRIGHTNESS 128
#define BLASTER_BEAM_WIDTH      RANDOM_LONG( 3, 4 )
#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"
#define BLASTER_BEAM_SPEED      300
#define BLASTER_DAMAGE          RANDOM_LONG( 60, 90 )
#define BLASTER_BEAM_LENGTH     RANDOM_LONG( 26, 34 )
#define BLASTER_BEAM_RANDOMNESS RANDOM_LONG( 1, 24 )
#define BLASTER_OFFSET_FORWARD  0
#define BLASTER_OFFSET_RIGHT    7
#define BLASTER_OFFSET_UP               0
int m_iSpriteTexture2;

class   CBlaster2Beam : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void EXPORT Hit         ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static CBlaster2Beam* Create( Vector, Vector, CBaseEntity* );
		CBasePlayer *pPlayer;
        int     BeamSprite;
		float m_flDie;
		float m_flDie2;
		float m_flDie3;
		void    Ef           ( );
		void    Explode2 ( TraceResult*, int);		float dmge;
		int rad;
};

class CBlaster2 : public CBasePlayerWeapon
{    
public: 
    void Reload( void );
   void Spawn( void ); 
   void Precache( void );
   CBasePlayer	*m_pPlayer;

private:

   EHANDLE m_hOwner;

};