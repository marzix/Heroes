#ifndef __ANIMATION_STATE_H__
#define __ANIMATION_STATE_H__

#include "Declarations.h"

struct Transition
{
    Transition( STATE::Type name ) :targetName( name ){}

    STATE::Type targetName;
    function<bool( Unit& )> CheckCondition;
};

struct AnimationState
{
    AnimationState( STATE::Type _name );

    STATE::Type name;
    vector<Transition> transitions;
};

bool CheckActiveCondition( Unit& unit );
bool CheckStandCondition( Unit& unit );
bool CheckIdleCondition( Unit& unit );
bool CheckStartMoveCondition( Unit& unit );
bool CheckMoveCondition( Unit& unit );
bool CheckEndMoveCondition( Unit& unit );
bool CheckTurnCondition( Unit& unit );
bool CheckAttackCondition( Unit& unit );
bool CheckGetHitCondition( Unit& unit );
bool CheckDefendCondition( Unit& unit );
bool CheckDieCondition( Unit& unit );
bool CheckShootCondition( Unit& unit );

#endif