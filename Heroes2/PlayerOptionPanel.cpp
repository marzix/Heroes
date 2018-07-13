#include "PlayerOptionPanel.h"
#include "Image.h"

PlayerOptionPanel::PlayerOptionPanel( PLAYER_COLOR::Color colorName, ButtonPtr flag, ImagePtr barTex )
    : m_FlagColor( colorName )
    , m_FlagButton( flag )
    , m_OptionBar( barTex )
{}

PLAYER_COLOR::Color PlayerOptionPanel::GetColor()const
{
    return m_FlagColor;
}

ButtonPtr PlayerOptionPanel::GetButton()const
{
    return m_FlagButton;
}

ImagePtr PlayerOptionPanel::GetBar()const
{
    return m_OptionBar;
}

void PlayerOptionPanel::SetPosition( int x, int y )
{
    m_FlagButton->SetPosition( x, y );
    x += m_FlagButton->GetImage()->GetRect().w;
    m_OptionBar->SetRect( x, y );
}

void PlayerOptionPanel::SetPlayerIndex( int i )
{
    m_PlayerIndex = i;
}

int PlayerOptionPanel::GetPlayerIndex()
{
    return m_PlayerIndex;
}

void PlayerOptionPanel::Render()
{
    m_FlagButton->Render();
    m_OptionBar->RenderTexture();
}