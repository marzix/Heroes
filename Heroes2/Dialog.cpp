#include "GameManager.h"
#include "Dialog.h"

Dialog::Dialog()
{ 
    m_Position.x = 0; 
    m_Position.y = 0; 
}

bool Dialog::Quit( SDL_Event const & e )const
{
    if( e.type == SDL_QUIT )
    {
        return true;
    }
    if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE )
    {
        GameManager::GetSingleton().SetScreen( m_Previous );
        //GameManager::GetSingleton().SetCursorTexture();
        GameManager::GetSingleton().ResetEvent();
        return true;
    }
    
    return false;
}

ImagePtr Dialog::GetBackground()
{
    return GetImage();
}

Dialog * Dialog::GetNext()
{
    return m_Next;
}

Dialog * Dialog::GetPrev()
{
    return m_Previous;
}

void Dialog::SetSoundName( string fileName )
{
    m_SoundTrackName = fileName;
}