#include "Node.h"
#include "GameManager.h"

void Node::Reset()
{
    cost = 0;
    IsOnOpen = false;
    pPrev.reset();
}

NodePtr FindRoute( Grid ** from, Grid * to, int movepoints, TYPE::Name unitType )
{
    vector<NodePtr> OpenList;
    if( !from || !from[0] )
        return nullptr;
    auto & Game = GameManager::GetSingleton();
    NodePtr pStart1 = Game.FindNode( from[0] );
    NodePtr pStart2 = Game.FindNode( from[1] );

    Game.ResetNodes();
    pStart1->pGrid->SetRange( true );
    OpenList.push_back( pStart1 );
    pStart1->IsOnOpen = true;
    if( pStart2 )
    {
        pStart2->pGrid->SetRange( true );
        OpenList.push_back( pStart2 );
        pStart2->IsOnOpen = true;
    }
    NodePtr dest;
    while( OpenList.size() )
    {
        auto pCur = OpenList[0];
        OpenList.erase( OpenList.begin() );
        if( !pCur || !pCur->pGrid || !pCur->pGrid->InRange() )
            continue;

        if( pCur->pGrid == to )
        {
            if( !dest || dest->cost > pCur->cost )
                dest = pCur;
        }

        if( pCur->cost == movepoints )
           continue;

        for( auto & neighbour : pCur->neighbours )
        {
            if( !neighbour || neighbour->IsOnOpen )
                continue;

            if( unitType != TYPE::FLYING && neighbour->pGrid->GetOccupied() )
            {
                if( pCur->cost + 1 <= movepoints )
                    neighbour->pGrid->SetRange( true );
                continue;
            }

            neighbour->pPrev = pCur;
            neighbour->cost = pCur->cost + 1;
            neighbour->IsOnOpen = true;
            if( neighbour->cost <= movepoints )
                neighbour->pGrid->SetRange( true );

            OpenList.push_back( neighbour );
        }
    }
    return dest;
}