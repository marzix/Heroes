#include "Unit.h"
#include "GameManager.h"
#include "AnimationSet.h"
#include "Animation.h"
#include "BattleScreen.h"
#include "Node.h"
#include <math.h>

#pragma warning(disable : 4996)

Unit::Unit( string _name, int _attack, int _defense, int _life, int _speed, TYPE::Name _type, int _size, int _number )
: m_Size( _size )
, m_Type( _type )
, m_AnimationController( AnimationController( this ) )
{
    m_Name = _name;
    m_Stats.speed = _speed;
    m_Stats.lifeRemaining = 100;
    m_Stats.attack = _attack;
    m_Stats.defense = _defense;
    m_Stats.lifePerUnit = _life;
    m_Stats.lifeRemaining = _life;
    m_Stats.quantity = _number;
    m_IsActive = true;
}

Unit::Unit( string name, TYPE::Name type, uint size, Statistics stats )
: m_Size( size )
, m_Type( type )
, m_Stats( stats )
, m_AnimationController( AnimationController( this ) )
{
    m_Name = name;
    m_IsActive = true;
    if( m_Type == TYPE::ARCHER )
    {
        m_Shot = GameManager::GetSingleton().FindShotObject( m_Name );        
    }
}

Unit::~Unit()
{
}

void Unit::Initialize()
{
    SetImage();
    m_AnimationController.Initialize();
    auto anim = GetAnimation( STATE::STAND );
    m_StandingUnitImg = anim.m_Frames[0];
    if( m_Shot )
        m_Shot->SetOwner( this );
}

void Unit::Actualize()
{
    auto& Game = GameManager::GetSingleton();
    STATE::Type prevState = m_State;
    m_AnimationController.Actualize();
    if( m_State == STATE::DIE && m_AnimationController.CurAnimationFinished() )
    {
        if( m_CurGrid )
            Deactivate();
    }
    else if( ( m_State == STATE::ATTACK || m_State == STATE::SHOOT ) && m_AttackedUnit )
    {
        if( prevState != m_State )
            SetFlip( GetEnemyDirection(), true );
        if( m_State == STATE::ATTACK )
        {
            if( m_AttackedUnit && m_Flip == m_AttackedUnit->GetFlip() )
                m_AnimationController.Pause( GetEnemyDirection() );
            else
                m_AnimationController.Pause( SIDE::NONE );
        }
        if( m_AnimationController.IsPaused() &&  m_State == STATE::ATTACK )
        {
            if( m_AttackedUnit->GetState() == STATE::TURN )
            {
                if( m_AttackedUnit->IsTurned() )
                {
                    SIDE pauseDir = m_Flip == SDL_FLIP_NONE ? LEFT : RIGHT;
                    m_AttackedUnit->PauseAnimation( pauseDir );
                }
            }
            else
            {
                m_AttackedUnit->SetHit( this );
                m_AttackedUnit->SetAtteckedUnit( this );
            }
        }
        else if( PerformAttack() )
        {
            m_AttackedUnit = nullptr;
        }
    }
    else if( m_State == STATE::GET_HIT || m_State == STATE::DEFEND )
    {
        m_IsHit = SIDE::NONE;
    }
    else if( m_State == STATE::ACTIVE || m_State == STATE::STAND || m_State == STATE::IDLE )
    {
        SetFlip( NONE, true );
        if( m_State == STATE::STAND || m_State == STATE::ACTIVE )
        {
            SetQuantityInfoImage();
            ModulateAlpha();
        }
        if( m_State == STATE::ACTIVE && Game.IsPlayerAI( m_Player ) )
            Game.CallAI( this );
        else
            MouseEvent();
    }
    else if( m_Route.size() && ( m_State == STATE::MOVE || m_State == STATE::START_MOVE || m_State == STATE::END_MOVE ) )
    {
        SetFlip( GetMoveDirection( m_Route.back() ), false );
        Move();
    }
    SetPosition( m_Position.x, m_Position.y );
}

void Unit::Render()
{
    SetPosition( m_Position.x, m_Position.y );

    if( m_State == STATE::ACTIVE )
    {
        m_StandingUnitImg->SetRect( m_CurImage->GetRect() );
        m_StandingUnitImg->RenderTexture( 255, m_Flip );
        m_CurImage->RenderTexture( m_ActiveUnitAlpha, m_Flip );
    }
    else
        m_CurImage->RenderTexture( m_Flip );
    if( m_State == STATE::STAND || m_State == STATE::IDLE || m_State == STATE::ACTIVE )
    {
        m_Stats.quantityInfoField->RenderTexture();
        m_Stats.numberImage->RenderTexture();
    }
}

