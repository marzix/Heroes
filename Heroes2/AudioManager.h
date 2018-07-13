#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include "Audio.h"

class AudioManager
{
public:
    ~AudioManager();
    static AudioManager& GetSingleton();

    bool     Initialize();
    void     Close();
    bool     LoadAudio();
    bool     IsPlaying();
    void     PlayTrack( const string& soundtrackName );
    void     PlaySound( const string& soundtrackName );
    AudioPtr FindAudio( const string& name );
    void     Stop();
    void     Pause();
    void     Resume();

private:
    static AudioManager s_Audio;

    vector<AudioPtr> m_SoundTracks;
    AudioPtr m_NowPlayed;
};

#endif
