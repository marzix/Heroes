#ifndef __CONSTANTS__
#define __CONSTANTS__

#include <iostream>
#include <string>

using namespace std;

//----------Window dimensions and position---------//
namespace eWindow {
    typedef enum: unsigned int {
        WIDTH = 800,
        HEIGHT = 600,
        POS_X = 100,
        POS_Y = 100
    }Type;
}

//---------------Textures directory----------------//
const string TEXTURE_FILE = "textures.xml";
const string loadingScreenFile = "../images/LoadingScreenV2.png";

//----------------Audio directory------------------//
const string AUDIO_FILE = "audio.xml";

//----------------Font directories-----------------//
const string ANDLSO = "../images/andlso.ttf";
const string TIMES = "../images/times.ttf";

//-----------Battlefield grid dimensions-----------//
enum: unsigned int{ GRID_ROW = 11 };
enum: unsigned int{ GRID_COLUMN = 15 };

//------------------Colors' keys-------------------//
//                Format: 0xRRGGBBAA               //
namespace eColor {
    typedef enum: Uint32{
        WHITE           = 0xFFFFFFFF,
        CYAN            = 0xFF00FFFF,
        MAGENTA         = 0xFFFF00FF,
        LIGHT_MAGENTA   = 0xFFFF96FF,
        VIOLET          = 0xFFB400FF,
        GREEN           = 0xFF00FF00,
        YELLOW          = 0xFFFFFF00,
        _TRANSPARENT    = 0x00FFFFFF,
        SHADOW          = 0x80000000,
        LIGHT_GOLD      = 0xEFD77BFF,
        DARK_GOLD       = 0xFAC300FF
    }Type;
}

//------------------Player Colors-------------------//
namespace PLAYER_COLOR {
    typedef enum: short int
    {
        RED,
        BLUE,
        PURPLE,
        GREEN
    }Color;

    static string ToString( Color color )
    {
        switch( color )
        {
            case RED:       return "RED";
            case BLUE:      return "BLUE";
            case PURPLE:    return "PURPLE";
            case GREEN:     return "GREEN";
            default:        return "";
        }
    }
}

//------------------Unit's states------------------//
namespace STATE {
    typedef enum: short int { 
        STAND       = 1, 
        IDLE        = 2, 
        ACTIVE      = 3,
        MOVE        = 4,
        TURN        = 6, 
        DEFEND      = 7, 
        ATTACK      = 8,
        GET_HIT     = 9,
        DIE         = 10,
        START_MOVE  = 11,
        END_MOVE    = 12,
        SHOOT       = 13
    }Type;

    static string ToString( short int type)
    {
        switch( type )
        {
            case 1  : return "STAND"     ; 
            case 2  : return "IDLE"      ;
            case 3  : return "ACTIVE"    ;
            case 4  : return "MOVE"      ;
            case 6  : return "TURN"      ; 
            case 7  : return "DEFEND"    ;
            case 8  : return "ATTACK"    ;
            case 9  : return "GET_HIT"   ;
            case 10 : return "DIE"       ;
            case 11 : return "START_MOVE";
            case 12 : return "END_MOVE"  ;
            case 13 : return "SHOOT"     ;
            default : return "STAND"     ;
        }
    }
    static string ToString( Type type)
    {
        switch( type )
        {
            case STAND       : return "STAND"     ;
            case IDLE        : return "IDLE"      ;
            case ACTIVE      : return "ACTIVE"    ;
            case START_MOVE  : return "START_MOVE";
            case MOVE        : return "MOVE"      ;
            case END_MOVE    : return "END_MOVE"  ;
            case TURN        : return "TURN"      ;
            case DEFEND      : return "DEFEND"    ;
            case ATTACK      : return "ATTACK"    ;
            case GET_HIT     : return "GET_HIT"   ;
            case DIE         : return "DIE"       ;
            case SHOOT       : return "SHOOT"     ;
            default          : return "STAND"     ;
        }
    }
    static Type GetState( string name )
    {
        if( name == "STAND" )
            return STAND;
        if( name == "IDLE" )
            return IDLE;
        if( name == "ACTIVE" )
            return ACTIVE;
        if( name == "START_MOVE" )
            return START_MOVE;
        if( name == "MOVE" )
            return MOVE;
        if( name == "END_MOVE" )
            return END_MOVE;
        if( name == "TURN" )
            return TURN;
        if( name == "DEFEND" )
            return DEFEND;
        if( name == "ATTACK" || name == "ATTACK_UP" || name == "ATTACK_DOWN" )
            return ATTACK;
        if( name == "GET_HIT" )
            return GET_HIT;
        if( name == "DIE" )
            return DIE;
        if( name == "SHOOT" || name == "SHOOT_UP" || name == "SHOOT_DOWN" )
            return SHOOT;
        else
            return STAND;
    }
};

//------------------Unit's Type------------------//
namespace TYPE
{
    typedef enum: short int {

        TERRESTRIAL,
        FLYING,
        ARCHER
    }Name;
};

//------------------Unit's turn direction and Player's side------------------//
enum SIDE { LEFT, RIGHT, NONE };

//-----------------------Archers's straight arrow range----------------------//
const unsigned int GOOD_SHOT_RANGE = 8;

//---------------------------Archers's shot type---------------------------//
enum SHOT_TYPE { RAY, BULLET };

#endif