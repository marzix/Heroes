#include "EndScreen.h"
#include "GameManager.h"
#include "BattleScreen.h"
#include "Castle.h"
#include "OptionsScreen.h"
#include "AudioManager.h"

#pragma warning(disable : 4996)

EndScreen::EndScreen( string n )
{
    m_Name = n;
}

void EndScreen::Initialize()
{
    auto & Game = GameManager::GetSingleton();
    m_Previous = Game.FindScreen( "BattleScreen" );
    m_Next = Game.FindScreen( "MenuScreen" );
    m_CurImage = make_shared<Image>( Game.FindImage( "endFrame" ) );
    SDL_Rect r = m_CurImage->GetRect();
    m_CurImage->SetRect( eWindow::WIDTH / 2 - r.w / 2, eWindow::HEIGHT / 2 - r.h / 2 );
    r = m_CurImage->GetRect();
    m_OkButton = make_shared<Button>( r.x + r.w / 2 - 30, r.y + r.h - 60, 
        make_shared<Image>( Game.FindImage( "buttonTick1" ) ), 
        nullptr, nullptr,
        make_shared<Image>( Game.FindImage( "buttonTick2" ) ), NextScreen );
    m_OkButton->Initialize();
}

void EndScreen::Reset()
{
    if( !m_Player || !m_Player[0] || !m_Player[1] )
        return;
    auto& Game = GameManager::GetSingleton();
    m_Images.clear();
    SDL_Rect r = m_CurImage->GetRect();
    ImagePtr text = make_shared<Image>( ANDLSO, "Koniec gry!", 20, GetRGBA( eColor::LIGHT_GOLD ) );
    text->SetRect( r.x + r.w / 2 - text->GetRect().w / 2, r.y + 205 );
    r = text->GetRect();
    m_Images.push_back( text );
    text = make_shared<Image>( ANDLSO, "Zwyciezyl gracz ", 20, GetRGBA( eColor::LIGHT_GOLD ) );
    text->SetRect( r.x + r.w / 2 - text->GetRect().w / 2, r.y + r.h );
    m_Images.push_back( text );
    text = make_shared<Image>( ANDLSO, "Straty: ", 15, GetRGBA( eColor::LIGHT_GOLD ) );
    text->SetRect( eWindow::WIDTH / 2 - text->GetRect().w / 2, m_CurImage->GetRect().y + 300 );
    m_Images.push_back( text );
    text = make_shared<Image>( ANDLSO, "Straty: ", 15, GetRGBA( eColor::LIGHT_GOLD ) );
    text->SetRect( eWindow::WIDTH / 2 - text->GetRect().w / 2, m_CurImage->GetRect().y + 397 );
    m_Images.push_back( text );

    ImagePtr playerName;
    if( m_Player[0]->IsDefeated() )
        playerName = m_Player[1]->GetName();
    else
        playerName = m_Player[0]->GetName();
    r = m_Images[1]->GetRect();
    text = make_shared<Image>( ANDLSO, playerName->GetTexName(), 20, GetRGBA( eColor::LIGHT_GOLD ) );
    r.x -= text->GetRect().w / 2;
    m_Images[1]->SetRect( r.x, r.y );
    text->SetRect( r.x + r.w, r.y );
    m_Images.push_back( text );
    r = m_CurImage->GetRect();
    m_Player[0]->SetNameImagePos( r.x + 158, r.y + 44 );
    m_Player[1]->SetNameImagePos( r.x + 316, r.y + 44 );
    m_Images.push_back( m_Player[0]->GetName() );
    m_Images.push_back( m_Player[1]->GetName() );

    auto hero1 = m_Player[0]->GetHero()->GetHeroDescr();
    hero1.SetAvatarImagePos( r.x + 26, r.y + 38 );
    m_Images.push_back( make_shared<Image>( hero1.GetAvatarImage() ) );
    SDL_Rect r2 = hero1.GetNameImage().GetRect();
    r2.w *= (int)1.2;
    r2.h *= (int)1.2;
    r2.x = r.x + 60 - r2.w / 2;
    r2.y = r.y + 122 - r2.h / 2;
    m_Images.push_back( make_shared<Image>( hero1.GetNameImage() ) );
    m_Images.back()->SetRect( r2 );

    auto hero2 = m_Player[1]->GetHero()->GetHeroDescr();
    hero2.SetAvatarImagePos( r.x + 397, r.y + 38 );
    m_Images.push_back( make_shared<Image>( hero2.GetAvatarImage() ) );
    r2 = hero2.GetNameImage().GetRect();
    r2.w *= (int)1.2;
    r2.h *= (int)1.2;
    r2.x = r.x + 410 - r2.w / 2;
    r2.y = r.y + 122 - r2.h / 2;
    m_Images.push_back( make_shared<Image>( hero2.GetNameImage() ) );
    m_Images.back()->SetRect( r2 );

    r = hero1.GetAvatarImage().GetRect();
    auto castle1 = m_Player[0]->GetCastle();
    castle1.SetAvatarImagePos( r.x, r.y + r.h );
    m_Images.push_back( make_shared<Image>( castle1.GetAvatarImage() ) );
    r = hero2.GetAvatarImage().GetRect();
    auto castle2 = m_Player[1]->GetCastle();
    castle2.SetAvatarImagePos( r.x, r.y + r.h );
    m_Images.push_back( make_shared<Image>( castle2.GetAvatarImage() ) );

    SetStatistics( 0 );
    SetStatistics( 1 );
}

