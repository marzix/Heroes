#include "AnimationSet.h"
#include "Animation.h"

AnimationSet::AnimationSet( string n )
: m_Name( n )
{}

AnimationSet::~AnimationSet()
{
    m_Set.clear();
}

const AnimationSet & AnimationSet::operator+=( AnimationPtr & anim )
{
    m_Set.push_back( anim );
    return *this;
}

string AnimationSet::GetName()
{
    return m_Name;
}

AnimationPtr AnimationSet::Find( Image * current )
{
    for( auto & anim: m_Set )
    {
        if( anim->Find( current ) >= 0 )
        {
            return anim;
        }
    }
    return nullptr;
}

AnimationPtr AnimationSet::Find( string state )
{
    for( auto & anim: m_Set )
    {
        if( anim->GetName() == state )
        {
            return anim;
        }
    }
    return nullptr;
}