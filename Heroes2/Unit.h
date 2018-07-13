#ifndef __UNIT_H__
#define __UNIT_H__

#include "Image.h"
#include "VisibleObject.h"
#include "AnimationController.h"
#include "StatsInfoBoard.h"
#include "ShotObject.h"

struct Statistics
{
    uint speed;
    uint attack = 5;
    uint defense = 10;
    uint lifePerUnit = 20;
    int lifeRemaining = 20;
    int quantity = 2;

    ImagePtr numberImage;
    ImagePtr quantityInfoField;
};

class Unit: public VisibleObject
{
public:
    using VisibleObject::SetImage;

    Unit() = default;
    Unit( string name, int attack, int defense, int life, int speed, TYPE::Name type, int size, int number );
    Unit( string name, TYPE::Name type, uint size, Statistics stats );
    ~Unit();

    void    Initialize()override;
    void    Actualize()override;
    void    Render()override;
    void    Reset()override;
    void    MouseEvent();
    int     GetHeightForSorting()const override;
    void    Move();
    void    RotateGrids();
    void    PauseAnimation( SIDE pauseInDirection );
    bool    MoveToNextGrid( Grid* g );
    NodePtr InRange( Grid* dest, uint speed = 0 );
    bool    PerformAttack();
    void    GetHit( uint damage, Unit* attacker, bool canHitBack );
    void    Deactivate();
    
    Grid**          GetGrid();
    STATE::Type     GetState()const;
    int             GetSize()const;
    uint            GetSpeed()const;
    uint            GetAttack()const;
    uint            GetDefense()const;
    uint            GetLifePerUnit()const;
    int             GetLifeRemaining()const;
    uint            GetQuantity()const;
    const ImagePtr  GetQuantityImg()const;
    Unit*           GetAttackedUnit();
    uint            GetRouteSize()const;
    Animation       GetAnimation( STATE::Type )const;
    bool            CurrentAnimationFinished();
    TYPE::Name      GetType()const;
    SIDE            GetMoveDirection( Grid* grid )const;
    SIDE            GetEnemyDirection( Unit* = nullptr )const;
    SIDE            GetPlayer()const;
    bool            GetDefendMode()const;
    bool            GetWaiting()const;
    ShotObjectPtr   GetShotObject();
    const Image&    GetAvatar();
    SIDE            IsHit()const;
    bool            CanHitBack()const;
    bool            IsTurned();
    Grid*           FindAttackPosition( Grid** enemyPos, vector<Grid*> unitNeighbours );
    Grid*           FindClosestFreeGrid( SDL_Point mousePos, Unit* enemy, float& rotation, vector<Grid*> unitNeighbours );

    void        SetIsActive( bool val );
    void        SetAtteckedUnit( Unit* attacker );
    void        SetState( STATE::Type state );
    void        SetQuantityInfoImage( ImagePtr rect );
    void        SetQuantityInfoImage();
    void        SetStatsBoard( StatsInfoBoardPtr board );
    void        SetRoute( NodePtr dest );
    void        SetPosition( int x, int y );
    void        SetPosition();
    void        SetImage();
    void        SetAnimations( AnimationSetPtr set );
    void        SetGrid( Grid * first, Grid * second = nullptr, bool setUnit = true );
    void        SetFlip( SIDE direction, bool updateGrids );
    void        SetPlayer( SIDE side );
    void        SetDefendMode( bool defend );
    void        SetHit( Unit* attacker );
    void        SetWaiting( bool wait = true );
    void        SetActiveState();
    void        DisableCurrentAnimation();

    bool        IsInGoodShotRange( Unit* target )const;
    bool        IsInAttackRange( vector<Grid*> possiblePositions, bool shootingRange )const;
    bool        IsIdle()const;

private:
    void        ModulateAlpha();

    STATE::Type m_State = STATE::STAND;
    TYPE::Name m_Type;
    SIDE m_Player;
    uint m_Size;
    SDL_Point m_Dimentions;

    SIDE m_IsHit = SIDE::NONE;
    bool m_CanHitBack = true;
    bool m_DefenseMode = false;
    bool m_Waiting = false;
    Uint32 m_ActiveUnitAlpha = 255;

    AnimationSetPtr m_Animations;
    Unit* m_AttackedUnit = nullptr;
    Grid* m_CurGrid[2] = { nullptr, nullptr };
    Statistics m_Stats;
    StatsInfoBoardPtr m_StatsBoard = nullptr;
    ShotObjectPtr m_Shot;
    vector<Grid*> m_Route;
    ImagePtr m_StandingUnitImg;
    AnimationController m_AnimationController;
};

#endif