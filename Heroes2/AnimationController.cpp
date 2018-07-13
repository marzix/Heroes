#include "Declarations.h"
#include "AnimationController.h"
#include "AnimationState.h"
#include "Animation.h"
#include "Unit.h"

vector<AnimationState> AnimationController::s_AnimStates;

AnimationController::AnimationController( Unit* unit )
{
    m_Unit = unit;
    if( s_AnimStates.empty() )
        LoadAnimationStates();
}

void AnimationController::Initialize()
{
    if( m_Unit )
    {
        m_CurrentAnim = m_Unit->GetAnimation( STATE::STAND );
        m_CurrentAnim.SetIsActive( true );
    }
}

void AnimationController::DisableCurrentAnimation( bool force )
{
    if( m_CurrentAnim.GetName() != "" )
    {
        STATE::Type currentState = STATE::GetState( m_CurrentAnim.GetName() );
        if( force || currentState == STATE::ACTIVE || currentState == STATE::STAND || currentState == STATE::IDLE )
            m_CurrentAnim.SetIsActive( false );
    }
    else
        Initialize();
}

void AnimationController::SetActiveStateAnimation()
{
    DisableCurrentAnimation();
    m_CurrentAnim = m_Unit->GetAnimation( STATE::ACTIVE );
    if( STATE::ToString( STATE::ACTIVE).compare( m_CurrentAnim.GetName() ) == 0 )
    {
        m_CurrentAnim.Reset();
        m_CurrentAnim.SetIsActive( true );
        m_Unit->SetState( STATE::ACTIVE );
    }
}

void AnimationController::LoadAnimationStates()
{
    Transition tActive{ STATE::ACTIVE };
    tActive.CheckCondition = CheckActiveCondition;
    Transition tStand{ STATE::STAND };
    tStand.CheckCondition = CheckStandCondition;
    Transition tIdle{ STATE::IDLE };
    tIdle.CheckCondition = CheckIdleCondition;
    Transition tStartMove{ STATE::START_MOVE };
    tStartMove.CheckCondition = CheckStartMoveCondition;
    Transition tMove{ STATE::MOVE };
    tMove.CheckCondition = CheckMoveCondition;
    Transition tEndMove{ STATE::END_MOVE };
    tEndMove.CheckCondition = CheckEndMoveCondition;
    Transition tTurn{ STATE::TURN };
    tTurn.CheckCondition = CheckTurnCondition;
    Transition tAttack{ STATE::ATTACK };
    tAttack.CheckCondition = CheckAttackCondition;
    Transition tShoot{ STATE::SHOOT };
    tShoot.CheckCondition = CheckShootCondition;
    Transition tGetHit{ STATE::GET_HIT };
    tGetHit.CheckCondition = CheckGetHitCondition;
    Transition tDefend{ STATE::DEFEND };
    tDefend.CheckCondition = CheckDefendCondition;
    Transition tDie{ STATE::DIE };
    tDie.CheckCondition = CheckDieCondition;

    s_AnimStates.push_back( AnimationState{ STATE::ACTIVE } );
    s_AnimStates.back().transitions = { tTurn, tDie, tDefend, tGetHit, tShoot, tAttack, tStartMove, tMove, tStand, tActive };

    s_AnimStates.push_back( AnimationState{ STATE::STAND } );
    s_AnimStates.back().transitions = { tTurn, tDie, tDefend, tGetHit, tAttack, tIdle, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::IDLE } );
    s_AnimStates.back().transitions = { tTurn, tDie, tDefend, tGetHit, tAttack, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::START_MOVE } );
    s_AnimStates.back().transitions = { tMove, tEndMove };

    s_AnimStates.push_back( AnimationState{ STATE::MOVE } );
    s_AnimStates.back().transitions = { tMove, tEndMove, tTurn, tAttack, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::END_MOVE } );
    s_AnimStates.back().transitions = { tTurn, tAttack, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::TURN } );
    s_AnimStates.back().transitions = { tDie, tDefend, tGetHit, tShoot, tAttack, tStartMove, tMove, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::ATTACK } );
    s_AnimStates.back().transitions = { tDie, tGetHit, tTurn, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::SHOOT } );
    s_AnimStates.back().transitions = { tTurn, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::GET_HIT } );
    s_AnimStates.back().transitions = { tDie, tTurn, tAttack, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::DEFEND } );
    s_AnimStates.back().transitions = { tDie, tTurn, tStand };

    s_AnimStates.push_back( AnimationState{ STATE::DIE } );
}

void AnimationController::Actualize()
{
    if( !m_Unit )
        return;
    if( !m_CurrentAnim.IsActive() )
    {
        vector<Transition> transitions = GetTransitions();
        for( auto transition : transitions )
        {
            if( transition.CheckCondition( *m_Unit ) )
            {
                m_CurrentAnim = m_Unit->GetAnimation( transition.targetName );
                m_CurrentAnim.Reset();
                m_CurrentAnim.SetIsActive( true );
                m_Unit->SetState( STATE::GetState( m_CurrentAnim.GetName() ) );
                break;
            }
        }
    }
    if( m_PauseAnimationDirection != SIDE::NONE && m_CurrentAnim.m_CurrentFrame == 0 )
    {
        auto anim = m_Unit->GetAnimation( STATE::STAND);
        m_Unit->SetFlip( m_PauseAnimationDirection, true );
        m_Unit->SetImage( anim.m_Frames[0] );
    }
    else
        m_CurrentAnim.Animate( *m_Unit );
}

vector<Transition> AnimationController::GetTransitions()const
{
    STATE::Type name = STATE::GetState( m_CurrentAnim.GetName() );
    for( auto& state : s_AnimStates )
    {
        if( state.name == name )
            return state.transitions;
    }
    return vector<Transition>();
}

void AnimationController::Pause( SIDE pauseInDirection )
{
    m_PauseAnimationDirection = pauseInDirection;
}

bool AnimationController::IsPaused()const
{
    return m_PauseAnimationDirection != SIDE::NONE;
}

Animation AnimationController::GetCurrentAnimation()
{
    return m_CurrentAnim;
}

bool AnimationController::CurAnimationFinished()
{
    return m_CurrentAnim.IsActive();
}