void Unit::Reset()
{
    m_DefenseMode = false;
    m_Waiting = false;
    m_CanHitBack = true;
    if( !IsIdle() )
        m_AnimationController.DisableCurrentAnimation();
}

void Unit::MouseEvent()
{
    auto & Game = GameManager::GetSingleton();
    const SDL_Event e = Game.GetEvent();
    SDL_Point mousePos = Game.GetMousePos();
    BattleScreen * pScreen = dynamic_cast<BattleScreen *>( Game.GetScreen() );
    Grid* pointedGrid = pScreen->FindGrid( mousePos.x, mousePos.y );
    Grid* destinationGrid = nullptr;
    Unit* pointedUnit = nullptr;
    STATE::Type state = m_State;
    bool inRange = false;
    if( pointedGrid )
        pointedUnit = pointedGrid->GetUnit();
    if( m_StatsBoard )
    {
        if( pointedUnit == this )
            m_StatsBoard->SetIsActive( true );
        else
            m_StatsBoard->SetIsActive( false );
    }
    if( !pointedGrid )
        return;
    if( m_State == STATE::ACTIVE )
    {
        if( pointedGrid->GetOccupied() )
        {
            if( pointedUnit && pointedUnit->GetPlayer() == m_Player )
                Game.SetCursorTexture( STATE::ACTIVE, m_Type, false );
            else
            {
                state = STATE::ATTACK;
                bool hasNeighbour = false;
                vector<Grid*> neighbours = pScreen->GetAllNeighbours( pointedUnit );
                inRange = IsInAttackRange( neighbours, false );
                if( m_Type == TYPE::ARCHER )
                {
                    vector<Grid*> archersNeighbours = pScreen->GetAllNeighbours( this );
                    if( IsInAttackRange( archersNeighbours, true ) )
                    {
                        state = STATE::SHOOT;
                        inRange = IsInGoodShotRange( pointedUnit );
                    }
                    else hasNeighbour = true;
                }
                Game.SetCursorTexture( state, m_Type, inRange );
                if( inRange && ( m_Type != TYPE::ARCHER || hasNeighbour ) )
                    destinationGrid = FindAttackPosition( pointedGrid->GetUnit()->GetGrid(), neighbours );
            }
        }
        else
            Game.SetCursorTexture( STATE::MOVE, m_Type, pointedGrid->InRange() );
        if( e.button.button == SDL_BUTTON_LEFT && e.type == SDL_MOUSEBUTTONDOWN )
        {
            if( state != STATE::SHOOT )
            {
                if( NodePtr dest = InRange( destinationGrid ) )
                {
                    SetRoute( dest );
                    m_AnimationController.DisableCurrentAnimation();
                }
            }
            if( inRange || state == STATE::SHOOT )
            {
                m_AnimationController.DisableCurrentAnimation();
                m_AttackedUnit = pointedUnit;
            }
        }
    }
}

int Unit::GetHeightForSorting()const
{
    SDL_Point p;
    p.y = 0;
    if( m_CurGrid[0] )
        p = m_CurGrid[0]->GetMiddle();
    if( m_State == STATE::ATTACK )
        p.y += 5;
    return p.y;
}

Grid* CorrectChoice( Grid* chosenGrid, float& rotation, vector<Grid*> neighbours, Unit* unit )
{
    Grid* newChoice = chosenGrid;
    if( !chosenGrid || !unit )
        return chosenGrid;
    if( unit->GetSize() == 1 )
    {
        if( neighbours.size() == 6 )
        {
            if( rotation == 90 )
                rotation = chosenGrid == neighbours[0] ? 45.0f : 135.0f;
            else if( rotation == 270 )
                rotation = chosenGrid == neighbours[4] ? 315.0f : 225.0f;
            return chosenGrid;
        }
    }
    else if( unit->GetSize() == 2 && rotation != 180.0f && rotation != 0.0f )
    {
        SIDE dir = unit->GetMoveDirection( chosenGrid );
        auto & Game = GameManager::GetSingleton();
        if( BattleScreen * pScreen = dynamic_cast<BattleScreen *>( Game.GetScreen() ) )
        {
            Grid** grids = unit->GetGrid();
            if( unit->GetPlayer() == RIGHT )
            {
                if( rotation == 45.0f )
                    newChoice = pScreen->GetPreviousNextGrid( neighbours[0], true );
                else if( rotation == 315.0f && neighbours.size() == 6 )
                    newChoice = pScreen->GetPreviousNextGrid( neighbours[4], true );
            }
            else
            {
                if( rotation == 135.0f )
                    newChoice = pScreen->GetPreviousNextGrid( neighbours[1], false );
                else if( rotation == 225.0f && neighbours.size() == 6 )
                    newChoice = pScreen->GetPreviousNextGrid( neighbours[3], false );
            }
            
            if( neighbours.size() == 6 )
            {
                if( rotation == 90.0f )
                    newChoice = unit->GetPlayer() == LEFT ? neighbours[1] : neighbours[0];
                else if( rotation == 270.0f )
                    newChoice = unit->GetPlayer() == LEFT ? neighbours[3] : neighbours[4];
            }
        }
    }
    if( newChoice && newChoice->InRange() && ( !newChoice->GetOccupied() || newChoice->GetUnit() == unit ) )
        return newChoice;
    else
        return chosenGrid;
}

