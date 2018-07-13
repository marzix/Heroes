#include "Declarations.h"
#include "Constants.h"

SDL_Color GetRGBA( eColor::Type c )
{
    SDL_Color color;
    color.a = c;
    color.b = c >> 8;
    color.g = c >> 16;
    color.r = c >> 24;

    return color;
}

SHOT_TYPE StringToShotType( string name )
{
    if( name == "RAY" )
        return SHOT_TYPE::RAY;
    else
        return SHOT_TYPE::BULLET;
}