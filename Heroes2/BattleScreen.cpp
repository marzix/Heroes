#include "BattleScreen.h"
#include "GameManager.h"
#include "StatsInfoBoard.h"
#include "AnimationSet.h"
#include "EndScreen.h"
#include "Object.h"
#include "Image.h"
#include "Unit.h"
#include "Node.h"
#include "AudioManager.h"

#pragma warning(disable : 4996)

BattleScreen::BattleScreen( string name )
{
    m_Name = name;
}

BattleScreen::~BattleScreen()
{}

void BattleScreen::Initialize()
{   
    auto & Game = GameManager::GetSingleton();
    m_Previous = Game.FindScreen( "exitB" );
    m_Next = Game.FindScreen( "endBattle" );
    SetBackground();

    if( Image* toolbar =  Game.FindImage( "toolbar" ) )
    {
        m_ToolBar = make_shared<Image>( *toolbar );
        m_ToolBar->SetRect( 0, eWindow::HEIGHT - m_ToolBar->GetRect().h );
    }

    Image* gridTex = Game.FindImage( "grid" );
    Image* gridRangeTex = Game.FindImage( "gridInRange" );
    if( gridTex && gridRangeTex )
    {
        SDL_Rect gridRect = gridTex->GetRect();
        int x = 60, y = 80;
        int i = 0, j = 0, k = 1;
        for( i = 0; i < GRID_ROW; i++ )
        {
            for( j = 0; j < GRID_COLUMN; j++ )
            {
                m_Map[i][j] = Grid( gridTex, gridRangeTex, x, y, i, j );
                x += gridRect.w;
                Node n;
                n.pGrid = &m_Map[i][j];
                Game.AddNode( n );
            }
            if( i % 2 == 0 )
                k = 1;
            else
                k = -1;

            y += gridRect.h * 3 / 4 + 2;
            x = m_Map[i][0].GetImg().GetRect().x + ( gridRect.w / 2 + 1 ) * k;
        }
        Game.BindNodes();
    }
}

void BattleScreen::Reset()
{
    m_ObjList.clear();
    ResetGrids();
    auto & Game = GameManager::GetSingleton();
    Game.ResetGameQueue();

    StatsInfoBoard::InitializeHeaders();
    SDL_Point heroPosition;
    heroPosition.x = 40;
    heroPosition.y = 60;
    InitPlayer( heroPosition, 0 );
    heroPosition.x = 760;
    InitPlayer( heroPosition, 1 );

    SDL_Rect r = m_ToolBar->GetRect();
    m_ObjList.push_back( make_shared<Button>( r.x, r.y, make_shared<Image>( Game.FindImage( "buttonMenu1" ) ), 
        nullptr, nullptr,
        make_shared<Image>(  Game.FindImage( "buttonMenu2" ) ), 
        PreviousScreen ) );
    r.x += r.w - 52; 
    m_ObjList.push_back( make_shared<Button>( r.x, r.y, make_shared<Image>( Game.FindImage( "buttonDefend1" ) ), 
        nullptr, nullptr,
        make_shared<Image>(  Game.FindImage( "buttonDefend2" ) ), 
        SetUnitDefend ) );

    r.x -= 52;
    m_ObjList.push_back( make_shared<Button>( r.x, r.y, make_shared<Image>( Game.FindImage( "buttonWait1" ) ), 
        nullptr, make_shared<Image>( Game.FindImage( "buttonWait3" ) ),
        make_shared<Image>(  Game.FindImage( "buttonWait2" ) ), 
        SetUnitWaiting ) );
    m_WaitButton = dynamic_cast<Button*>( m_ObjList.back().get() );

    for( auto& obj : m_ObjList )
        obj->Initialize();
    PlayRandomSoundtrack();
    if( m_Player[1] && m_Player[1]->IsAI() )
        Game.InitAI( m_Player[1], m_Player[0], this );
}

