#include "Audio.h"

Audio::Audio( string file, string name, bool looped )
: m_File( file )
, m_Name( name )
{
    if( looped )
        m_Loops = -1;
    else
        m_Loops = 0;
}

Audio::~Audio()
{
    if( m_Music )
        Mix_FreeMusic( m_Music ); 
    m_Music = nullptr;
    if( m_Sound )
        Mix_FreeChunk( m_Sound );
    m_Sound = nullptr;
}

const string& Audio::GetName()const
{
    return m_Name;
}

void Audio::Load( bool track )
{
    if( !m_Music && track )
        m_Music = Mix_LoadMUS( m_File.c_str() );
    else if( !m_Sound && !track )
        m_Sound = Mix_LoadWAV( m_File.c_str() );
}

void Audio::Play( bool track )
{
    if( track && Mix_PlayingMusic() == 0 )
        Mix_PlayMusic( m_Music, m_Loops );
    else if( !track )
        Mix_PlayChannel( -1, m_Sound, m_Loops );
}

bool Audio::IsPlaying()
{
    return Mix_PlayingMusic() == 1;
}

void Audio::Stop()
{
    Mix_HaltMusic();
    Mix_FreeMusic( m_Music );
}

void Audio::Pause()
{
    if( Mix_PausedMusic() == 0 )
        Mix_PauseMusic();
}

void Audio::Resume()
{
    if( Mix_PausedMusic() )
        Mix_ResumeMusic();
}
