#include "GameManager.h"
#include "Constants.h"
#include "AudioManager.h"
#include <thread>
#include <chrono>

int GameMain( char* cmdline , int cmdshow );

// Windows
#if defined(_MSC_VER)
    #include <Windows.h>

int WINAPI WinMain(
    HINSTANCE hInstance,           
    HINSTANCE hPrevInstance,       
    LPSTR lpCmdLine,               
    int nCmdShow                   
    )
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    return GameMain( lpCmdLine , nCmdShow );
}
#elif defined __GNUC__
// Linux
int main( int argc, char* argv[] )
{
    return GameMain( 0 , 0 );
}

#else
// I don't know :(
    #error "Unknown compiler !"
#endif

int GameMain( char* cmdline , int cmdshow )
{
    auto & Game = GameManager::GetSingleton();
    auto& Audio = AudioManager::GetSingleton();
    bool run = true;

	if( GameInitialize( eWindow::POS_X, eWindow::POS_Y ) != 0 )
		return 1;

    if( Game.LoadFromFile( TEXTURE_FILE ) == false )
        return 2;

    if( !Audio.Initialize() )
        return 3;
    if( !Audio.LoadAudio() )
        return 4;

    Game.ScreenInit();
    Game.CursorInit();
    SDL_ShowCursor(0);
    Dialog * pCurrent = Game.GetScreen();
    srand( (uint)time( nullptr ) );

    while ( run )
    {
        Game.SetTime();
        Game.SetEvent();
        Game.CheckMousePos();
        Game.SetCursorDefault();
        Game.ResetMouseOn();
        pCurrent->Actualize();
        if( pCurrent != Game.GetScreen() || pCurrent->IsActive() == false )
        {
            pCurrent = Game.GetScreen();
            if( !pCurrent || pCurrent->IsActive() == false )
            {
                Audio.Close();
                return 0; 
            }
            else
            {
                continue;
            }
        }
        SDL_RenderClear( Game.ren );
        pCurrent->Render();
        Game.RenderCursor();
        SDL_RenderPresent( Game.ren );

        std::this_thread::sleep_for( std::chrono::milliseconds(1000/60) );
    }

    return 0;
}