void BattleScreen::Actualize()
{
    auto & Game = GameManager::GetSingleton();
    SDL_Event e = Game.GetEvent();
    if( Quit( e ) )
    {
        m_IsActive = false;
        return;
    }
    if( GameFinished() )
    {
        SDL_Delay( 2000 );
        SetEndScreenPlayers();
        NextScreen();
        return;
    }
    Game.ResetRenderQueue();
    if( Game.IsGameQueueEmpty() )
        PrepareGameQueue();
    auto unit = Game.GetActiveUnit();
    if( unit )
    {
        if( unit->GetState() != STATE::ACTIVE )
        {
            Game.ResetNodes();
            Game.ResetGrids();
        }
    }
    if( Game.ActiveUnitChanged() )
    {
        Game.ResetNodes();
        Game.ResetGrids();
        auto newUnit = Game.GetActiveUnit().get();
        ActivateUnit( newUnit );
        ChangeButtonWaiting( !newUnit->GetWaiting() );
    }
    for( unsigned int i = 0; i < m_ObjList.size(); i++ )
    {
        Game.SetMouseOn();
        m_ObjList[i]->Actualize();
        Game.AddToRenderQueue( m_ObjList[i] );
    }
    if( !AudioManager::GetSingleton().IsPlaying() )
        PlayRandomSoundtrack();
}

void BattleScreen::Render()
{    
    auto & Game = GameManager::GetSingleton(); 
    Game.SortRenderQueue();

    //rendering background texture
    VisibleObject::Render();

    for( int i = 0; i < GRID_ROW; i++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            m_Map[i][j].Render();
        }
    }
    m_ToolBar->RenderTexture();
    m_Player[0]->RenderHero();
    m_Player[1]->RenderHero();
    Game.RenderQueue();
}

const BattleScreen & BattleScreen::operator+=( VisibleObjectPtr obj )
{
    m_ObjList.push_back( obj );
    return *this;
}

void BattleScreen::PrepareGameQueue()
{
    auto & Game = GameManager::GetSingleton();
    m_Player[0]->AddToGameQueue();
    m_Player[1]->AddToGameQueue();
    Game.SortGameQueue();
    Game.SetActiveUnit();
}

SDL_Point BattleScreen::GetGridCenterPos( int row, int col )
{
    SDL_Point p;
    SDL_Rect r = m_Map[row][col].GetImg().GetRect();
    p.x = r.x + r.w / 2;
    p.y = r.y + r.h / 2;
    return p;
}

Grid * BattleScreen::GetGrid( int row, int col )
{
    return &m_Map[row][col];
}

Grid * BattleScreen::GetPreviousGrid( Unit * u )
{
    for( int i = 0; i < GRID_ROW; i++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            if( u == m_Map[i][j].GetUnit() && j > 0 )
            {
                return  &m_Map[i][j - 1];
            }
        }
    }
    return nullptr;
}

Grid * BattleScreen::GetPreviousNextGrid( Grid * g, bool prev )
{
    if( g )
    {
        int i = g->GetRow(), j = g->GetCol();
        if( prev && j > 0 )
            return  &m_Map[i][j-1];
        else if( !prev && j != GRID_COLUMN - 1 )
             return &m_Map[i][j+1];         
    }
    return nullptr;
}

void BattleScreen::SetUnitOnGrid( Unit * u, Grid * g )
{
    if( g )
    {
        int i = g->GetRow(), j = g->GetCol();
        m_Map[i][j].SetUnit( u );
        if( u->GetSize() > 1 )
        {
            if( u->GetFlip() == SDL_FLIP_HORIZONTAL )
            {
                m_Map[i][j+1].SetUnit( u );
            }
            else
            {
                m_Map[i][j-1].SetUnit( u );
            }
        }
    }    
}

void BattleScreen::SetBackground()
{
    auto & Game = GameManager::GetSingleton();
    int texNumber = rand() % 5;
    char buffer [33];
    sprintf( buffer , "%d", texNumber );
    string str = buffer;
    string texName = "battlefield" + str;
    if( Image* img = Game.FindImage( texName ) )
        m_CurImage = make_shared<Image>( *img );
}

