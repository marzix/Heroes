#ifndef __ANIMSET_H__
#define __ANIMSET_H__

#include "Declarations.h"

class AnimationSet
{
public:
    AnimationSet() = default;
    AnimationSet( string n );
    ~AnimationSet();
    
    string GetName();

    AnimationPtr Find( Image* current );
    AnimationPtr Find( string state );

    const AnimationSet & operator+=( AnimationPtr & anim );

private:
    string m_Name;
    vector<AnimationPtr> m_Set;
};

#endif