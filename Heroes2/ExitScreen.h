#ifndef __EXIT_H__
#define __EXIT_H__

#include "Declarations.h"
#include "Dialog.h"
#include "Button.h"
#include "Texture.h"

class ExitScreen: public Dialog
{
public:
    ExitScreen( string n );
    ~ExitScreen();

    virtual void Initialize()override;
    virtual void Actualize()override;
    virtual void Reset()override;
    virtual void Render()override;

    void RenderPrevScreenAsBackground();

    ButtonPtr m_Tick; 
    ButtonPtr m_Cancel;
    Texture m_Background;
};

#endif