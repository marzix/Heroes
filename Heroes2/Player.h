#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Declarations.h"
#include "Descriptor.h"
#include "Hero.h"

class BattleScreen;

class Player
{
public:
    Player() = default;
    Player( string n );
    Player( Player && rhs );

    void SetColor( PLAYER_COLOR::Color color );
    void SetCastle( Descriptor& rCastle );
    void SetCastlePos( int x, int y );
    void SetHeroDescr( Descriptor & rHero );
    void SetHeroDescrPos( int x, int y );
    void SetHero( HeroPtr hero );
    void SetHeroBodyTex( Image body );
    void SetName( string n );
    void SetUnits( BattleScreen* pScreen );
    void SetSide( SIDE playerSide );
    void SetIsAI( bool isAI );
    void SetNameImagePos( int x, int y );
    bool ChangeName();
    void EndEdditing();

    UnitPtr             GetUnit( unsigned int i );
    PLAYER_COLOR::Color GetColor()const;
    Descriptor &        GetCastle();
    HeroPtr             GetHero();
    ImagePtr            GetName();
    vector<UnitPtr> &   GetAllUnits();
    SIDE                GetSide()const;
    bool IsAI()const;
    bool IsDefeated();
    void AddUnit( UnitPtr& unit );
    void ResetArmy();
    void AddToGameQueue();
    void RenderHero();

    bool active = false;
private:

    vector<UnitPtr> m_Units;
    Descriptor m_Castle;
    HeroPtr m_Hero;
    ImagePtr m_Name;
    PLAYER_COLOR::Color m_FlagColor = PLAYER_COLOR::RED;
    SIDE m_PlayerSide = SIDE::NONE;
    bool m_IsAI = false;
};

#endif