void BattleScreen::SetPlayers( Player* players[2] )
{
    if( players && players[0] && players[1] )
    {
        m_Player[0] = players[0];
        m_Player[1] = players[1];
    }
}

void BattleScreen::InitPlayer( SDL_Point heroPosition, int index )
{
    if( !m_Player )
        return;
    auto& Game = GameManager::GetSingleton();
    if( auto hero = m_Player[index]->GetHero() )
    {
        hero->SetPosition( heroPosition.x, heroPosition.y );
        string colorName = PLAYER_COLOR::ToString( m_Player[index]->GetColor() );
        if( AnimationSetPtr tmpAnim = Game.GetSet( colorName ) )
            m_Player[index]->GetHero()->SetFlagAnimation( tmpAnim->Find( STATE::ToString( STATE::STAND ) ) );
        m_Player[index]->GetHero()->SetFlip( index == 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL );
        m_ObjList.push_back( m_Player[index]->GetHero() );
    }
    m_Player[index]->SetSide( index == 0 ? SIDE::LEFT : SIDE::RIGHT );
    m_Player[index]->SetUnits( this );
}

void BattleScreen::AddToObjectList( VisibleObjectPtr obj )
{
    m_ObjList.push_back( obj );
}

void BattleScreen::ChangeButtonWaiting( bool enabled )
{
    if( enabled )
        m_WaitButton->SetEnabled();
    else
        m_WaitButton->SetDisabled();
}

VisibleObject* BattleScreen::FindObject( const string name )
{
    for( auto& obj : m_ObjList )
    {
        if( obj && name.compare( obj->GetName() ) == 0 )
            return obj.get();
    }
    return nullptr;
}

void BattleScreen::ActivateUnit( Unit* unit )
{
    if( !unit )
        return;

    if( Grid** unitGrids = unit->GetGrid() )
    {
        FindRoute( unitGrids, nullptr, unit->GetSpeed(), unit->GetType() );
        uint unitSize = unit->GetSize();
        if( unitSize == 1 )
            return;
        for( int i = 0; i < GRID_ROW; i++ )
        {
            uint freeGrids = 0;
            for( int j = 0; j < GRID_COLUMN; j++ )
            {
                if( ( !m_Map[i][j].GetOccupied() || m_Map[i][j].GetUnit() == unit ) && m_Map[i][j].InRange() && j != GRID_COLUMN - 1 )
                    freeGrids++;
                else
                {
                    if( freeGrids < unitSize && j - 1 >= 0 )
                    {
                        if( j == GRID_COLUMN - 1 && ( !m_Map[i][j-1].GetOccupied() || m_Map[i][j-1].GetUnit() == unit ) )
                            continue;
                        else if( !m_Map[i][j].GetOccupied() )
                            m_Map[i][j].SetRange( false );
                        if( j - 2 >= 0 && m_Map[i][j-2].GetUnit() == unit )
                            continue;
                        if( !m_Map[i][j-1].GetOccupied() )
                            m_Map[i][j-1].SetRange( false );
                    }
                    freeGrids = 0;
                }                
            }
        }
    }
}

void BattleScreen::ClearUnitFromGrids( Unit * u )
{
    for( int i = 0; i < GRID_ROW; i++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            if( m_Map[i][j].GetUnit() == u )
            {
                m_Map[i][j].SetOccupied( false );
            }
        }
    }    
}

Unit * BattleScreen::GetUnitOnGrid( int row, int col )
{
    return m_Map[row][col].GetUnit();
}

Grid * BattleScreen::FindGrid( Unit * u )
{
    for( int i = 0; i < GRID_ROW; i++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            if( u == m_Map[i][j].GetUnit() )
            {
                return  &m_Map[i][j];
            }
        }
    }
    return nullptr;
}

