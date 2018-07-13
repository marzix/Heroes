#include "AI.h"
#include "Unit.h"
#include "Grid.h"
#include "BattleScreen.h"
#include "GameManager.h"
#include "Node.h"
#include <algorithm>

AI::AI()
{
}

void AI::SetPlayers( Player* myPlayer, Player* enemy )
{
    m_Player = myPlayer;
    m_Enemy = enemy;
    m_Activated = true;
}

void AttackOption::CalculateScore()
{
    if( damageReceived.totalDamage )
        damageScore = damageGiven.totalDamage / (float)damageReceived.totalDamage;
    else
        damageScore = (float)damageGiven.totalDamage;
    if( damageReceived.unitsKilled )
        unitsKilledScore = damageGiven.unitsKilled / (float)damageReceived.unitsKilled;
    else
        unitsKilledScore = (float)damageGiven.unitsKilled;
    if( enemy->GetType() == TYPE::ARCHER )
        unitsKilledScore += 3;
}

vector<AttackOption> AI::FindAttackOptions()
{
    vector<UnitPtr> targets;
    vector<AttackOption> options;
    if( !m_ActiveUnit )
        return options;
    if( m_Enemy )
        targets = m_Enemy->GetAllUnits();
    bool archer = m_ActiveUnit->GetType() == TYPE::ARCHER;
    archer &= m_ActiveUnit->IsInAttackRange( m_CurrentScreen->GetAllNeighbours( m_ActiveUnit ), true );
    for( auto enemy : targets )
    {
        if( enemy && enemy->IsActive() )
        {
            vector<Grid*> neighbours = m_CurrentScreen->GetAllNeighbours( enemy.get() );
            bool skip = true;
            for( auto g : neighbours )
            {
                if( g && ( !g->GetOccupied() || g->GetUnit() == m_ActiveUnit ) )
                {
                    skip = false;
                    break;
                }
            }
            if( skip )
                continue;
            AttackOption temp;
            temp.enemy = enemy.get();
            bool noLoss = archer || enemy->GetDefendMode() || !enemy->CanHitBack();
            
            temp.damageGiven = CountDamage( m_ActiveUnit, m_ActiveUnit->GetQuantity(), archer, enemy.get() );
            if( noLoss == false )
                temp.damageReceived = CountDamage( enemy.get(), enemy->GetQuantity() - temp.damageGiven.unitsKilled, false, m_ActiveUnit );
            temp.CalculateScore();
            if( InRange( enemy.get(), m_ActiveUnit, 0 ) )
                temp.canEnemyReachMe = true;
            else
                temp.canEnemyReachMe = false;
            NodePtr dest = nullptr;
            if( dest = InRange( m_ActiveUnit, enemy.get(), 0 ) )
            {
                temp.route = SetRoute( dest );
                options.push_back( temp );
            }
            else if( dest = InRange( m_ActiveUnit, enemy.get(), 50 ) )
            {
                temp.route = SetRoute( dest );
                options.push_back( temp );
            }
        }
    }
    sort( options.begin(), options.end(), CompareAttackOptions );
    return options;
}
//
//Grid* AI::Move()
//{
//    vector<UnitPtr> targets;
//    AttackOption bestOption;
//    Grid* dest = nullptr;
//    if( !m_ActiveUnit )
//        return dest;
//    if( m_Enemy )
//        targets = m_Enemy->GetAllUnits();
//    for( auto enemy : targets )
//    {
//        if( enemy && enemy->IsActive() )
//        {
//            AttackOption temp;
//            temp.enemy = enemy.get();
//
//            temp.damageGiven = CountDamage( m_ActiveUnit, m_ActiveUnit->GetQuantity(), false, enemy.get() );
//            temp.CalculateScore();
//            if( !bestOption.enemy )
//                bestOption = temp;
//            else
//            {
//                if( bestOption.unitsKilledScore < temp.unitsKilledScore )
//                    bestOption = temp;
//                else if( bestOption.unitsKilledScore == temp.unitsKilledScore )
//                {
//                    if( bestOption.damageScore < temp.damageScore )
//                        bestOption = temp;
//                }
//            }
//        }
//    }
//    if( !bestOption.enemy )
//        return dest;
//    Grid** tempGridsEnemy = bestOption.enemy->GetGrid();
//    Grid** tempGridsUnit = m_ActiveUnit->GetGrid();
//    if( tempGridsEnemy && tempGridsEnemy[0] && tempGridsUnit && tempGridsUnit[0] )
//    {
//
//    }
//}

