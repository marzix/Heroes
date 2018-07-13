#include "MenuScreen.h"
#include "GameManager.h"
#include "AudioManager.h"

MenuScreen::MenuScreen( string n )
{
    m_Name = n;
}

MenuScreen::~MenuScreen()
{
    m_Buttons.clear();
}

void MenuScreen::Initialize()
{
    auto & Game = GameManager::GetSingleton();
    m_CurImage = make_shared<Image>( Game.FindImage( "MSbackground" ) );
    m_Previous = Game.FindScreen( "exitG" );
    m_Next = Game.FindScreen( "OptionScreen" );

    m_Buttons.push_back( 
        make_shared<Button>( 540, 60, make_shared<Image>( Game.FindImage( "buttonNewGame1" ) ), 
                                      make_shared<Image>( Game.FindImage( "buttonNewGame2" ) ),
                                      nullptr,
                                      make_shared<Image>( Game.FindImage( "buttonNewGame3" ) ), 
                            NextScreen ) );
    m_Buttons.push_back( 
        make_shared<Button>( 580, 380, make_shared<Image>( Game.FindImage( "buttonExit1" ) ), 
                                       make_shared<Image>( Game.FindImage( "buttonExit2" ) ),
                                       nullptr,
                                       make_shared<Image>( Game.FindImage( "buttonExit3" ) ), 
                            PreviousScreen ) );
    m_Buttons.push_back( 
        make_shared<Button>( 555, 220, make_shared<Image>( Game.FindImage( "buttonAuthor1" ) ), 
                                       make_shared<Image>( Game.FindImage( "buttonAuthor2" ) ), 
                                       nullptr,
                                       make_shared<Image>( Game.FindImage( "buttonAuthor3" ) ), 
                            [=]() { auto & Game = GameManager::GetSingleton();
                                    Game.SetScreen( Game.FindScreen( "credits" ) ); } ) );
    for( auto& button : m_Buttons )
    {
        if( button )
            button->Initialize();
    }
    m_IsActive = true;
}

void MenuScreen::Reset()
{}

void MenuScreen::Actualize()
{
    auto & Game = GameManager::GetSingleton();
    SDL_Event e = Game.GetEvent();
    if( Quit( e ) )
    {
        m_IsActive = false;
        return;
    }
    for( auto& button : m_Buttons )
    {
        button->Actualize();
    }
    AudioManager::GetSingleton().PlayTrack( m_SoundTrackName );
}

void MenuScreen::Render()
{
    auto & Game = GameManager::GetSingleton();
    m_CurImage->RenderTexture();
    for( auto& button : m_Buttons )
    {
        button->Render();
    }
}

void MenuScreen::AddButton( ButtonPtr button )
{
    m_Buttons.push_back( button );
}