Grid* Unit::FindAttackPosition( Grid** enemyPos, vector<Grid*> neighbours )
{
    if( !enemyPos || !enemyPos[0] )
        return nullptr;
    auto & Game = GameManager::GetSingleton();
    SDL_Point mousePos = Game.GetMousePos();
    SDL_Rect grid;
    if( enemyPos[1] && enemyPos[1]->GetCol() < enemyPos[0]->GetCol() )
        grid = enemyPos[1]->GetRect();
    else
        grid = enemyPos[0]->GetRect();
    int width = grid.w;
    if( enemyPos[0]->GetUnit()->GetSize() == 2 )
        width += grid.w;
    float rotation = 0.0f;

    if( mousePos.x <= grid.x + width / 3 )
    {
        mousePos.x -= grid.w / 2;
        if( mousePos.y <= grid.y + grid.h / 3 )
        {
            rotation = 45.0f;
            mousePos.y -= grid.h / 2;
        }
        else if( mousePos.y > grid.y + 2*grid.h / 3 )
        {
            rotation = 315.0f;
            mousePos.y += grid.h / 2;
        }
        else
            mousePos.x -= 10;
    }
    else if( mousePos.x > grid.x + 2*width / 3 )
    {
        mousePos.x += grid.w / 2;
        if( mousePos.y <= grid.y + grid.h / 3 )
        {
            rotation = 135.0f;
            mousePos.y -= grid.h / 2;
        }
        else if( mousePos.y > grid.y + 2*grid.h / 3 )
        {
            rotation = 225.0f;
            mousePos.y += grid.h / 2;
        }
        else
        {
            rotation = 180.0f;
            mousePos.x += 10;
        }
    }
    else
    {
        if( mousePos.y <= grid.y + grid.h / 2 )
        {
            rotation = 90.0f;
            mousePos.y -= grid.h / 2;
        }
        else
        {
            rotation = 270.0f;
            mousePos.y += grid.h / 2;
        }
    }
    Grid* attackPosGrid = FindClosestFreeGrid( mousePos, enemyPos[0]->GetUnit(), rotation, neighbours );
    attackPosGrid = CorrectChoice( attackPosGrid, rotation, neighbours, this );
    if( !attackPosGrid )
        return nullptr;
    Game.SetCursorRotation( rotation );
    return attackPosGrid;
}

