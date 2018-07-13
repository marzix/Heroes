#include "Player.h"
#include "GameManager.h"
#include "Unit.h"
#include "BattleScreen.h"
#include "OptionsScreen.h"

Player::Player( string n )
{
    m_Name = make_shared<Image>( TIMES, n, 15, GetRGBA( eColor::LIGHT_GOLD ) );
}

Player::Player( Player && rhs )
{
    m_Name = rhs.m_Name;
    m_Units = rhs.m_Units;
    m_Hero = rhs.m_Hero;
    m_Castle = rhs.m_Castle;
    m_FlagColor = rhs.m_FlagColor;
}

void Player::SetColor( PLAYER_COLOR::Color color )
{
    m_FlagColor = color;
}

void Player::SetHero( HeroPtr h )
{
    m_Hero = h;
}

void Player::SetName( string n )
{
    int x, y;
    if( n.compare( m_Name->GetTexName() ) != 0 )
    {
        SDL_Rect r = m_Name->GetRect();
        x = r.x + r.w / 2;
        y = r.y + r.h / 2;
        m_Name.reset();
        m_Name = make_shared<Image>( TIMES, n, 15, GetRGBA( eColor::LIGHT_GOLD ) );
        x -= m_Name->GetRect().w / 2;
        y -= m_Name->GetRect().h / 2;
        m_Name->SetRect( x, y );
    }
}

void Player::SetHeroBodyTex( Image body )
{
    if( m_Hero )
        m_Hero->SetBodyTex( body );
}

void Player::EndEdditing()
{
    string n = m_Name->GetTexName();
    if( n.size() > 1 )
        n.erase( n.end() - 1 );
    if( n.size() < 1 || n[0] == '_' )
    {
        n = "Player";
    }
    SetName( n );
}

bool Player::ChangeName()
{
    auto & Game = GameManager::GetSingleton();
    SDL_Event e = Game.GetEvent();
    if(e.type == SDL_KEYDOWN)
    {
        string n = m_Name->GetTexName();
        const char * c = SDL_GetKeyName(e.key.keysym.sym);
        if( e.key.keysym.sym == SDLK_BACKSPACE )
        {
            Game.ResetEvent();

            if( n.size() > 1 )
            {
                n.erase( n.end() - 2 );
                SetName( n );
            }
        }
        else if( e.key.keysym.sym == SDLK_RETURN )
        {
            EndEdditing();
            return false;
        }
        else if( (c[0] >= '0' && c[0] <= '9' ) || ( c[0] >= 'A' && c[0] <= 'Z' ) )
        {
            n.insert( n.size() - 1, c );
            SetName( n );
        }

    }
    return true;
}

void Player::SetCastle( Descriptor & rCastle )
{
    int x = m_Castle.GetAvatarImage().GetRect().x, y = m_Castle.GetAvatarImage().GetRect().y;
    m_Castle = rCastle;
    m_Castle.SetDescriptorPosition( x, y );
}

void Player::SetCastlePos( int x, int y )
{
    m_Castle.SetDescriptorPosition( x, y );
}

void Player::SetHeroDescr( Descriptor & rHero )
{
    if( m_Hero )
        m_Hero->SetHeroDescr( rHero );
}

void Player::SetHeroDescrPos( int x, int y )
{
    if( m_Hero )
        m_Hero->SetHeroDescrPos( x, y );
}

void Player::SetUnits( BattleScreen * pScreen )
{
    auto& Game = GameManager::GetSingleton();
    bool changeFlip = false;
    int column1 = m_PlayerSide == SIDE::LEFT ? 0 : GRID_COLUMN - 1;
    int column2 = m_PlayerSide == SIDE::LEFT ? 1 : GRID_COLUMN - 2;
    int position[] = { 0, 2, 4, 5, 6, 8, 10 };
    Image* tRect = Game.FindImage( "rectangle" );
    for( int i = 0; i < 7; i++ )
    {
        m_Units[i]->SetPlayer( m_PlayerSide );
        m_Units[i]->SetAnimations( Game.GetSet( m_Units[i]->GetName() ) );

        if( m_Units[i]->GetSize() == 1 )
            m_Units[i]->SetGrid( pScreen->GetGrid( position[i], column1 ) );
        else
            m_Units[i]->SetGrid( pScreen->GetGrid( position[i], column2 ), pScreen->GetGrid( position[i], column1 ) );

        m_Units[i]->SetQuantityInfoImage( make_shared<Image>( tRect ) );
        m_Units[i]->SetFlip( SIDE::NONE, false );
        pScreen->AddToObjectList( m_Units[i] );
        StatsInfoBoardPtr unit1InfoBoard = make_shared<StatsInfoBoard>( m_Units[i].get(), m_FlagColor );
        m_Units[i]->SetStatsBoard( unit1InfoBoard );
        pScreen->AddToObjectList( unit1InfoBoard );
        if( m_Units[i]->GetType() == TYPE::ARCHER )
            pScreen->AddToObjectList( m_Units[i]->GetShotObject() );
    }
}

void Player::SetSide( SIDE playerSide )
{
    m_PlayerSide = playerSide;
}

void Player::SetIsAI( bool isAI )
{
    m_IsAI = isAI;
    string name;
    if( isAI )
        name = "Komputer";
    else
    {
        name = "Player1";
        if( auto screen = dynamic_cast<OptionsScreen*>( GameManager::GetSingleton().GetScreen() ) )
        {
            if( auto player = screen->GetPlayer( 0 ) )
            {
                if( player == this )
                    player = screen->GetPlayer( 1 );
                if( player && player->GetName() && player->GetName()->GetTexName().compare( name ) == 0 )
                    name = "Player2";
            }
        }
    }
    SetName( name );
}

void Player::SetNameImagePos( int x, int y )
{
    m_Name->SetRect( x - m_Name->GetRect().w / 2, y - m_Name->GetRect().h / 2 );
}

void Player::AddUnit( UnitPtr& unit )
{
    m_Units.push_back( unit );
}

UnitPtr Player::GetUnit( unsigned int i )
{
    if( i < m_Units.size() )
        return m_Units[i];
    else
        return nullptr;
}

PLAYER_COLOR::Color Player::GetColor()const
{
    return m_FlagColor;
}

Descriptor & Player::GetCastle()
{
    return m_Castle;
}

HeroPtr Player::GetHero()
{
    return m_Hero;
}

ImagePtr Player::GetName()
{
    return m_Name;
}

vector<UnitPtr> & Player::GetAllUnits()
{
    return m_Units;
}

SIDE Player::GetSide()const
{
    return m_PlayerSide;
}

bool Player::IsAI()const
{
    return m_IsAI;
}

bool Player::IsDefeated()
{
    UnitPtr temp;
    for( auto unit: m_Units )
    {
        if( unit->GetState() != STATE::DIE || !unit->CurrentAnimationFinished() )
            return false;
    }
    return true;
}

void Player::ResetArmy()
{
    if( m_Units.size() != 0 )
        m_Units.clear();
}

void Player::AddToGameQueue()
{
    auto & Game = GameManager::GetSingleton();
    for( auto unit : m_Units )
    {
        if( unit && unit->IsActive() )
        {
            unit->Reset();
            Game.AddToGameQueue( unit );
        }
    }
}

void Player::RenderHero()
{
    if( m_Hero )
        m_Hero->Render();
}