#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include "Dialog.h"
#include "Button.h"
#include "Castle.h"
#include "PlayerOptionPanel.h"
#include "Player.h"

class OptionsScreen: public Dialog
{
public:
    OptionsScreen( string n );
    ~OptionsScreen();

    void Initialize()override;
    void Reset()override;
    void Actualize()override;
    void Render()override;

    void InitializePlayers();
    void SetName( string n, int index );
    void SetEnterName( int val );
    void SetColor( PLAYER_COLOR::Color color, int index );
    void SetText();
    void SetTextPos();
    void SetBars();
    void SetCastles( rapidxml::xml_node<char> * rootNode );
    void SetBattle();
    void SwitchPlayerColor( uint index );

    Player*             GetPlayer( int index );
    Castle*             GetCastle( const string castle );
    PlayerOptionPanel*  FindOptionPanel( PLAYER_COLOR::Color color );
    PlayerOptionPanel*  FindOptionPanel( Button * button );
    int                 FindCurrentCastle( const string castleName );
    Castle*             FindNextCastleTex( const string castleName );
    Castle*             FindPrevCastleTex( const string castleName );
    Descriptor*         FindNextHero( int playerIndex, bool reverseOrder );
    Button*             FindButton( const string name );

private:
    ImagePtr m_Frame;
    vector<ButtonPtr> m_ButtonList;
    vector<ImagePtr> m_Text;
    vector<PlayerOptionPanel> m_PlayerOptionList;
    vector<Castle> m_CastleList;
    Player* m_Players[2];
    int enterName = -1;
};

#endif