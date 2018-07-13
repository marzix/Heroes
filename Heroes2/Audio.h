#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL_mixer.h>
#include "Declarations.h"

class Audio;
using AudioPtr = shared_ptr<Audio>;

class Audio
{
public:
    Audio( string file, string name, bool looped );
    ~Audio();

    void            Load( bool track );
    void            Play( bool track );
    void            Stop();
    void            Pause();
    void            Resume();
    bool            IsPlaying();
    const string&   GetName()const;

protected:
    int m_Loops;
    string m_File;
    string m_Name;
    Mix_Music* m_Music = nullptr;
    Mix_Chunk* m_Sound = nullptr;
};


#endif