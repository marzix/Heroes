#ifndef __AI_H__
#define __AI_H__

#include "Player.h"

struct Damage
{
    int totalDamage = 0;
    int unitsKilled = 0;
};

struct AttackOption
{
    AttackOption() = default;
    void CalculateScore();

    Unit* enemy = nullptr;
    Damage damageGiven;
    Damage damageReceived;
    float damageScore = 0;
    float unitsKilledScore = 0;
    vector<Grid*> route;
    bool canEnemyReachMe;
};

class AI
{
public:
    AI();
    void SetPlayers( Player* myPlayer, Player* enemy );
    vector<AttackOption> FindAttackOptions();
    vector<Grid*> DefendArcher();
    Damage Wait();
    //Grid* Move();
    void SetActiveUnit( Unit* unit );
    NodePtr InRange( Unit* unit, Unit* enemy, uint movepoints = 0 );
    vector<Grid*> SetRoute( NodePtr last );
    bool IsActivated()const;
    void MakeDecision();
    Damage CountDamage( Unit* attacker, int attackerQuantity, bool canShoot, Unit* enemy );
    void SetCurrentScreen( BattleScreen* current );
    AttackOption GetBestInRangeOption( vector<AttackOption>& options, bool archer );
    static bool CompareAttackOptions( const AttackOption& first, const AttackOption& second );

private:
    Player* m_Player = nullptr;
    Player* m_Enemy = nullptr;
    Unit* m_ActiveUnit = nullptr;
    BattleScreen* m_CurrentScreen;
    bool m_Activated = false;
};

#endif
