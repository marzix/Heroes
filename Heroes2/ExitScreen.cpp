#include "ExitScreen.h"
#include "GameManager.h"
#include "MenuScreen.h"
#include "BattleScreen.h"

ExitScreen::ExitScreen( string n )
{
    m_Name = n;
}

ExitScreen::~ExitScreen()
{}

void ExitScreen::Initialize()
{
    auto& Game = GameManager::GetSingleton();
    if( m_Name == "exitG" )
    {
        m_Previous = Game.FindScreen( "MenuScreen" );
        m_Next = nullptr;
    }
    else
    {
        m_Previous = Game.FindScreen( "BattleScreen" );
        m_Next = Game.FindScreen( "MenuScreen" );
    }
    m_CurImage = make_shared<Image>( Game.FindImage( "EBbackground" ) );
    int x = eWindow::WIDTH / 2 - m_CurImage->GetRect().w / 2;
    int y = eWindow::HEIGHT / 2 - m_CurImage->GetRect().h / 2;
    m_CurImage->SetRect( x, y );

    m_Tick = make_shared<Button>( 0, 0, 
        make_shared<Image>( Game.FindImage( "buttonTick1" ) ), 
        nullptr, nullptr,
        make_shared<Image>( Game.FindImage( "buttonTick2" ) ), NextScreen );
    x += m_CurImage->GetRect().w / 2 - ( m_Tick->GetImage()->GetRect().w * 8 / 7 );
    y += m_CurImage->GetRect().h / 2 + ( m_Tick->GetImage()->GetRect().h * 8 / 7 ) + 2;
    m_Tick->SetPosition( x, y );

    m_Cancel = make_shared<Button>( 0, 0, 
        make_shared<Image>( Game.FindImage( "buttonCancel1" ) ), 
        nullptr, nullptr,
        make_shared<Image>( Game.FindImage( "buttonCancel2" ) ), PreviousScreen );
    x += m_Cancel->GetImage()->GetRect().w * 5 / 4 + 2;
    m_Cancel->SetPosition( x, y );

    m_Cancel->Initialize();
    m_Tick->Initialize();
}

void ExitScreen::Reset()
{}

void ExitScreen::Actualize()
{
    auto& Game = GameManager::GetSingleton();
    SDL_Event e = Game.GetEvent();
    if( Quit( e ) || e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
    {
        m_IsActive = false;
        return;
    }    
    m_Tick->Actualize();
    m_Cancel->Actualize();
}

void ExitScreen::Render()
{
    auto& Game = GameManager::GetSingleton();
    SDL_RenderClear( Game.ren );
    RenderPrevScreenAsBackground();
    m_CurImage->RenderTexture();
    m_Tick->GetImage()->RenderTexture();
    m_Cancel->GetImage()->RenderTexture();
    Game.RenderCursor();
    SDL_RenderPresent( Game.ren );
}

void ExitScreen::RenderPrevScreenAsBackground()
{
    auto& Game = GameManager::GetSingleton();
    m_Background.CreateRenderTarget();
    if( m_Previous && m_Background.GetTex() )
        m_Previous->Render();
    m_Background.RenderTarget();
}