Grid* Unit::FindClosestFreeGrid( SDL_Point mousePos, Unit* enemy, float& rotation, vector<Grid*> n )
{
    if( !enemy )
        return nullptr;
    auto & Game = GameManager::GetSingleton();
    bool found = false;
    vector<Grid*> neighbours = n;
    if( neighbours.size() )
    {
        for( uint i = 0; i < neighbours.size(); i++ )
        {
            if( !neighbours[i] )
                continue;
            SDL_Rect r = neighbours[i]->GetRect();
            if( r.x <= mousePos.x && r.x + r.w > mousePos.x && r.y <= mousePos.y && r.y + r.h > mousePos.y )
            {
                if( neighbours[i]->InRange() && ( !neighbours[i]->GetOccupied() || neighbours[i]->GetUnit() == this ) )
                    return neighbours[i];
                else
                {
                    rotate( neighbours.begin(), neighbours.begin() + i, neighbours.end() );
                    found = true;
                    break;
                }
            }
        }
        if( !found )
        {
            if( !neighbours[0] )
            {
                rotation = 0;
            }
            else if( !neighbours[3] )
            {
                rotation = 180;
                rotate( neighbours.begin(), neighbours.begin() + 3, neighbours.end() );
            }
        }
        int endPoint = neighbours.size() / 2;
        float angleI = rotation, angleJ = rotation; 
        int i = 1, j = neighbours.size() - 1;
        for( ; i <= endPoint && j > endPoint; i++, j-- )
        {
            angleI += 45.0f;
            if( angleI >= 360.0f )
                angleI -= 360.0f;
            if( enemy->GetSize() == 1 && ( angleI == 90 || angleI == 270 ) )
                angleI += 45.0f;

            angleJ -= 45.0f;
            if( angleJ < 0.0f )
                angleJ += 360.0f;
            if( enemy->GetSize() == 1 && ( angleJ == 90 || angleJ == 270 ) )
                angleJ -= 45.0f;

            if( !neighbours[i] )
                continue;
            if( neighbours[i]->InRange() && ( !neighbours[i]->GetOccupied() || neighbours[i]->GetUnit() == this ) )
            {
                rotation = angleI;
                return neighbours[i];
            }
            if( !neighbours[j] )
                continue;
            if( neighbours[j]->InRange() && ( !neighbours[j]->GetOccupied() || neighbours[i]->GetUnit() == this ) )
            {
                rotation = angleJ;
                return neighbours[j];
            }
        }
        if( neighbours[i] && neighbours[i]->InRange() && ( !neighbours[i]->GetOccupied() || neighbours[i]->GetUnit() == this ) )
            return neighbours[i];
        else if( neighbours[j] && neighbours[j]->InRange() && ( !neighbours[j]->GetOccupied() || neighbours[i]->GetUnit() == this ) )
            return neighbours[j];
    }
    for( uint k = 0; k < neighbours.size(); k++ )
    {
        if( neighbours[k] && neighbours[k]->InRange() && ( !neighbours[k]->GetOccupied() || neighbours[k]->GetUnit() == this ) )
            return neighbours[k];
    }
    return nullptr;
}

void Unit::Move()
{
    if( m_Route.size() <= 0 )
        return;

    auto & Game = GameManager::GetSingleton();
    if( MoveToNextGrid( m_Route.back() ) )
    {
        if( m_Route.size() == 1 && m_Size == 2 )
        {
            //jezeli sa rowne to second = nullptr, jezeli second jest wlasciwa to zwroc, w przeciwnym razie znajdz druga
            Grid* second = Game.GetSecondUnitGrid( m_Route.back(), m_CurGrid[0] );
            SetGrid( m_Route[0], second );
        }
        else
            SetGrid( m_Route.back(), m_CurGrid[0], m_Route.size() == 1 );

        m_Route.erase( m_Route.end() - 1 );
    }
    if( m_Route.size() <= 0 )
        m_AnimationController.DisableCurrentAnimation();
}

void Unit::SetState( STATE::Type state )
{
    m_State = state;
}

bool Unit::MoveToNextGrid( Grid* g )
{
    if( !g )
        return false;
    SDL_Point r = g->GetMiddle();
    SDL_Point currentPoint, nextPoint;
    currentPoint.x = m_CurGrid[0] ? m_CurGrid[0]->GetMiddle().x : 0;
    currentPoint.y = m_CurGrid[0] ? m_CurGrid[0]->GetMiddle().y : 0;
    nextPoint.x = r.x;
    nextPoint.y = r.y;
    if( m_Size > 1 )
    {
        if( m_Flip == SDL_FLIP_NONE )
            r.x -= g->GetWidth() / 4;
        else
            r.x += g->GetWidth() / 4;
    }

    if( ( m_Position.x >= r.x && currentPoint.x <= nextPoint.x || m_Position.x <= r.x && currentPoint.x >= nextPoint.x ) &&
        ( m_Position.y >= r.y && currentPoint.y <= nextPoint.y || m_Position.y <= r.y && currentPoint.y >= nextPoint.y ) )
        return true;

    int kx, ky;
    kx = ky = m_Stats.speed / 4;
    if( kx == 0 )
        kx = ky = 1;
    if( m_Position.x > r.x )
    {
        kx = -kx;
    }
    if( m_Position.y > r.y )
    {
        ky = -ky;
    }
    if( m_Position.y != r.y )
    {
        m_Position.y += ky;
        if( m_Position.y != r.y )
            m_Position.x += int( ceil(double( r.x - m_Position.x )*ky / ( r.y - m_Position.y )) );
    }
    else if( m_Position.x != r.x )
        m_Position.x += kx;
    
    return false;
}

void Unit::RotateGrids()
{
    Grid* temp = m_CurGrid[0];
    m_CurGrid[0] = m_CurGrid[1];
    m_CurGrid[1] = temp;
}

void Unit::PauseAnimation( SIDE pauseInDirection )
{
    m_AnimationController.Pause( pauseInDirection );
}

STATE::Type Unit::GetState()const
{
    return m_State;
}