Grid * BattleScreen::FindGrid( int x, int y )
{
    SDL_Rect r = m_Map[0][0].GetImg().GetRect();
    for( int i = 0; i < GRID_ROW; i ++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            if( x >= m_Map[i][j].GetImg().GetRect().x && x < m_Map[i][j].GetImg().GetRect().x + r.w &&
                y >= m_Map[i][j].GetImg().GetRect().y && y < m_Map[i][j].GetImg().GetRect().y + r.h )
            {
                return &m_Map[i][j];
            }
        }
    }
    return nullptr;
}

vector<Grid *> BattleScreen::FindGrids( Unit * u )
{
    vector<Grid *> Ret;
    for( int i = 0; i < GRID_ROW; i++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            if( u == m_Map[i][j].GetUnit() )
            {
                Ret.push_back( &(m_Map[i][j]) );
            }
        }
    }
    return Ret;
}

template< int Count >
void InitTbl( int (&DST)[Count] , const int (&SRC)[Count] )
{
    for( int i = 0 ; i < Count ; ++i )
        DST[i] = SRC[i];
}

vector<Grid*> BattleScreen::GetAllNeighbours( Unit * u )
{
    vector<Grid*> neighbours;
    if( !u )
        return neighbours;
    Grid** g = u->GetGrid();
    if( !g || ( !g[0] && !g[1] ) )
        return neighbours;
    int row = g[0]->GetRow(), col = g[0]->GetCol();
    int columns[9];
    int columnCount = 2;
    if( u->GetSize() == 2 )
    {
        if( g[1] && g[1]->GetCol() < g[0]->GetCol() )
            col = g[1]->GetCol();
        columnCount = 3;
        if( row % 2 )
        {
            InitTbl( columns , { col,    col+1,  col+2 
                               , col-1,  -1,     col+2
                               , col+2,    col+1,  col } );
        }
        else
        {
            InitTbl( columns , { col-1, col, col+1 
                               , col-1, -1,  col+2
                               , col+1, col, col-1 } );
        }
    }
    else
    {
        if( row % 2 )
        {
            InitTbl( columns , { col,    col+1,  -1 
                               , col-1,  col+1,  -1
                               , col+1,    col,  -1 } );
        }
        else
        {
            InitTbl( columns , { col-1, col,    -1 
                               , col-1, col+1,  -1
                               , col, col-1,    -1 } );
        }
    }
    row -= 1;
    int lastGrid = 0;
    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < columnCount; j++ )
        {
            int colIndex = columns[i*3 + j];
            if( i == 1 )
            {
                if( j == 1 && u->GetSize() == 2 )
                    continue;
                if( j == 0 )
                {
                    lastGrid = colIndex;
                    continue;
                }
            }
            if( colIndex < 0 || colIndex >= GRID_COLUMN || i + row >= GRID_ROW || i + row < 0 )
                neighbours.push_back( nullptr );
            else
                neighbours.push_back( &m_Map[i+row][colIndex] );
        }
    }
    if( lastGrid < 0 || lastGrid >= GRID_COLUMN )
        neighbours.push_back( nullptr );
    else
        neighbours.push_back( &m_Map[row+1][lastGrid] );
    return neighbours;
}

void BattleScreen::ResetGrids()
{
    for( int i = 0; i < GRID_ROW; i ++ )
    {
        for( int j = 0; j < GRID_COLUMN; j++ )
        {
            m_Map[i][j].SetOccupied( false );
        }
    }
}

bool BattleScreen::GameFinished()const
{
    if( !m_Player[0] || !m_Player[1] )
        return true;
    return m_Player[0]->IsDefeated() || m_Player[1]->IsDefeated();
}

void BattleScreen::SetEndScreenPlayers()
{
    if( EndScreen* eScreen = dynamic_cast<EndScreen*>( m_Next ) )
        eScreen->SetPlayers( m_Player );
}

void BattleScreen::PlayRandomSoundtrack()
{
    int texNumber = rand() % 4 + 1;
    char buffer[10];
    sprintf( buffer , "%d", texNumber );
    string str = buffer;
    string soundtrack = m_SoundTrackName + str;
    AudioManager::GetSingleton().PlayTrack( soundtrack );
}
