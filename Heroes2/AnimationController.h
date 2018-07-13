#ifndef __ANIMATION_CONTROLLER_H__
#define __ANIMATION_CONTROLLER_H__

#include "Declarations.h"
#include "AnimationState.h"
#include "Animation.h"

class AnimationController
{
public:
    AnimationController( Unit* unit );
    static void LoadAnimationStates();
    void Initialize();
    void Actualize();
    void Pause( SIDE pauseInDirection );
    void DisableCurrentAnimation( bool force = false );
    void SetActiveStateAnimation();
    bool IsPaused()const;
    vector<Transition>  GetTransitions()const;
    Animation           GetCurrentAnimation();
    bool                CurAnimationFinished();

private:
    static vector<AnimationState> s_AnimStates;
    Animation m_CurrentAnim;
    SIDE m_PauseAnimationDirection = NONE;

    Unit* m_Unit;
};

#endif