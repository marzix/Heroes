#include "GameManager.h"

void logSDLError( char* msg )
{
	printf( "%s error: %s\n", msg, SDL_GetError() );
}

int GameInitialize( int winPosX, int winPosY, Uint32 windowFlag )
{
	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		logSDLError( "SDL_Init" );
		return 1;
	}

	if (( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) != IMG_INIT_PNG )
	{
		logSDLError( "IMG_Init" );
		return 2;
	}

	if ( TTF_Init() != 0 )
	{
		logSDLError( "TTF_Init" );
		return 3;
	}
    if( !GameManager::GetSingleton().Initialize( winPosX, winPosY, windowFlag ) )
        return 4;

    return 0;
}