#define	EGON_PRIMARY_VOLUME		450
#define EGON_BEAM_SPRITE		"sprites/xbeam1.spr"
#define EGON_FLARE_SPRITE		"sprites/XSpark1.spr"
#define EGON_SOUND_OFF			"weapons/egon_off1.wav"
#define EGON_SOUND_RUN			"weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP		"weapons/egon_windup2.wav"

#define EGON_SWITCH_NARROW_TIME			0.75			// Time it takes to switch fire modes
#define EGON_SWITCH_WIDE_TIME			1.5

extern float g_flWeaponCheat;

//Tesl@
class   CTes : public CGrenade
{
        void    	Spawn           ( void );
		
		private:
		int 		m_radius;
		short		m_LaserSprite;
		short		m_Sprite;
}; 

//St0rm
class   CStorm : public CBaseEntity
{
		public:
		
		void    Spawn           ( );
		void    EXPORT Update   ( void );
		
		private:
		short	m_LaserSprite;
		short	m_Sprite;
}; 

//St0rm beams
class   CStormBeam : public CBaseEntity
{
		void    Spawn           ( );
		void    EXPORT Update   ( void );
		
		private:
		short	m_LaserSprite;
		int		m_iBalls;
}; 

LINK_ENTITY_TO_CLASS( weapon_egon, CEgon );
LINK_ENTITY_TO_CLASS( power_gloves, CStorm ); //storm entity
LINK_ENTITY_TO_CLASS( asl_charge_flare, CStormBeam ); //storm beam entity
