#ifndef __BATTLE_H__
#define __BATTLE_H__

#include "Grid.h"
#include "Dialog.h"
#include "Player.h"
#include "Button.h"

class BattleScreen: public Dialog
{
public:

    BattleScreen( string n );
    ~BattleScreen();

    void Initialize()override;
    void Reset()override;
    void Actualize()override;
    void Render()override;

    SDL_Point   GetGridCenterPos( int row, int col );
    Grid*       GetGrid( int row, int col );
    Unit*       GetUnitOnGrid( int row, int col );
    Grid*       GetPreviousGrid( Unit * u );
    Grid*       GetPreviousNextGrid( Grid * g, bool prev );
    void        SetUnitOnGrid( Unit * u, Grid * g );
    void        SetBackground();
    void        SetPlayers( Player* players[2] );
    void        InitPlayer( SDL_Point heroPosition, int index );
    void        AddToObjectList( VisibleObjectPtr obj );
    void        ChangeButtonWaiting( bool enabled );

    VisibleObject*  FindObject( const string name );
    void            ActivateUnit( Unit* unit );
    void            ClearUnitFromGrids( Unit * u );
    Grid*           FindGrid( Unit * u );
    Grid*           FindGrid( int x, int y );
    vector<Grid*>   FindGrids( Unit * u );
    vector<Grid*>   GetAllNeighbours( Unit * u );
    void            ResetGrids();
    
    bool        GameFinished()const;
    void        SetEndScreenPlayers();
    void        PlayRandomSoundtrack();

    const BattleScreen & operator+=( VisibleObjectPtr obj );

private:
    void PrepareGameQueue();

    Grid m_Map[GRID_ROW][GRID_COLUMN];
    vector<VisibleObjectPtr> m_ObjList;
    ImagePtr m_ToolBar;
    Player* m_Player[2];
    Button* m_WaitButton;
};

#endif