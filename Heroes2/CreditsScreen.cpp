#include "CreditsScreen.h"
#include "GameManager.h"

CreditsScreen::CreditsScreen( string n )
{
    m_Name = n;
}

CreditsScreen::~CreditsScreen()
{}

void CreditsScreen::Initialize()
{
    auto & Game = GameManager::GetSingleton();
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Projekt Heroes",  20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Autor",           28, GetRGBA( eColor::DARK_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Marzena Kruszy",  20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Kierunek",        28, GetRGBA( eColor::DARK_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Informatyka",     20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Semestr",         28, GetRGBA( eColor::DARK_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Siódmy",          20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Grupa",           28, GetRGBA( eColor::DARK_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "GKiO",            20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "Rok akademicki ", 28, GetRGBA( eColor::DARK_GOLD ) ) );
    m_Credits.push_back( make_shared<Image>( ANDLSO, "2016/2017",       20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    SetTextPos( eWindow::WIDTH * 3 / 4, eWindow::HEIGHT / 10 );
    m_CurImage = make_shared<Image>( Game.FindImage( "MSbackground" ) );
    auto rect = m_CurImage->GetRect();
    rect.w = eWindow::WIDTH;
    rect.h = eWindow::HEIGHT;
    m_CurImage->SetRect( rect );
    m_Next = m_Previous = Game.FindScreen( "MenuScreen" );
}

void CreditsScreen::Reset()
{}

void CreditsScreen::Actualize()
{
    auto& Game = GameManager::GetSingleton();
    SDL_Event e = Game.GetEvent();
    if( Quit( e ) )
    {
        m_IsActive = false;
        return;
    }
}

void CreditsScreen::Render()
{
    auto & Game = GameManager::GetSingleton();
    m_CurImage->RenderTexture();
    for( auto text : m_Credits )
        text->RenderTexture();
}

const CreditsScreen & CreditsScreen::operator+=( ImagePtr & text )
{
    m_Credits.push_back( text );
    return *this;
}

void CreditsScreen::SetTextPos( int x, int y )
{
    m_Credits[0]->SetRect( x - m_Credits[0]->GetRect().w / 2, y ); 
    for( unsigned int i = 1; i < m_Credits.size(); i++ )
    {
        m_Credits[i]->SetRect( x - m_Credits[i]->GetRect().w / 2, m_Credits[i - 1]->GetRect().y + m_Credits[i - 1]->GetRect().h ); 
    }
}