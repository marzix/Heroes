#include "Animation.h"
#include "GameManager.h"
#include "Unit.h"

Animation::Animation( STATE::Type name, float FT, string soundFile )
: m_FrameTime( FT )
, m_SoundName( soundFile )
{
    m_Name = STATE::ToString( name );
}

Animation::Animation( string name, float FT, string soundFile )
: m_FrameTime( FT )
, m_SoundName( soundFile )
{
    m_Name = name;
}

Animation::~Animation()
{
    m_Frames.clear();
}

void Animation::Initialize()
{
}

void Animation::Reset()
{
    m_CurrentTime = 0;
    m_CurrentFrame = 0;
}

void Animation::Actualize()
{
    m_CurrentTime += GameManager::GetSingleton().GetDeltaTime();
    if( m_CurrentTime < m_FrameTime )
        return;

    m_CurrentTime = 0;
    if( m_CurrentFrame >= (uint)m_Frames.size() - 1 )
    {
        m_IsActive = false;
        return;
    }
    m_CurrentFrame++;
}

void Animation::Animate( VisibleObject& obj )
{
    Actualize();
    if( m_IsActive )
        obj.SetImage( m_Frames[ m_CurrentFrame ] );
}

const Animation & Animation::operator+=( ImagePtr & img )
{
    m_Frames.push_back( img );
    return *this;
}


int Animation::Find( Image * current )
{
    for( unsigned int j = 0; j < m_Frames.size(); j++ )
    {
        if( m_Frames[j].get() == current )
        {
            return j;
        }
    }
    return -1;
}
