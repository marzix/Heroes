#ifndef __MENU_H__
#define __MENU_H__

#include "Dialog.h"
#include "Button.h"

class MenuScreen: public Dialog
{
public:

    MenuScreen( string n );
    ~MenuScreen();
    virtual void Initialize()override;
    virtual void Reset()override;
    virtual void Actualize()override;
    virtual void Render()override;

    void AddButton( ButtonPtr button );

private:
    vector<ButtonPtr> m_Buttons;
};

#endif