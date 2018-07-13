#include <stdlib.h>
#include <time.h>
#include "Declarations.h"
#include "AnimationState.h"
#include "Unit.h"
#include "GameManager.h"
#include "BattleScreen.h"

AnimationState::AnimationState( STATE::Type _name )
: name( _name )
{
}

bool CheckActiveCondition( Unit& unit )
{
    return unit.GetState() == STATE::ACTIVE;
}

bool CheckStandCondition( Unit& unit )
{
    if( unit.GetState() == STATE::MOVE && unit.GetType() == TYPE::FLYING )
        return false;
    if( unit.GetQuantity() <= 0 || unit.GetRouteSize() )
        return false;
    else if( unit.GetState() == STATE::ACTIVE )
    {
        auto& Game = GameManager::GetSingleton();
        if( !Game.RoundIsOver() && Game.GetActiveUnit()->GetGrid() == unit.GetGrid() )
            return false;
    }
    return true;
}

bool CheckIdleCondition( Unit& unit )
{
    //srand( (uint)time( nullptr ) );
    int i = rand() % 3;
    cout << i << endl;
    //the IDLE state activates after a random number of STAND states
    return i == 0;
}

bool CheckStartMoveCondition( Unit& unit )
{
    return CheckMoveCondition( unit ) && unit.GetType() == TYPE::FLYING;
}

bool CheckMoveCondition( Unit& unit )
{
    STATE::Type unitState = unit.GetState();
    return unit.IsActive() && unit.GetRouteSize();
}

bool CheckEndMoveCondition( Unit& unit )
{
    if( unit.GetType() != TYPE::FLYING )
        return false;
    return !unit.GetRouteSize();
}

bool CheckTurnCondition( Unit& unit )
{
    STATE::Type unitState = unit.GetState();
    SIDE player = unit.GetPlayer();

    if( unit.GetType() == TYPE::FLYING && unitState == STATE::MOVE )
        return false;
    if( unit.GetRouteSize() )
    {
        if( player == LEFT && unit.GetMoveDirection( nullptr ) == LEFT )
            return true;
        else if( player == RIGHT  && unit.GetMoveDirection( nullptr ) == RIGHT )
            return true;
    }
    else if( unitState == STATE::ATTACK || unitState == STATE::SHOOT )
    {
        if( unit.GetFlip() == SDL_FLIP_NONE && player == RIGHT )
                return true;
        else if( unit.GetFlip() != SDL_FLIP_NONE && player == LEFT )
                return true;
    }
    else if( unit.IsHit() != SIDE::NONE )
    {
        if( ( unit.IsHit() == SIDE::LEFT && unit.GetFlip() == SDL_FLIP_NONE ) || ( unit.IsHit() == SIDE::RIGHT && unit.GetFlip() != SDL_FLIP_NONE ) )
            return true;
        else
            return false;
    }
    else if( ( unitState == STATE::END_MOVE && unit.GetType() == TYPE::FLYING ) || ( ( unitState == STATE::MOVE || unitState == STATE::STAND || unitState == STATE::GET_HIT )
            && unit.GetRouteSize() == 0 ) )
    {
        auto direction = unit.GetEnemyDirection();
        if( direction != NONE && unit.GetAttackedUnit()->GetState() == STATE::SHOOT )
            return false;
        if( unit.GetFlip() == SDL_FLIP_NONE )
        {
            if(( player == LEFT && direction == LEFT ) || ( player == RIGHT && direction != RIGHT ))
                return true;
        }
        else
        {
            if(( player == RIGHT && direction == RIGHT ) || ( player == LEFT && direction != LEFT ))
                return true;
        }
    }
    return false;
}

bool CheckAttackCondition( Unit& unit )
{
    return unit.GetAttackedUnit() && !unit.GetRouteSize();
}

bool CheckGetHitCondition( Unit& unit )
{
    return unit.IsHit() != SIDE::NONE && !unit.GetDefendMode();
}

bool CheckDefendCondition( Unit& unit )
{
    return unit.IsHit() != SIDE::NONE && unit.GetDefendMode();
}

bool CheckDieCondition( Unit& unit )
{
    return unit.GetQuantity() <= 0;
}

bool CheckShootCondition( Unit& unit )
{
    Unit* enemy = unit.GetAttackedUnit();
    if( unit.GetType() == TYPE::ARCHER && enemy && unit.GetRouteSize() == 0 )
    {
        Grid** archerPos = unit.GetGrid();
        Grid** enemyPos = enemy->GetGrid();
        uint myColumn = archerPos[0]->GetCol();
        uint col = enemyPos[0]->GetCol();
        if( enemy->GetSize() == 2 )
        {
            if( col < myColumn && col < enemyPos[1]->GetCol() )
                col = enemyPos[1]->GetCol();
            else if( col > myColumn && col > enemyPos[1]->GetCol() )
                col = enemyPos[1]->GetCol();
        }
        if( unit.GetSize() == 2 )
        {
            if( col < myColumn && archerPos[1]->GetCol() < myColumn )
                myColumn = archerPos[1]->GetCol();
            else if( col > myColumn && archerPos[1]->GetCol() > myColumn )
                myColumn = archerPos[1]->GetCol();
        }
        uint myRow = archerPos[0]->GetRow();
        int rowDiff = myRow - enemyPos[0]->GetRow();
        if( rowDiff <= 1 && rowDiff >= -1 )
        {
            int colDiff = myColumn - col;
            if( colDiff == 0 )
                return false;
            if( colDiff == 1 && ( rowDiff == 0 || myRow % 2 == 0 ) )
                return false;
            if( colDiff == -1 && ( rowDiff == 0 || myRow % 2 ) )
                return false;
        }
        return true;
    }
    return false;
}