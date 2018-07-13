#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "VisibleObject.h"

class Button;
using ButtonPtr = shared_ptr<Button>;

class Button: public VisibleObject
{
public:
    Button() = default;
    Button( int x, int y, ImagePtr notActive, ImagePtr active, ImagePtr disabled, ImagePtr pressed, function<void()> f, SDL_RendererFlip buttonFlip = SDL_FLIP_NONE );

    void Initialize()override;
    void Actualize()override;
    void Render()override;
    void Reset()override;
    void SetPosition( int x, int y )override;
    void SetDisabled();
    void SetEnabled();
    void SetStayPushed( bool stayPushed );

private:
    function<void()> m_OnClick; 
    ImagePtr m_ButtonNotActive;
    ImagePtr m_ButtonActive;
    ImagePtr m_ButtonPressed;
    ImagePtr m_ButtonDisabled;
    
    bool m_StayPushed = false;

    static float s_PressTime;
    static string s_SoundName;
};

#endif