vector<Grid*> AI::DefendArcher()
{
    vector<UnitPtr> units;
    vector<Grid*> availableGrids;
    if( !m_ActiveUnit )
        return availableGrids;
    if( m_Player )
        units = m_Player->GetAllUnits();
    for( auto unit : units )
    {
        if( unit && unit->IsActive() && unit->GetType() == TYPE::ARCHER && InRange( m_ActiveUnit, unit.get() ) )
        {
            vector<Grid*> temp = m_CurrentScreen->GetAllNeighbours( unit.get() );
            for( auto g : temp )
            {
                if( g && g->GetUnit() == m_ActiveUnit )
                {
                    availableGrids.clear();
                    return availableGrids;
                }
                if( g && g->InRange() && !g->GetOccupied() )
                    availableGrids.push_back( g );
            }
        }
    }
    return availableGrids;
}

Damage AI::Wait()
{
    vector<UnitPtr> enemies;
    Damage worstOption;
    if( !m_ActiveUnit )
        return worstOption;
    if( m_Enemy )
        enemies = m_Enemy->GetAllUnits();
    
    for( auto enemy : enemies )
    {
        if( !enemy || !enemy->IsActive() )
            continue;

        bool archer = enemy->GetType() == TYPE::ARCHER;
        archer &= enemy->IsInAttackRange( m_CurrentScreen->GetAllNeighbours( enemy.get() ), true );
        if( InRange( enemy.get(), m_ActiveUnit ) || archer )
        {
            Damage temp = CountDamage( enemy.get(), enemy->GetQuantity(), archer, m_ActiveUnit );
            
            if( temp.totalDamage > worstOption.totalDamage )
                worstOption = temp;
        }
    }
    return worstOption;
}

void AI::SetActiveUnit( Unit* unit )
{
    m_ActiveUnit = unit;
}

NodePtr AI::InRange( Unit* unit, Unit* enemy, uint movepoints )
{
    if( !enemy )
        return nullptr;
    vector<NodePtr> routes;
    vector<Grid*> grids = m_CurrentScreen->GetAllNeighbours( enemy );
    for( auto g : grids )
    {
        if( g )
        {
            if( NodePtr route = unit->InRange( g, movepoints ) )
                routes.push_back( route );
        }
    }
    if( routes.empty() )
        return nullptr;
    int index = rand() % routes.size();
    return routes[index];
}

vector<Grid*> AI::SetRoute( NodePtr last )
{
    Grid** curGrid = m_ActiveUnit->GetGrid();
    vector<Grid*> route;
    while( last && last->pGrid != curGrid[0] && last->pGrid != curGrid[1] )
    {
        route.push_back( last->pGrid );
        last = last->pPrev;
    }
    return route;
}

bool AI::IsActivated()const
{
    return m_Activated;
}

Damage AI::CountDamage( Unit* attacker, int attackerQuantity, bool canShoot, Unit* enemy )
{
    Damage damage;
    bool archer = attacker->GetType() == TYPE::ARCHER;
    damage.totalDamage = attacker->GetAttack() * attackerQuantity;
    if( archer && ( !canShoot || !attacker->IsInGoodShotRange( enemy ) ) )
        damage.totalDamage = damage.totalDamage * 2 / 3;

    if( enemy->GetDefendMode() )
        damage.totalDamage -= enemy->GetDefense();
    else
        damage.totalDamage -= ( rand() * enemy->GetDefense() % 100 ) / 100;
    damage.unitsKilled = (int)( floor(  damage.totalDamage / enemy->GetLifePerUnit() ) );
    return damage;
}

