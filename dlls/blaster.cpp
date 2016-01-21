/*      New weapon!
  Modelled after a tutorial by PsychoDude (eresnick@u.arizona.edu)
*/
#include "extdll.h"
#include "decals.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"

// Basic weapon constants:

#define WEAPON_BLASTER          16
#define BLASTER_SLOT            0//4
#define BLASTER_POSITION                1//4
#define BLASTER_WEIGHT          25
#define BLASTER_FIRE_DELAY      0.1
#define BLASTER_DEFAULT_AMMO    25
#define BLASTER_MAX_AMMO                100
#define BLASTER_DAMAGE          32


#define BLASTER_MODEL_1STPERSON "models/v_9mmhandgun.mdl" // What we see
#define BLASTER_MODEL_3RDPERSON "models/p_9mmhandgun.mdl" // What they see
#define BLASTER_MODEL_WORLD     "models/w_crossbow.mdl"
#define BLASTER_SOUND_SHOOT     "weapons/electro5.wav"
#define BLASTER_SOUND_VOLUME    0.25
#define BLASTER_BEAM_RED                192
#define BLASTER_BEAM_GREEN              0
#define BLASTER_BEAM_BLUE               255
#define BLASTER_BEAM_BRIGHTNESS 255
#define BLASTER_BEAM_WIDTH      2
#define BLASTER_BEAM_SPRITE     "sprites/smoke.spr"
#define BLASTER_BEAM_SPEED      1000
#define BLASTER_BEAM_LENGTH     4
#define BLASTER_BEAM_RANDOMNESS 15.0

//Hack to get the beam to come out of the gun ;)
#define BLASTER_OFFSET_FORWARD  24
#define BLASTER_OFFSET_RIGHT    8
#define BLASTER_OFFSET_UP               -8

// Animations:

//Enum automatically numbers these from 0 upwards...
//They tie in directly with the order of the animations in the model file...
enum    Blaster_Animations
{
        BLASTER_ANIM_IDLE1      = 0,
        BLASTER_ANIM_IDLE2,
        BLASTER_ANIM_IDLE3,
        BLASTER_ANIM_SHOOT,
        BLASTER_ANIM_SHOOT_EMPTY,
        BLASTER_ANIM_RELOAD,
        BLASTER_ANIM_RELOAD_NOSHOT,
        BLASTER_ANIM_DRAW,
        BLASTER_ANIM_HOLSTER,
        BLASTER_ANIM_ADD_SILENCER
};

// The CBlaster class:

class   CBlaster : public CBasePlayerWeapon
{
        public:

        void    Spawn           ( );
        void    Precache        ( );
        int     iItemSlot       ( );
        int     GetItemInfo     ( ItemInfo* );

        void    PrimaryAttack   ( );
        BOOL    Deploy          ( );
        void    WeaponIdle      ( );

//      BeamSprite isn't need in this class anymore.
//      int     BeamSprite;
};

// Associate it with the Worldcraft entity:

LINK_ENTITY_TO_CLASS( weapon_blaster, CBlaster );

// The CBlasterBeam class:

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

// Associate it with the Worldcraft entity:

LINK_ENTITY_TO_CLASS( proj_blaster, CBlasterBeam );

/**********************************

CBLASTER STUFF:

**********************************/

// Spawn: called when the weapon spawns in the world.

void CBlaster :: Spawn( )
{
        pev->classname = MAKE_STRING( "weapon_blaster" );
        Precache( );

        m_iId = WEAPON_BLASTER;
        SET_MODEL( ENT(pev), BLASTER_MODEL_WORLD );

        m_iDefaultAmmo = BLASTER_DEFAULT_AMMO;

        FallInit( );
}

// Precache: Stores resources in system RAM.

void CBlaster :: Precache( )
{
        PRECACHE_MODEL( BLASTER_MODEL_1STPERSON );
        PRECACHE_MODEL( BLASTER_MODEL_3RDPERSON );
        PRECACHE_MODEL( BLASTER_MODEL_WORLD );
        PRECACHE_SOUND( BLASTER_SOUND_SHOOT );
}

// iItemSlot: returns the item slot??

int CBlaster :: iItemSlot( )
{
        return BLASTER_SLOT;
}

// GetItemInfo: sets a lot of the weapon information.
//              Always returns 1.

int CBlaster :: GetItemInfo( ItemInfo* Info )
{
        Info->pszName           = STRING( pev->classname );
        Info->pszAmmo1          = "uranium";
        Info->iMaxAmmo1         = BLASTER_MAX_AMMO;
        Info->pszAmmo2          = NULL;
        Info->iMaxAmmo2         = -1;
        Info->iMaxClip          = 1;
        Info->iSlot                     = BLASTER_SLOT;
        Info->iPosition         = BLASTER_POSITION;
        Info->iFlags            = 0;
        Info->iId                       = WEAPON_BLASTER;
        Info->iWeight           = BLASTER_WEIGHT;
        //Info->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE; //Not sure, maybe it is limited to a certain 
ammount of instances (?)
        return 1;
}

// Deploy: called when the weapon is deployed, or drawn.

BOOL CBlaster :: Deploy( )
{
        return DefaultDeploy( BLASTER_MODEL_1STPERSON,
                              BLASTER_MODEL_3RDPERSON,
                              BLASTER_ANIM_DRAW,
                              "onehanded" );
        ALERT( at_console, "Blaster Selected");
        //Well THAT did something. Not. Why doesn't this seem to get called?
}

// PrimaryAttack: called when the user presses the fire button.

