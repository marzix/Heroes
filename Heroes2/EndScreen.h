#ifndef __END_H__
#define __END_H__

#include "Declarations.h"
#include "Dialog.h"
#include "Button.h"
#include "Image.h"
#include "Player.h"
#include "Texture.h"

class EndScreen: public Dialog
{
public:
    EndScreen() = default;
    EndScreen( string n );

    void Initialize()override;
    void Reset()override;
    void Actualize()override;
    void Render()override;

    void SetPlayers( Player* players[2] );
    void SetStatistics( int playerIndex );

    void RenderPrevScreenAsBackground();

private:
    ButtonPtr m_OkButton;
    vector<ImagePtr> m_Images;
    Player* m_Player[2];
    Texture m_Background;
};

#endif