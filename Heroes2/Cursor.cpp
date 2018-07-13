#include "Cursor.h"
#include "Texture.h"
#include "GameManager.h"
#include "BattleScreen.h"
#include "Grid.h"
#include "Unit.h"

Cursor::Cursor()
{
    m_MousePos.x = 0;
    m_MousePos.y = 0;
    m_MouseOn = nullptr;
    auto& Game = GameManager::GetSingleton();
    m_Cursors = 
    {
        { "sword", make_shared<Image>( Game.FindImage( "sword" ) ) },
        { "straightArrow", make_shared<Image>( Game.FindImage( "straightArrow" ) ) },
        { "brokenArrow", make_shared<Image>( Game.FindImage( "brokenArrow" ) ) },
        { "outOfRange", make_shared<Image>( Game.FindImage( "outOfRange" ) ) },
        { "walk", make_shared<Image>( Game.FindImage( "walk" ) ) },
        { "fly", make_shared<Image>( Game.FindImage( "fly" ) ) }, 
        { "info", make_shared<Image>( Game.FindImage( "info" ) ) },
        { "classic", make_shared<Image>( Game.FindImage( "classic" ) ) }
    };

    m_CurrentImage = m_Cursors["classic"];
}

VisibleObject * Cursor::GetMouseOn()
{
    if( !m_MouseOn )
        SetMouseOn();
    return m_MouseOn;
}

SDL_Point const & Cursor::GetMousePos()const
{
    return m_MousePos;
}

void Cursor::CheckMousePos()
{
    SDL_GetMouseState( &m_MousePos.x, &m_MousePos.y );
    SDL_Point texturePos;
    if( m_CurrentImage )
    {
        if( m_CurrentImage == m_Cursors["classic"] )
        {
            texturePos = m_MousePos;
        }
        else
        {
            texturePos.x = m_MousePos.x - m_CurrentImage->GetRect().w;
            texturePos.y = m_MousePos.y - m_CurrentImage->GetRect().h;
        }
        m_CurrentImage->SetRect( texturePos.x, texturePos.y );
    }
}

void Cursor::SetMouseOn()
{
    if( BattleScreen * pScreen = dynamic_cast<BattleScreen*>( GameManager::GetSingleton().GetScreen() ) )
    {
        for( int i = 0; i < GRID_ROW; i++ )
        {
            for( int j = 0; j < GRID_COLUMN; j++ )
            {
                Grid* pGrid = pScreen->GetGrid( i, j );
                if( pGrid && pGrid->IsMouseOn( m_MousePos ) )
                {
                    m_MouseOn = pGrid->GetUnit();
                    return;
                }
            }
        }
    }
    m_MouseOn = nullptr;
}

void Cursor::RenderCursor()
{
    if( m_CurrentImage )
        m_CurrentImage->RenderTexture( SDL_FLIP_NONE, m_Rotation );
}
    
void Cursor::SetTexture( STATE::Type s, TYPE::Name t, bool inRange )
{
    switch(s)
    {
    case STATE::MOVE:
        if( inRange )
        {
            if( t == TYPE::FLYING )
                m_CurrentImage = m_Cursors["fly"];
            else
                m_CurrentImage = m_Cursors["walk"];
        }
        else
            m_CurrentImage = m_Cursors["outOfRange"];
        break;
    case STATE::ACTIVE:
        m_CurrentImage = m_Cursors["info"];
        break;
    case STATE::ATTACK:
        if( inRange )
            m_CurrentImage = m_Cursors["sword"];
        else
            m_CurrentImage = m_Cursors["outOfRange"];
        break;
    case STATE::SHOOT:
        if( inRange )
            m_CurrentImage = m_Cursors["straightArrow"];
        else
            m_CurrentImage = m_Cursors["brokenArrow"];
        break;
    default:
        m_CurrentImage = m_Cursors["classic"];
        break;
    }
    SDL_Point texturePos;
    if( m_CurrentImage == m_Cursors["classic"] )
    {
        texturePos = m_MousePos;
    }
    else
    {
        texturePos.x = m_MousePos.x - m_CurrentImage->GetRect().w / 2;
        texturePos.y = m_MousePos.y - m_CurrentImage->GetRect().h / 2;
    }
    m_CurrentImage->SetRect( texturePos.x, texturePos.y );
}

