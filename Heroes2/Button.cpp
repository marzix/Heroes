#include "Button.h"
#include "Image.h"
#include "GameManager.h"
#include "AudioManager.h"

float Button::s_PressTime = 0.2f;
string Button::s_SoundName = "Button";

Button::Button( int x, int y, ImagePtr notActive, ImagePtr active, ImagePtr disabled, ImagePtr pressed, function<void()> f, SDL_RendererFlip buttonFlip )
: m_ButtonNotActive( notActive )
, m_ButtonActive( active )
, m_ButtonPressed( pressed )
, m_ButtonDisabled( disabled )
, m_OnClick( f )
{
    m_Position.x = x;
    m_Position.y = y;
    m_CurImage = m_ButtonNotActive;
    m_Flip = buttonFlip;
}

void Button::Initialize()
{
    //button active and disabled images are optional
    if( m_ButtonActive )
        m_ButtonActive->SetRect( m_Position.x, m_Position.y );
    if( m_ButtonDisabled )
        m_ButtonDisabled->SetRect( m_Position.x, m_Position.y );
    //not active and pressed images are assumed to exist
    m_ButtonNotActive->SetRect( m_Position.x, m_Position.y );
    m_ButtonPressed->SetRect( m_Position.x, m_Position.y );
}

void Button::Actualize()
{
    static float dt = 0;
    if( m_CurImage == m_ButtonDisabled )
        return;
    auto& Game = GameManager::GetSingleton();
    if( m_IsActive )
    {
        dt += Game.GetDeltaTime();
        if( dt >= s_PressTime )
        {
            m_OnClick();
            dt = 0;
            m_IsActive = false;
        }
    } 
    else if( Game.IsMouseOn( this ) )
    {
        if( !m_StayPushed )
        {
            if( m_ButtonActive )
                m_CurImage = m_ButtonActive;
            else
                m_CurImage = m_ButtonNotActive;
        }

        if( Game.WasMouseButtonPressed( SDL_BUTTON_LEFT ) && m_OnClick )
        {        
            m_IsActive = true;
            if( m_StayPushed )
            {
                if( m_CurImage == m_ButtonPressed )
                    m_CurImage = m_ButtonNotActive;
                else
                    m_CurImage = m_ButtonPressed;
            }
            else
                m_CurImage = m_ButtonPressed;
            AudioManager::GetSingleton().PlaySound( s_SoundName );
            dt = 0;
        }
    }
    else if( !m_StayPushed )
    {
        m_CurImage = m_ButtonNotActive;
    }
}

void Button::Render()
{
    m_CurImage->RenderTexture( m_Flip );
}

void Button::Reset()
{
    m_IsActive = false;
    m_CurImage = m_ButtonNotActive;
}

void Button::SetPosition( int x, int y )
{
    m_Position.x = x;
    m_Position.y = y;
    m_ButtonNotActive->SetRect( x, y );
    m_ButtonPressed->SetRect( x, y );
    if( m_ButtonActive )
        m_ButtonActive->SetRect( x, y );
}

void Button::SetDisabled()
{
    if( m_ButtonDisabled )
        m_CurImage = m_ButtonDisabled;
}

void Button::SetEnabled()
{
    m_CurImage = m_ButtonNotActive;
}

void Button::SetStayPushed( bool stayPushed )
{
    m_StayPushed = stayPushed;
}