int Unit::GetSize()const
{
    return m_Size;
}

uint Unit::GetAttack()const
{
    return m_Stats.attack;
}

uint Unit::GetDefense()const
{
    return m_Stats.defense;
}

uint Unit::GetLifePerUnit()const
{
    return m_Stats.lifePerUnit;
}

Grid ** Unit::GetGrid()
{
    return m_CurGrid;
}

int Unit::GetLifeRemaining()const
{
    return m_Stats.lifeRemaining;
}

uint Unit::GetQuantity()const
{
    return m_Stats.quantity;
}

const ImagePtr Unit::GetQuantityImg()const
{
    return m_Stats.numberImage;
}

Unit* Unit::GetAttackedUnit()
{
    return m_AttackedUnit;
}

uint Unit::GetRouteSize()const
{
    return m_Route.size();
}

Animation Unit::GetAnimation( STATE::Type state )const
{
    string name =  STATE::ToString( state );
    if( state == STATE::ATTACK || state == STATE::SHOOT )
    {
        if( m_AttackedUnit )
        {
            SDL_Point p = m_AttackedUnit->GetPosition();
            if( p.y < m_Position.y )
                name += "_UP";
            else if( p.y > m_Position.y )
                name += "_DOWN";
        }
    }
    return *m_Animations->Find( name );
}

bool Unit::CurrentAnimationFinished()
{
    auto anim = m_AnimationController.GetCurrentAnimation();
    return anim.m_CurrentFrame >= anim.m_Frames.size() - 1;
}

TYPE::Name Unit::GetType()const
{
    return m_Type;
}

SIDE Unit::GetMoveDirection( Grid* grid )const
{
    if( !grid )
    {
        if( m_Route.size() )
            grid = m_Route.back();
    }
    if( !grid || !m_CurGrid )
        return NONE;

    if( grid->GetMiddle().x <= m_CurGrid[0]->GetMiddle().x )
        return LEFT;
    if( grid->GetMiddle().x > m_CurGrid[0]->GetMiddle().x )
        return RIGHT;

    return NONE;
}

SIDE Unit::GetEnemyDirection( Unit* enemy )const
{
    if( !m_AttackedUnit && !enemy )
        return NONE;
    auto attackedUnitGrid = enemy ? enemy->GetGrid() : m_AttackedUnit->GetGrid();
    if( attackedUnitGrid && m_CurGrid )
    {
        Grid* enemyGrid = attackedUnitGrid[0];
        Grid* myGrid = m_CurGrid[0];
        if( attackedUnitGrid[1] && attackedUnitGrid[1]->GetCol() < enemyGrid->GetCol() )
            enemyGrid = attackedUnitGrid[1];
        if( m_CurGrid[1] && m_CurGrid[1]->GetCol() < myGrid->GetCol() )
            myGrid = m_CurGrid[1];
        if( enemyGrid->GetRect().x < myGrid->GetRect().x )
        {
            return LEFT;
        }
        else if( enemyGrid->GetRect().x > myGrid->GetRect().x )
        {
            return RIGHT;
        }
    }
    return NONE;
}

SIDE Unit::GetPlayer()const
{
    return m_Player;
}

uint Unit::GetSpeed()const
{
    return m_Stats.speed;
}

bool Unit::GetDefendMode()const
{
    return m_DefenseMode;
}

bool Unit::GetWaiting()const
{
    return m_Waiting;
}

ShotObjectPtr Unit::GetShotObject()
{
    return m_Shot;
}

const Image& Unit::GetAvatar()
{
    return m_StatsBoard->GetAvatar();
}

SIDE Unit::IsHit()const
{
    return m_IsHit;
}

bool Unit::CanHitBack()const
{
    return m_CanHitBack;
}

bool Unit::IsTurned()
{
    auto anim = m_AnimationController.GetCurrentAnimation();
    {
        if( anim.m_CurrentFrame == anim.m_Frames.size() - 1 )
        {
            return true;
        }
    }
    return false;
}

void Unit::SetIsActive( bool val )
{
    m_IsActive = val;
}

void Unit::SetAtteckedUnit( Unit* attacker )
{
    m_AttackedUnit = attacker;
}

void Unit::SetImage()
{
    Animation tmp = GetAnimation( m_State );
    m_CurImage = tmp.m_Frames[0];
}

void Unit::SetAnimations( AnimationSetPtr set )
{
    m_Animations = set;
    AnimationPtr tmp = set->Find( "STAND" );
    m_Dimentions.x = tmp->m_Frames[0]->GetRect().w;
    m_Dimentions.y = tmp->m_Frames[0]->GetRect().h;
    m_CurImage = tmp->m_Frames[0];
}

