#include "GameManager.h"
#include "OptionsScreen.h"
#include "Button.h"
#include "Texture.h"
#include "Castle.h"
#include "Player.h"
#include "Hero.h"
#include "Unit.h"
extern string times;
extern SDL_Color lightGold;

void NextScreen()
{
    auto & Game = GameManager::GetSingleton();
    if( auto currentScreen = Game.GetScreen() )
        Game.SetScreen( currentScreen->GetNext() );
    if( auto newScreen = Game.GetScreen() )
    {
        newScreen->Reset();
    }
}

void PreviousScreen()
{
    auto & Game = GameManager::GetSingleton();
    if( auto currentScreen = Game.GetScreen() )
        Game.SetScreen( currentScreen->GetPrev() );
}

void SwitchColor( PLAYER_COLOR::Color c )
{
    auto & Game = GameManager::GetSingleton();
    if( OptionsScreen* pScreen = dynamic_cast<OptionsScreen*>( Game.GetScreen() ) )
    {
        if( PlayerOptionPanel* panel = pScreen->FindOptionPanel( c ) )
            pScreen->SwitchPlayerColor( panel->GetPlayerIndex() ); 
    }
}


void ChangePlayerName( int player )
{
    if( OptionsScreen* pScreen = dynamic_cast<OptionsScreen*>( GameManager::GetSingleton().GetScreen() ) )
    {
        if( player == 1 && pScreen->GetPlayer( 1 )->IsAI() )
            return;
        pScreen->SetEnterName( player );
        pScreen->SetName( "_", player );
    }
}

void SwitchCastle( int index, bool reverseOrder )
{
    auto & Game = GameManager::GetSingleton();
    if( OptionsScreen* pScreen = dynamic_cast<OptionsScreen*>( Game.GetScreen() ) )
    {
        Castle* pCastle = nullptr;
        if( !reverseOrder ) 
        {
            pCastle = pScreen->FindNextCastleTex( pScreen->GetPlayer( index )->GetCastle().GetName() );
        }
        else
        {
            pCastle = pScreen->FindPrevCastleTex( pScreen->GetPlayer( index )->GetCastle().GetName() );
        }
        pScreen->GetPlayer( index )->SetCastle( pCastle->GetCastleDescr() );
        pScreen->GetPlayer( index )->SetHeroDescr( pCastle->GetHero( 0 ) );
        if( pScreen->GetPlayer( 0 )->GetCastle() == pScreen->GetPlayer( 1 )->GetCastle() )
        {
            auto hero1 = pScreen->GetPlayer( 0 )->GetHero();
            auto hero2 = pScreen->GetPlayer( 1 )->GetHero();
            if( hero1 && hero2 && hero1->GetName().compare( hero2->GetName() ) == 0 )
            {
                pScreen->GetPlayer( index )->SetHeroDescr( pCastle->GetHero( 1 ) );
            }
        }
    }
}

void SwitchHero( int playerIndex, bool reverseOrder )
{
    auto & Game = GameManager::GetSingleton();
    if( OptionsScreen* pScreen = dynamic_cast<OptionsScreen*>( Game.GetScreen() ) )
    {
        if( Descriptor* pHero = pScreen->FindNextHero( playerIndex, reverseOrder ) )
            pScreen->GetPlayer( playerIndex )->SetHeroDescr( *pHero );
    }
}

void ChangeText( ImagePtr & oldText, string newText, int time )
{
    auto & Game = GameManager::GetSingleton();
    Game.SetRoundTime( time );
    if( oldText->GetTexName() != newText )
    {
        SDL_Rect r = oldText->GetRect();
        oldText->SetTexture( TIMES, newText, 15, GetRGBA( eColor::LIGHT_GOLD ) );
        oldText->SetRect( r.x, r.y ); 
    };
}

void ChangeIsAI()
{
    auto & Game = GameManager::GetSingleton();
    if( OptionsScreen* pScreen = dynamic_cast<OptionsScreen*>( Game.GetScreen() ) )
    {
        if( Player* player = pScreen->GetPlayer( 1 ) )
            player->SetIsAI( !player->IsAI() );
    }
}

void SetUnitWaiting()
{
    auto & Game = GameManager::GetSingleton();
    if( auto unit = Game.GetActiveUnit() )
    {
        if( !Game.IsPlayerAI( unit->GetPlayer() ) )
            Game.SetActiveUnitWait();
    }
}

void SetUnitDefend()
{
    auto & Game = GameManager::GetSingleton();
    if( auto unit = Game.GetActiveUnit() )
    {
        if( !Game.IsPlayerAI( unit->GetPlayer() ) )
            Game.SetActiveUnitInDefendMode();
    }
}