void Cursor::SetRotation( double rotation )
{
    m_Rotation = rotation;
    SDL_Point texturePos = m_MousePos;
    SDL_Rect img = m_CurrentImage->GetRect();
    switch( (int)rotation )
    {
    case 45:                                //  ____
        texturePos.x -= img.w / 2;          //  \_)
        texturePos.y -= 2*img.h;            //   \ 
        break;                              
    case 90:                                //  ____
        texturePos.x += img.h / 2;          // |_)  
        texturePos.y -= img.w;              // |
        break;                              
    case 135:                               //   ____
        texturePos.x -= img.h / 2;          //  /_)  
        texturePos.y -= img.h * 2;          // / 
    case 180:
        texturePos.x += img.w;              //    __
        texturePos.y += img.h / 2;          // __(__)__
        break;
    case 225:
        texturePos.x += 2*img.h;            // \__
        texturePos.y += img.w / 2 + img.h;  //  \_)__
        break;                              
    case 270:
        texturePos.x -= img.h / 2;          //  |_
        texturePos.y += img.w;              //  |_)__
        break;                              
    case 315:
        texturePos.x -= img.w - img.h / 2;  //   /  
        texturePos.y += 2*img.h;            //  /)__
        break;                              
    default:
        texturePos.x -= img.w;
        texturePos.y -= img.h / 2;
        break;
    }
    m_CurrentImage->SetRect( texturePos.x, texturePos.y );
}

void Cursor::SetDefault()
{
    m_CurrentImage = m_Cursors["classic"];
    m_Rotation = 0.0f;
    m_CurrentImage->SetRect( m_MousePos.x, m_MousePos.y );
}

void Cursor::ResetMouseOn()
{
    m_MouseOn = nullptr;
}

void Cursor::BlockCursor()
{
    m_CurrentImage = nullptr;
}

bool Cursor::IsMouseOn( VisibleObject* pObj )
{
    if( !pObj || !pObj->GetImage() )
        return false;
    if( m_MouseOn )
        return m_MouseOn == pObj;
    vector<SDL_Rect> PositionsToCheck;
    PositionsToCheck.push_back( pObj->GetImage()->GetRect() );

    if( Unit * temp = dynamic_cast<Unit*>( pObj ) )
    {
        if( temp->GetState() == STATE::DIE )
            return false;
        else
        {
            BattleScreen* pScreen = dynamic_cast<BattleScreen*>( GameManager::GetSingleton().FindScreen( "battle" ) );
            vector<Grid*> Grids = pScreen->FindGrids( temp );
            if( Grids.size() )
            {
                PositionsToCheck.clear();
                for( auto pGrid : Grids )
                    PositionsToCheck.push_back( pGrid->GetImg().GetRect() );
            }
            else
                return false;
        }
    }
    for( auto& Pos : PositionsToCheck )
    {
        if( m_MousePos.x > Pos.x && ( m_MousePos.x < Pos.x + Pos.w ) && m_MousePos.y > Pos.y && ( m_MousePos.y < Pos.y + Pos.h ) )
        {
            m_MouseOn = pObj;
            return true;
        }
    }
    return false;
}

Uint32 Cursor::WasButtonPressed( Uint32 button )
{
    if( Uint32 state = SDL_GetMouseState( nullptr, nullptr ) )
    {
        switch( button )
        {
        case 0:
            return true;
        case SDL_BUTTON_LEFT:
            return state & SDL_BUTTON_LMASK;
        case SDL_BUTTON_RIGHT:
            return state & SDL_BUTTON_RMASK;
        case SDL_BUTTON_MIDDLE:
            return state & SDL_BUTTON_MMASK;
        }
    }
    return false;
}
