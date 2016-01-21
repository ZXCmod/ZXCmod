// Basic weapon constants:

#include "explode.h"

#define WEAPON_BLASTER          17
#define BLASTER_SLOT            0//4
#define BLASTER_POSITION                1//4
#define BLASTER_WEIGHT          8
#define BLASTER_FIRE_DELAY      0.1
#define BLASTER_DEFAULT_AMMO    25
#define BLASTER_MAX_AMMO                100
#define BLASTER_DAMAGE          63


#define BLASTER_MODEL_1STPERSON "models/v_9mmhandgun.mdl" // What we see
#define BLASTER_MODEL_3RDPERSON "models/p_9mmhandgun.mdl" // What they see
#define BLASTER_MODEL_WORLD     "models/w_crossbow.mdl"
#define BLASTER_SOUND_SHOOT     "weapons/electro5.wav"
#define BLASTER_SOUND_VOLUME    0.25
#define BLASTER_BEAM_BRIGHTNESS 255
#define BLASTER_BEAM_WIDTH      5
#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"
#define BLASTER_BEAM_SPEED      800
#define BLASTER_BEAM_LENGTH     12
#define BLASTER_BEAM_RANDOMNESS 15.0
#define FLASH_RANGE 600

//Hack to get the beam to come out of the gun ;)
#define BLASTER_OFFSET_FORWARD  24
#define BLASTER_OFFSET_RIGHT    8
#define BLASTER_OFFSET_UP               -8


class   CBlasterBeamS : public CGrenade
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        void    MoveThink       ( );
        void EXPORT Hit         ( CBaseEntity* );
        void    Explode         ( TraceResult*, int);
        static CBlasterBeamS* Create( Vector, Vector, CBaseEntity* );

        int     BeamSprite;
		
		//void FireBubbles( void );



};











class CBlasterS : public CBasePlayerWeapon
{    
public: 

   void Spawn( void ); 
   void Precache( void );
   CBasePlayer	*m_pPlayer;

private:

   EHANDLE m_hOwner;

};