void Unit::SetPosition( int x, int y )
{
    m_Position.x = x;
    m_Position.y = y;
    if( m_Size == 1 )
    {
        if( m_Flip == SDL_FLIP_NONE )
            x += 30;
        else
            x -= 30;
    }
    m_CurImage->SetRect( x, y - m_Dimentions.y/2, m_Flip );
}

void Unit::SetPosition()
{
    SetPosition( m_Position.x, m_Position.y );
}

void Unit::SetQuantityInfoImage()
{
    SDL_Rect r = m_CurGrid[0]->GetRect();
    auto & Game = GameManager::GetSingleton();
    Unit* unit = nullptr;
    if( BattleScreen * pScreen = dynamic_cast<BattleScreen *>( Game.GetScreen() ) )
    {
        if( Grid* g = pScreen->GetPreviousNextGrid( m_CurGrid[0], m_Flip == SDL_FLIP_HORIZONTAL ) )
        {
            if( unit = g->GetUnit() )
            {
                if( unit == this )
                    unit = nullptr;
            }
        }
    }
    if( unit )
    {
        m_Stats.quantityInfoField->SetRect( r.x + r.w / 2, r.y + r.h - 5 );
    }
    else if( m_Flip == SDL_FLIP_HORIZONTAL )
    {
        m_Stats.quantityInfoField->SetRect( r.x, r.y + r.h - 5 );
    }
    else
    {
        m_Stats.quantityInfoField->SetRect( r.x + r.w, r.y + r.h - 5 ); 
    }

    char temp[5];
    auto ToStr = [&]( int val )->const char*
    {
        sprintf( temp , "%d" , val );
        return temp;
    };

    if( m_Stats.numberImage == nullptr || ( m_Stats.quantity != atoi( m_Stats.numberImage->GetTexName().c_str() ) ) )
    {
        m_Stats.numberImage = make_shared<Image>( TIMES, ToStr( m_Stats.quantity ), 10, GetRGBA( eColor::WHITE ) );
    }
    
    r =  m_Stats.quantityInfoField->GetRect();
    m_Stats.numberImage->SetRect( r.x + r.w / 2 - m_Stats.numberImage->GetRect().w / 2, r.y + r.h / 2 - m_Stats.numberImage->GetRect().h / 2 + 1 );
}

void Unit::SetStatsBoard( StatsInfoBoardPtr board )
{
    m_StatsBoard = board;
}

void Unit::SetQuantityInfoImage( ImagePtr rect )
{
    if( !m_Stats.quantityInfoField || !m_Stats.quantityInfoField->GetTex() )
    {
        m_Stats.quantityInfoField = rect;
        m_Stats.quantityInfoField->SetTextureCenter();
    }
    SetQuantityInfoImage();
}

void Unit::SetRoute( NodePtr dest )
{
    while( dest && dest->pGrid != m_CurGrid[0] && dest->pGrid != m_CurGrid[1] )
    {
        m_Route.push_back( dest->pGrid );
        dest = dest->pPrev;
    }
}

void Unit::SetGrid( Grid * first, Grid * second, bool setUnit )
{
    if( !first )
        return;
    if( m_CurGrid[0] && m_CurGrid[0]->GetUnit() == this )
        m_CurGrid[0]->SetOccupied( false );
    m_CurGrid[0] = first;
    if( setUnit )
        m_CurGrid[0]->SetUnit( this );
    if( m_CurGrid[1] && m_CurGrid[1]->GetUnit() == this )
        m_CurGrid[1]->SetOccupied( false );
    if( m_Size > 1 && second && second != first )
    {
        m_CurGrid[1] = second;
        if( setUnit )
            m_CurGrid[1]->SetUnit( this );
    }
    if(!setUnit)
        return;
    SDL_Point p = m_CurGrid[0]->GetMiddle();

    if( m_Size > 1 )
    {
        if( m_Flip == SDL_FLIP_NONE )
            p.x -= first->GetWidth() / 4;
        else
            p.x += first->GetWidth() / 4;
    }
    SetPosition( p.x, p.y );
}

void Unit::SetFlip( SIDE direction, bool updateGrids )
{
    SDL_RendererFlip prevFlip = m_Flip;
    if( direction == NONE )
    {
        if( m_Player == LEFT )
            m_Flip = SDL_FLIP_NONE;
        else
            m_Flip = SDL_FLIP_HORIZONTAL;
    }
    else if( m_Player == direction )
        m_Flip = m_Player == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    else
        m_Flip = m_Player == LEFT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    if( prevFlip != m_Flip && updateGrids )
    {
        if( m_Size > 1 )
            RotateGrids();
        SetGrid( m_CurGrid[0], m_CurGrid[1] );
    }
}