void CBlaster :: PrimaryAttack( )
{
        if( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 ) return;

        m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
        SendWeaponAnim( BLASTER_ANIM_SHOOT);

        EMIT_SOUND_DYN( ENT( m_pPlayer->pev ),
                        CHAN_WEAPON,
                        BLASTER_SOUND_SHOOT,
                        BLASTER_SOUND_VOLUME,
                        ATTN_NORM,
                        0,
                        150 );

        m_flNextPrimaryAttack = gpGlobals->time + BLASTER_FIRE_DELAY;
        m_flTimeWeaponIdle = gpGlobals->time + BLASTER_FIRE_DELAY;

        m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] --;

        UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

        Vector GunPosition = m_pPlayer->GetGunPosition( );
        GunPosition = GunPosition + gpGlobals->v_forward * BLASTER_OFFSET_FORWARD;
        GunPosition = GunPosition + gpGlobals->v_right   * BLASTER_OFFSET_RIGHT;
        GunPosition = GunPosition + gpGlobals->v_up      * BLASTER_OFFSET_UP;


        //Essentially, create a beam entity
        //The reason for this is we don't want the beams to be instantaneous beams (gauss gun, etc)
        CBlasterBeam* Beam = CBlasterBeam :: Create( GunPosition,
                                                 m_pPlayer->pev->v_angle,
                                                 m_pPlayer );

        float RandomX = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
                                       BLASTER_BEAM_RANDOMNESS );
        float RandomY = RANDOM_FLOAT( -BLASTER_BEAM_RANDOMNESS,
                                       BLASTER_BEAM_RANDOMNESS );

        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_right * RandomX;
        Beam->pev->velocity = Beam->pev->velocity + gpGlobals->v_up    * RandomY;

}

// WeaponIdle: called when the weapon is idle.

void CBlaster :: WeaponIdle( )
{
//      Is it time to stop being idle?
        if( m_flTimeWeaponIdle > gpGlobals->time ) return;

        m_flTimeWeaponIdle = gpGlobals->time + 3.0;
        SendWeaponAnim( BLASTER_ANIM_IDLE1 );
}

/**********************************

CBLASTERBEAM STUFF:

**********************************/

void    CBlasterBeam :: Spawn( )
{
        Precache( );
        SET_MODEL( ENT(pev), "models/rpgrocket.mdl" );
        pev->movetype = MOVETYPE_TOSS; //So gravity affects it a *tad*
        pev->solid = SOLID_BBOX;
        pev->rendermode = kRenderTransTexture;
        pev->renderamt = 0;

        UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) );//Point sized bounding box
        UTIL_SetOrigin( pev, pev->origin );
        pev->classname = MAKE_STRING( "proj_blaster" );//PROJectile from BLASTER

        SetThink( MoveThink );
        SetTouch( Hit );

        pev->angles.x -= 30;
        pev->velocity = gpGlobals->v_forward * BLASTER_BEAM_SPEED;
        pev->angles.x = -(pev->angles.x + 30);

        pev->gravity = 0.075;//A SMIDGEN of gravity. Can you HAVE a smidgen of gravity??
        pev->nextthink = gpGlobals->time + 0.1;//10 times a second
        pev->dmg = BLASTER_DAMAGE;
}

void    CBlasterBeam :: Precache( )
{
        BeamSprite = PRECACHE_MODEL( BLASTER_BEAM_SPRITE );
        PRECACHE_MODEL( "models/rpgrocket.mdl" );
}
//We hit something (yay)
void    CBlasterBeam :: Hit( CBaseEntity* Target )
{
        TraceResult TResult;
        Vector      StartPosition;

        pev->enemy = Target->edict( );
        StartPosition = pev->origin - pev->velocity.Normalize() * 32;

        UTIL_TraceLine( StartPosition,
                        StartPosition + pev->velocity.Normalize() * 64,
                        dont_ignore_monsters,
                        ENT( pev ),
                        &TResult );

        Explode( &TResult, DMG_BLAST );
}

void    CBlasterBeam :: Explode( TraceResult* TResult, int DamageType )
{
        RadiusDamage( pev,
                      VARS( pev->owner ),
                      pev->dmg,
                      CLASS_NONE,
                      DamageType );

        if( TResult->fAllSolid ) return;
        UTIL_DecalTrace( TResult, DECAL_GUNSHOT1 );

        SUB_Remove( );
}

CBlasterBeam* CBlasterBeam :: Create( Vector Pos, Vector Aim, CBaseEntity* Owner )
{
        CBlasterBeam* Beam = GetClassPtr( (CBlasterBeam*)NULL );

        UTIL_SetOrigin( Beam->pev, Pos );
        Beam->pev->angles = Aim;
        Beam->Spawn( );
        Beam->SetTouch( CBlasterBeam :: Hit );
        Beam->pev->owner = Owner->edict( );

        return Beam;
}

void    CBlasterBeam :: MoveThink( )
{
        MESSAGE_BEGIN           ( MSG_BROADCAST, SVC_TEMPENTITY );

                WRITE_BYTE      ( TE_BEAMFOLLOW );
                WRITE_SHORT     ( entindex() );
                WRITE_SHORT     ( BeamSprite );
                WRITE_BYTE      ( BLASTER_BEAM_LENGTH );
                WRITE_BYTE      ( BLASTER_BEAM_WIDTH );
                WRITE_BYTE      ( BLASTER_BEAM_RED );
                WRITE_BYTE      ( BLASTER_BEAM_GREEN );
                WRITE_BYTE      ( BLASTER_BEAM_BLUE );
                WRITE_BYTE      ( BLASTER_BEAM_BRIGHTNESS );

        MESSAGE_END             ( );
}
