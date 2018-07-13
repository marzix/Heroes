#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

#include "Dialog.h"
#include "Image.h"
#include "Cursor.h"
#include "ShotObject.h"
#include "AI.h"

class GameManager
{
public:
    GameManager();
    ~GameManager();

    static GameManager &    GetSingleton();
    float                   GetDeltaTime()const;
    SDL_Point const &       GetMousePos()const;
    VisibleObject*          GetMouseOn();
    bool                    IsMouseOn( VisibleObject* pObj );
    bool                    WasMouseButtonPressed( Uint32 button = 0 );
    void                    ResetMouseOn();
    Dialog *                GetScreen()const;
    const SDL_Event         GetEvent()const;
    AnimationSetPtr         GetSet( string name );
    UnitPtr                 GetActiveUnit();
    bool                    ActiveUnitChanged();
    void                    SetActiveUnitInDefendMode();
    void                    SetActiveUnitWait();

    void InitAI( Player* myPlayer, Player* enemy, BattleScreen* screen );
    bool IsPlayerAI( SIDE playerSide )const;
    void CallAI( Unit* activeUnit );

    void SetTime();
    void SetScreen( Dialog * pCurrent );
    void SetEvent();
    void SetMouseOn();
    void SetCursorDefault();
    void SetCursorTexture( STATE::Type state, TYPE::Name type, bool inRange );
    void SetCursorRotation( double rotation );
    void SetActiveUnit();
    void SetRoundTime( int t );
    bool RoundIsOver();
    
    bool    Initialize( int winPosX, int winPosY, Uint32 windowFlag = SDL_WINDOW_SHOWN );
    void    ScreenInit();
    void    CursorInit();
    void    ResetEvent();
    Dialog* FindScreen( string n );
    Image*  FindImage( string name );
    void    CheckMousePos();
    void    RenderCursor();

    void SetLoadingScreen();
    bool LoadFromFile( const string& fileName );
    void LoadTextures( rapidxml::xml_node<char> * node );
    void LoadAnimations( rapidxml::xml_node<char> * node );
    void SaveMusicFileNames( string screenName, string soundtrackFile );

    void        AddToRenderQueue( VisibleObjectPtr & t );
    void        SortRenderQueue();
    void        RenderQueue();
    void        ResetRenderQueue();
    static bool CompareHeight( const VisibleObjectPtr & first, const VisibleObjectPtr & second );

    void        AddToGameQueue( UnitPtr & unit );
    void        SortGameQueue();
    void        ResetGameQueue();
    static bool CompareSpeed( const UnitPtr & first, const UnitPtr & second );
    bool        IsGameQueueEmpty();

    void        AddNode( Node & n );
    void        BindNodes();
    void        ResetNodes(); 
    NodePtr     FindNode( Grid * g );
    Grid*       GetSecondUnitGrid( Grid* first, Grid* second );
    void        ResetGrids();

    void            AddShotObject( ShotObjectPtr shot );
    ShotObjectPtr   FindShotObject( string& unitName );

    SDL_Window * win = nullptr;
    SDL_Renderer * ren = nullptr;
    
private:
    SDL_Event m_Event;
    float m_DeltaTime;
    Uint32 m_LastGameTime = 0;
    vector<Image*> m_GameImages;
    vector<AnimationSetPtr> m_Animations;
    Dialog * m_CurrentScreen; 
    Cursor * m_Cursor;
    vector<VisibleObjectPtr> m_RenderQueue;
    vector<NodePtr> m_AllNodes;
    vector<Dialog*> m_ScreenList;
    vector<UnitPtr> m_GameQueue;
    vector<ShotObjectPtr> m_ShotObjects;
    float m_RoundTime;
    float m_CurRoundTime;
    bool m_UnitChanged = false;
    AI m_ComputerPlayer;

    static GameManager Game;
};

#endif