NodePtr Unit::InRange( Grid* chosenGrid, uint speed )
{
    Grid * to = nullptr;
    auto & Game = GameManager::GetSingleton();
    SDL_Point mousePos = Game.GetMousePos();
    uint unitSpeed = speed > 0 ? speed : m_Stats.speed;
    BattleScreen * pScreen = dynamic_cast<BattleScreen *>( Game.GetScreen() );
    if( !pScreen )
        return nullptr;
    Grid* grid = nullptr, * second = nullptr;

    if( chosenGrid )
        to = chosenGrid;
    else
        to = pScreen->FindGrid( mousePos.x, mousePos.y );

    if( !to || !to->InRange() || ( to->GetOccupied() && ( !chosenGrid || to->GetUnit() != this ) ) )
        return nullptr;
    grid = to;
    SIDE moveDir = GetMoveDirection( to );
    if( m_Size > 1 )
    {
        if( m_Player == moveDir )
        {
            second = pScreen->GetPreviousNextGrid( to, moveDir == LEFT );
            if( second && !second->GetOccupied() && second->InRange() )
                grid = second;
        }
        else
        {
            second = pScreen->GetPreviousNextGrid( to, moveDir == RIGHT );
            if( second && second->GetOccupied() && second->GetUnit() != this )
                to = grid = pScreen->GetPreviousNextGrid( to, moveDir == LEFT );
        }
    }

    if( grid && ( grid->GetOccupied() || !grid->InRange() ) )
        return nullptr;
    NodePtr dest = FindRoute( m_CurGrid, grid, unitSpeed, m_Type );
    if( dest && dest->pPrev && dest->pPrev->pGrid )
    {
        if( grid != to )
        {
            if( ( m_Player == LEFT && dest->pPrev->pGrid->GetMiddle().x < grid->GetMiddle().x ) ||
                ( m_Player == RIGHT && dest->pPrev->pGrid->GetMiddle().x > grid->GetMiddle().x ) )
                    dest = FindRoute( m_CurGrid, to, m_Stats.speed, m_Type );
        }
        else if( moveDir != m_Player && second )
        {
            if( ( m_Player == LEFT && dest->pPrev->pGrid->GetMiddle().x > grid->GetMiddle().x ) ||
            ( m_Player == RIGHT && dest->pPrev->pGrid->GetMiddle().x < grid->GetMiddle().x ) )
                dest = FindRoute( m_CurGrid, second, m_Stats.speed, m_Type );
        }
    }
    return dest;
}

bool Unit::PerformAttack()
{
    if( !m_AttackedUnit )
        return false;
    bool canHitBack = true;
    uint frame = 0;
    auto anim = m_AnimationController.GetCurrentAnimation();
    {
        if( m_State == STATE::SHOOT )
        {
            frame = anim.m_Frames.size() / 3;
            if( anim.m_CurrentFrame >= frame && m_Shot && !m_Shot->IsActive() )
            { 
                SetPosition( m_Position.x, m_Position.y );
                m_Shot->SetOwner( this );
                m_Shot->SetPosition( anim.GetName() );
                m_Shot->Initialize();
            } 
        }
        frame = anim.m_Frames.size() / 2 - 1;
        if( anim.m_CurrentFrame >= frame )
        {
            if( GameManager::GetSingleton().GetActiveUnit().get() != this || m_State == STATE::SHOOT )
                canHitBack = false;
            uint damage =  m_Stats.attack * m_Stats.quantity;
            if( ( m_State == STATE::SHOOT && !IsInGoodShotRange( m_AttackedUnit ) ) || ( m_Type == TYPE::ARCHER && m_State == STATE::ATTACK ) )
                damage = damage * 2 / 3;
            m_AttackedUnit->GetHit( damage, this, canHitBack );
            m_AttackedUnit->PauseAnimation( SIDE::NONE );
            return true;
        }
    }
    return false;
}