void EndScreen::Actualize()
{
    SDL_Event e = GameManager::GetSingleton().GetEvent();
    if( Quit( e ) )
    {
        m_IsActive = false;
        return;
    }
    m_OkButton->Actualize();
    AudioManager::GetSingleton().PlayTrack( m_SoundTrackName );
}

void EndScreen::Render()
{
    RenderPrevScreenAsBackground();
    m_CurImage->RenderTexture();
    for( auto& img : m_Images )
        img->RenderTexture();
    m_OkButton->GetImage()->RenderTexture();
}

void EndScreen::SetPlayers( Player* players[2] )
{
    if( players && players[0] && players[1] )
    {
        m_Player[0] = players[0];
        m_Player[1] = players[1];
    }
}

void EndScreen::SetStatistics( int playerIndex )
{
    if( !m_Player || !m_Player[playerIndex] )
        return;
    ImagePtr text;
    SDL_Rect p = m_CurImage->GetRect();
    p.x += 80;
    p.y += 360 + playerIndex * 97;
    int offset = 30;
    text = make_shared<Image>( ANDLSO, m_Player[playerIndex]->GetName()->GetTexName(), 15, GetRGBA( eColor::LIGHT_GOLD ) );
    SDL_Rect pos = m_Images[2+playerIndex]->GetRect();
    pos.x -= text->GetRect().w / 2;
    m_Images[2+playerIndex]->SetRect( pos.x, pos.y );
    text->SetRect( pos.x + pos.w, pos.y );
    m_Images.push_back( text );
    if( OptionsScreen* optionScreen = dynamic_cast<OptionsScreen*>( GameManager::GetSingleton().FindScreen( "OptionScreen" ) ) )
    {
        if( auto castle = optionScreen->GetCastle( m_Player[playerIndex]->GetCastle().GetName() ) )
        {
            vector<UnitDescr> unitDescr = castle->GetUnitDescrList();
            vector<UnitPtr> units = m_Player[playerIndex]->GetAllUnits();
            for( auto descr : unitDescr )
            {
                for( auto unit : units )
                {
                    if( unit && unit->GetName().compare( descr.name ) == 0 )
                    {
                        m_Images.push_back( make_shared<Image>( unit->GetAvatar() ) );
                        SDL_Rect r = m_Images.back()->GetRect();
                        r.w /= 2;
                        r.h /= 2;
                        r.x = p.x;
                        r.y = p.y - r.h;
                        m_Images.back()->SetRect( r );
                        int number = descr.unitStats.quantity - unit->GetQuantity();
                        char temp[10];
                        sprintf( temp , "%d", number );
                        text = make_shared<Image>( ANDLSO, temp, 15, GetRGBA( eColor::LIGHT_GOLD ) );
                        text->SetRect( p.x + r.w / 2 - text->GetRect().w / 2, p.y );
                        m_Images.push_back( text );
                        p.x += text->GetRect().w + offset;
                    }
                }
            }
        }
    }
}

void EndScreen::RenderPrevScreenAsBackground()
{
    auto& Game = GameManager::GetSingleton();
    if( m_Previous )
        m_Previous->Render();
}