void AI::MakeDecision()
{
    auto& Game = GameManager::GetSingleton();
    if( !m_ActiveUnit || !m_CurrentScreen )
        return;
    Grid* destinationGrid = nullptr; 
    vector<AttackOption> options = FindAttackOptions();
    vector<UnitPtr> army = m_Player->GetAllUnits();
    Damage worstDamage = Wait();
    Unit* chosenEnemy = nullptr;
    bool archer = m_ActiveUnit->GetType() == TYPE::ARCHER;
    if( archer )
        archer = m_ActiveUnit->IsInAttackRange( m_CurrentScreen->GetAllNeighbours( m_ActiveUnit ), true );
    if( !options.empty() && !options[0].canEnemyReachMe )
    {
        bool enemyHasArcher = false;
        for( auto& e : options )
        {
            if( e.enemy && e.enemy->GetType() == TYPE::ARCHER )
            {
                enemyHasArcher = true;
                break;
            }
        }
        if( !enemyHasArcher )
        {
            for( auto& u : army )
            {
                if( u && u->GetType() == TYPE::ARCHER )
                { 
                    if( m_ActiveUnit->GetWaiting() == false )
                        Game.SetActiveUnitWait();
                    else
                        Game.SetActiveUnitInDefendMode();
                    return;
                }
            }
        }
        if( archer )
        {
            AttackOption choice = GetBestInRangeOption( options, archer );
            chosenEnemy = choice.enemy;
        }
        else if( m_ActiveUnit->GetWaiting() == false && worstDamage.unitsKilled < 0.3 * m_ActiveUnit->GetQuantity() )
        {
            if( options.size() != 1 || options[0].enemy->GetType() != TYPE::ARCHER )
            {
                Game.SetActiveUnitWait();
                return;
            }
        }
        if( !chosenEnemy )
        {
            if( options[0].route.size() <= m_ActiveUnit->GetSpeed() )
                chosenEnemy = options[0].enemy;
            else
            {
                int index = options[0].route.size() - options[0].route.size() % m_ActiveUnit->GetSpeed();
                destinationGrid = options[0].route[index];
            }
        }
    }
    else if( archer )
    {
        chosenEnemy = options[0].enemy;
    }
    else if( options.size() == 1 || army.size() == 1 )
    {
        if( options[0].route.size() <= m_ActiveUnit->GetSpeed() )
            chosenEnemy = options[0].enemy;
        else
        {
            int index = options[0].route.size() - m_ActiveUnit->GetSpeed() - 1;
            destinationGrid = options[0].route[index];
        }
    }
    else if( options[0].route.size() <= m_ActiveUnit->GetSpeed() )
    {
        AttackOption choice = GetBestInRangeOption( options, archer );
        chosenEnemy = choice.enemy;
    }
    else
    {
        if( m_ActiveUnit->GetWaiting() == false && worstDamage.unitsKilled < 0.3 * m_ActiveUnit->GetQuantity() )
        {
            Game.SetActiveUnitWait();
            return;
        }
        else
        {
            vector<Grid*> grids = DefendArcher();
            if( grids.empty() )
            {
                Game.SetActiveUnitInDefendMode();
                return;
            }
            else
            {
                int index = rand() % grids.size();
                if( NodePtr dest = m_ActiveUnit->InRange( grids[index] ) )
                {
                    m_ActiveUnit->SetRoute( dest );
                    m_ActiveUnit->DisableCurrentAnimation();
                }
            }
        }
    }
    if( chosenEnemy )
    {
        NodePtr dest = nullptr;
        if( !archer )
        {
            vector<Grid*> neighbours = m_CurrentScreen->GetAllNeighbours( chosenEnemy );
            vector<Grid*> grids;
            for( uint i = 0; i < neighbours.size(); i++ )
            {
                if( !neighbours[i] )
                    continue;
                if( dest = m_ActiveUnit->InRange( neighbours[i] ) )
                {
                    grids.push_back( neighbours[i] );
                }
            }
            if( grids.size() )
            {
                int index = rand() % grids.size();
                destinationGrid = grids[index];
            }
        }
    }
    if( destinationGrid )
    {
        if( NodePtr dest = m_ActiveUnit->InRange( destinationGrid ) )
        {
            m_ActiveUnit->SetRoute( dest );
            m_ActiveUnit->DisableCurrentAnimation();
        }
    }
    if( chosenEnemy )
    {
        m_ActiveUnit->DisableCurrentAnimation();
        m_ActiveUnit->SetAtteckedUnit( chosenEnemy );
    }
}

void AI::SetCurrentScreen( BattleScreen* current )
{
    m_CurrentScreen = current;
}

AttackOption AI::GetBestInRangeOption( vector<AttackOption>& options, bool archer )
{
    AttackOption choice;
    for( auto& option : options )
    {
        if( archer || option.route.size() <= m_ActiveUnit->GetSpeed() )
        {
            if( !choice.enemy )
                choice = option;
            else if( option.unitsKilledScore >= choice.unitsKilledScore )
            {
                if( option.unitsKilledScore == choice.unitsKilledScore )
                {
                    if( option.damageReceived.unitsKilled < choice.damageReceived.unitsKilled )
                        choice = option;
                }
            }
        }
    }
    return choice;
}

bool AI::CompareAttackOptions( const AttackOption& first, const AttackOption& second )
{
    if( first.canEnemyReachMe != second.canEnemyReachMe )
        return first.canEnemyReachMe;
    if( first.route.size() != second.route.size() )
        return first.route.size() < second.route.size();
    if( first.unitsKilledScore != second.unitsKilledScore )
        return first.unitsKilledScore > second.unitsKilledScore;
    if( first.damageReceived.unitsKilled != second.damageReceived.unitsKilled )
        return first.damageReceived.unitsKilled < second.damageReceived.unitsKilled;
    return first.damageScore > second.damageScore;
}