void Unit::GetHit( uint damage, Unit* attacker, bool canHitBack )
{
    float def = 1;
    if( m_DefenseMode )
        def = 1;
    else
        def = ( rand() * m_Stats.defense % 100 ) / 100.0f;
    float totalLife = float( m_Stats.lifePerUnit * ( m_Stats.quantity - 1 ) + m_Stats.lifeRemaining );
    totalLife -= ( (float)damage - def * m_Stats.defense );
    if( totalLife > 0 )
    {
        m_Stats.quantity = (int)( ceil( totalLife / m_Stats.lifePerUnit ) );
        m_Stats.lifeRemaining = (int)( totalLife - ( m_Stats.quantity - 1 ) * m_Stats.lifePerUnit );

        if( IsHit() == SIDE::NONE )
            SetHit( attacker );
        if( canHitBack && m_CanHitBack && !m_DefenseMode )
        {
            m_AttackedUnit = attacker;
            m_CanHitBack = false;
            SIDE pauseDir = attacker->GetFlip() == SDL_FLIP_NONE ? RIGHT : LEFT;
            attacker->PauseAnimation( pauseDir );
            attacker->SetHit( this );
        }
        else
        {
            m_AttackedUnit = nullptr;
            attacker->PauseAnimation( SIDE::NONE );
        }
    }
    else
        m_Stats.quantity = 0;
    m_AnimationController.DisableCurrentAnimation( totalLife <= 0 );
}

void Unit::Deactivate()
{
    m_IsActive = false;
    m_CurGrid[0]->SetOccupied( false );
    if( m_CurGrid[1] )
        m_CurGrid[1]->SetOccupied( false );
    m_StatsBoard->SetIsActive( false );
}

void Unit::SetPlayer( SIDE side )
{
    m_Player = side;
    if( m_Player == RIGHT )
        m_Flip = SDL_FLIP_HORIZONTAL;
}

void Unit::SetDefendMode( bool defend )
{
    m_DefenseMode = defend;
    m_AnimationController.DisableCurrentAnimation();
}

void Unit::SetHit( Unit* attacker )
{
    if( attacker && attacker->GetState() == STATE::SHOOT )
        m_IsHit = attacker->GetPlayer();
    else
        m_IsHit = GetEnemyDirection( attacker );
    m_AnimationController.DisableCurrentAnimation();
}

void Unit::SetWaiting( bool wait )
{
    m_Waiting = wait;
    if( wait )
        m_AnimationController.DisableCurrentAnimation();
}

void Unit::SetActiveState()
{
    m_AnimationController.SetActiveStateAnimation();
   
    m_DefenseMode = false;
}

void Unit::DisableCurrentAnimation()
{
    m_AnimationController.DisableCurrentAnimation();
}

bool Unit::IsInGoodShotRange( Unit* target )const
{
    uint row = 0, col = 0;
    uint myRow = m_CurGrid[0]->GetRow();
    uint myColumn = m_CurGrid[0]->GetCol();
    if( target )
    {
        if( Grid** grid = target->GetGrid() )
        {
            row = grid[0]->GetRow();
            col = grid[0]->GetCol();
            if( target->GetSize() == 2 )
            {
                if( col < myColumn && col < grid[1]->GetCol() )
                    col = grid[1]->GetCol();
                else if( col > myColumn && col > grid[1]->GetCol() )
                    col = grid[1]->GetCol();
            }
            if( m_Size == 2 )
            {
                if( col < myColumn && m_CurGrid[1]->GetCol() < myColumn )
                    myColumn = m_CurGrid[1]->GetCol();
                else if( col > myColumn && m_CurGrid[1]->GetCol() > myColumn )
                    myColumn = m_CurGrid[1]->GetCol();
            }
        }
    }
    int maxColDiff = myColumn;
    int diff = abs( int(row - myRow) );
    if( myRow % 2 == 0 )
        diff++;
    if( diff > GOOD_SHOT_RANGE )
        return false;
    if( diff <= 1 )
        maxColDiff = GOOD_SHOT_RANGE;
    else
        maxColDiff = GOOD_SHOT_RANGE - diff / 2;
    diff = abs( int(col - myColumn) );
    return diff <= maxColDiff;
}

bool Unit::IsInAttackRange( vector<Grid*> possiblePositions, bool shootingRange )const
{
    for( auto g : possiblePositions )
    {
        if( g && g->InRange() )
        {
            if( !g->GetOccupied() || g->GetUnit() == this )
            {
                if( !shootingRange )
                    return true;
            }
            else if( g->GetUnit()->GetPlayer() != m_Player && shootingRange )
                return false;
        }
    }
    return shootingRange;
}

bool Unit::IsIdle()const
{
    return m_State == STATE::STAND || m_State == STATE::IDLE;
}

void Unit::ModulateAlpha()
{
    static int add = -6;
    if( m_State == STATE::ACTIVE )
    {
        if( m_ActiveUnitAlpha <= 20 )
            add = 6;
        else if( m_ActiveUnitAlpha >= 255 )
            add = -6;
        m_ActiveUnitAlpha += add;
    }
}