#include "AudioManager.h"

AudioManager AudioManager::s_Audio;

AudioManager::~AudioManager()
{
    Close();
}

AudioManager& AudioManager::GetSingleton()
{
    return s_Audio;
}

bool AudioManager::Initialize()
{
    //Initialize SDL 
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) 
    { 
        logSDLError( "SDL could not initialize!" ); 
        return false; 
    }
    //Initialize SDL_mixer 
    Mix_Init(0);
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0 ) 
    {
        logSDLError( "SDL_mixer could not initialize!" ); 
        return false;
    }
    return true;
}

void AudioManager::Close()
{
    if( m_NowPlayed && m_NowPlayed->IsPlaying() )
    {
        m_NowPlayed->Stop();
    }
    m_NowPlayed.reset();
    m_SoundTracks.clear();
    Mix_Quit();
}

bool AudioManager::LoadAudio()
{
    rapidxml::xml_document<> Doc;
    ifstream XmlFile{ AUDIO_FILE };
    Dialog * pDialog = nullptr;
    if( !XmlFile.is_open() )
        return false;

    string content( (std::istreambuf_iterator<char>( XmlFile ) ), std::istreambuf_iterator<char>() );
    XmlFile.close();

    Doc.parse<0>( &content[0] );
    auto RootNode = Doc.first_node();
    rapidxml::xml_node<char>* node = nullptr;

    if( !RootNode )
        return false;
    string path = SDL_GetBasePath();
    if( RootNode )
    {
        string key = RootNode->name();
        if( key == string( "Audio" ) )
        {
            node = RootNode->first_node();
            while( node )
            {
                string file, name, lString;
                bool looped = false;

                if( auto _file = node->first_attribute( "file" ) )
                    file = _file->value();
                if( auto _name = node->first_attribute( "name" ) )
                    name = _name->value();
                if( auto _looped = node->first_attribute( "looped" ) )
                    lString = _looped->value();
                if( lString.compare( "T" ) == 0 )
                    looped = true;
                m_SoundTracks.push_back( make_shared<Audio>( path + file, name, looped ) );
                node = node->next_sibling();
            }
        }
    }
    return true;
}

bool AudioManager::IsPlaying()
{
    if( m_NowPlayed )
        return m_NowPlayed->IsPlaying();
    return false;
}

void AudioManager::PlayTrack( const string& soundtrackName )
{
    if( !m_NowPlayed || m_NowPlayed->GetName().compare( soundtrackName ) != 0 )
    {
        if( m_NowPlayed )
            m_NowPlayed->Stop();
        m_NowPlayed.reset();
        m_NowPlayed = FindAudio( soundtrackName );
    }
    if( m_NowPlayed )
    {
        m_NowPlayed->Load( true );
        m_NowPlayed->Play( true );
    }
}

void AudioManager::PlaySound( const string& soundtrackName )
{
    AudioPtr sound = FindAudio( soundtrackName );    
    if( sound )
    {
        sound->Load( false );
        sound->Play( false );
    }
}

AudioPtr AudioManager::FindAudio( const string& name )
{
    for( auto& audio : m_SoundTracks )
    {
        if( audio && audio->GetName().compare( name ) == 0 )
            return audio;
    }
    return nullptr;
}

void AudioManager::Pause()
{
    if( !m_NowPlayed )
        return;
    m_NowPlayed->Pause();
}

void AudioManager::Resume()
{
    if( !m_NowPlayed )
        return;
    m_NowPlayed->Resume();
}

void AudioManager::Stop()
{
    if( !m_NowPlayed )
        return;
    m_NowPlayed->Stop();
}