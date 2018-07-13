#ifndef __PLAYER_OPTION_PANEL_H__
#define __PLAYER_OPTION_PANEL_H__

#include "Declarations.h"
#include "Button.h"

class PlayerOptionPanel
{
public:
    PlayerOptionPanel( PLAYER_COLOR::Color colorName, ButtonPtr flag, ImagePtr barTex );

    PLAYER_COLOR::Color GetColor()const;
    ButtonPtr           GetButton()const;
    ImagePtr            GetBar()const;
    int                GetPlayerIndex();

    void SetPosition( int x, int y );
    void SetPlayerIndex( int i );
    void Render();

private:
    PLAYER_COLOR::Color m_FlagColor;
    ButtonPtr m_FlagButton;
    ImagePtr m_OptionBar;
    int m_PlayerIndex = -1;
};

#endif