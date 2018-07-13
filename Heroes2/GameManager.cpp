#include <algorithm>
#include "GameManager.h"
#include "Object.h"
#include "Animation.h"
#include "AnimationSet.h"
#include "Image.h"
#include "Node.h"
#include "BattleScreen.h"
#include "Unit.h"
#include "OptionsScreen.h"
#include "MenuScreen.h"
#include "EndScreen.h"
#include "ExitScreen.h"
#include "CreditsScreen.h"

GameManager GameManager::Game;

GameManager::GameManager()
{
    m_ScreenList.push_back( new MenuScreen{ "MenuScreen" } );
    m_ScreenList.push_back( new CreditsScreen{ "credits" } );
    m_ScreenList.push_back( new ExitScreen{ "exitG" } );
    m_ScreenList.push_back( new OptionsScreen{ "OptionScreen" } );
    m_ScreenList.push_back( new ExitScreen{ "exitB" } );
    m_ScreenList.push_back( new BattleScreen{ "BattleScreen" } );
    m_ScreenList.push_back( new EndScreen{ "endBattle" } );
}

GameManager::~GameManager()
{
    SDL_DestroyRenderer( ren );
    ren = nullptr;
    SDL_DestroyWindow( win );
    win = nullptr;
    delete m_Cursor;
    for( auto & tmp: m_ScreenList )
    {
        delete tmp;
    }
    for( Image* img: m_GameImages )
        delete img;

    for( auto & node: m_AllNodes )
    {
        for( auto & neighb: node->neighbours )
        {
            neighb.reset();
        }
        node.reset();
    }
}

bool GameManager::Initialize( int winPosX, int winPosY, Uint32 windowFlag )
{
    win = SDL_CreateWindow( "Heroes", winPosX, winPosY, eWindow::WIDTH, eWindow::HEIGHT, SDL_WINDOW_SHOWN );
	if ( win == nullptr )
    {
		logSDLError( "CreateWindow" );
        return false;
    }
    ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if ( ren == nullptr )
    {
		logSDLError( "CreateRenderer" );
	    return false;
    }
    else
    {
        return true;
    }
}

void GameManager::ScreenInit()
{
    for( auto & screen: m_ScreenList )
    {
        screen->Initialize();
    }
    m_CurrentScreen = FindScreen( string( "MenuScreen" ) );
}

void GameManager::CursorInit()
{
    m_Cursor = new Cursor;
}

float GameManager::GetDeltaTime()const
{
    return m_DeltaTime;
}

Dialog * GameManager::GetScreen()const
{
    return m_CurrentScreen;
}

const SDL_Event GameManager::GetEvent()const
{
    return m_Event;
}

GameManager & GameManager::GetSingleton()
{
    return Game;
}

AnimationSetPtr GameManager::GetSet( string setName )
{
    for( auto & tmp: m_Animations )
    {
        if( tmp->GetName() == setName )
        {
            return tmp;
        }
    }
    return nullptr;
}

UnitPtr GameManager::GetActiveUnit()
{
    if( m_GameQueue.size() )
        return m_GameQueue.back();
    else return nullptr;
}

bool GameManager::ActiveUnitChanged()
{
    if( m_GameQueue.empty() )
        return false;
    m_CurRoundTime += m_DeltaTime;
    if( UnitPtr unit = m_GameQueue.back() )
    {
        if( unit->IsIdle() || ( unit->GetState() == STATE::DIE && unit->CurrentAnimationFinished() ) )
        {
            m_GameQueue.pop_back();
            if( m_GameQueue.empty() )
                return false;

            SetActiveUnit();
            if( m_GameQueue.empty() )
                return false;
            m_UnitChanged = false;
            return true;
        }
        else if( m_UnitChanged )
        {
            m_UnitChanged = false;
            return true;
        }
    }
    return false;
}

void GameManager::SetActiveUnitInDefendMode()
{
    if( m_GameQueue.back() )
    {
        m_GameQueue.back()->SetDefendMode( true );
        m_GameQueue.pop_back();
        SetActiveUnit();
    }
}

void GameManager::SetActiveUnitWait()
{
    if( m_GameQueue.back() )
    {
        m_GameQueue.back()->SetWaiting();
        SortGameQueue();
        SetActiveUnit();
    }
}

void GameManager::InitAI( Player* myPlayer, Player* enemy, BattleScreen* screen )
{
    m_ComputerPlayer.SetPlayers( myPlayer, enemy );
    m_ComputerPlayer.SetCurrentScreen( screen );
}

bool GameManager::IsPlayerAI( SIDE playerSide )const
{
    return playerSide == SIDE::RIGHT && m_ComputerPlayer.IsActivated();
}

void GameManager::CallAI( Unit* activeUnit )
{
    static float wait = 0;
    wait += m_DeltaTime;
    if( wait < 1.5 )
        return;
    wait = 0;
    m_ComputerPlayer.SetActiveUnit( activeUnit );
    m_ComputerPlayer.MakeDecision();
}

SDL_Point const & GameManager::GetMousePos()const
{
    return m_Cursor->GetMousePos();
}

void GameManager::SetTime()
{
    uint curTime = SDL_GetTicks();
    m_DeltaTime = ( curTime - m_LastGameTime ) * 0.001f;
    m_LastGameTime = curTime;
}

void GameManager::SetScreen( Dialog * pCurrent )
{
    if( m_CurrentScreen )
    {
        m_CurrentScreen->SetIsActive( false );
    }
    m_CurrentScreen = pCurrent;
    if( m_CurrentScreen )
    {
        m_CurrentScreen->SetIsActive( true );
    }
}

void GameManager::SetEvent()
{
    SDL_PollEvent( &m_Event );
}

void GameManager::SetMouseOn()
{
    if( m_Cursor )
        m_Cursor->SetMouseOn();
}

void GameManager::SetCursorDefault()
{
    if( m_Cursor )
        m_Cursor->SetDefault();
}

void GameManager::SetCursorTexture( STATE::Type state, TYPE::Name type, bool inRange )
{
    if( m_Cursor )
        m_Cursor->SetTexture( state, type, inRange );
}

void GameManager::SetCursorRotation( double rotation )
{
    if( m_Cursor )
        m_Cursor->SetRotation( rotation );
}

void GameManager::SetActiveUnit()
{
    m_CurRoundTime = 0;
    if( m_GameQueue.empty() )
        return;

    while( !m_GameQueue.empty() && m_GameQueue.back() && m_GameQueue.back()->GetState() == STATE::DIE )
        m_GameQueue.pop_back();
    if( m_GameQueue.empty() )
        return;

    m_GameQueue.back()->SetActiveState();
    m_UnitChanged = true;
}

void GameManager::SetRoundTime( int t )
{
    m_RoundTime = (float)t;
}

bool GameManager::RoundIsOver()
{
    if( m_RoundTime == 0 )
        return false;
    return m_CurRoundTime > m_RoundTime;
}

void GameManager::CheckMousePos()
{
    if( m_Cursor )
        m_Cursor->CheckMousePos();
}

VisibleObject* GameManager::GetMouseOn()
{
    if( m_Cursor )
        return m_Cursor->GetMouseOn();
    return nullptr;
}

bool GameManager::IsMouseOn( VisibleObject* pObj )
{
    return m_Cursor->IsMouseOn( pObj );
}

bool GameManager::WasMouseButtonPressed( Uint32 button )
{
    if( m_Cursor )
        return m_Cursor->WasButtonPressed( button ) > 0;
    return false;
}

void GameManager::ResetMouseOn()
{
    m_Cursor->SetMouseOn();
}

void GameManager::RenderCursor()
{
    if( m_Cursor )
        m_Cursor->RenderCursor();
}

void GameManager::ResetEvent()
{
    m_Event.key.keysym.sym = SDLK_UNKNOWN;
}

Image * GameManager::FindImage( string name )
{
    for( auto tmp : m_GameImages )
    {
        if( tmp && tmp->GetTexName() == name )
        {
            return tmp;
        }
    }
    return m_GameImages[0];
}

void GameManager::AddToRenderQueue( VisibleObjectPtr & t )
{
    m_RenderQueue.push_back( t );
}

bool GameManager::CompareHeight( const VisibleObjectPtr & first, const VisibleObjectPtr & second )
{
    if( !first || !second )
        return false;
    int y1 = first->GetHeightForSorting();
    int y2 = second->GetHeightForSorting();
    if(  y1 < y2 )
    {
        return true;
    }
    else if( y1 == y2 && second->IsActive() && !first->IsActive() )
    {
        return true;
    }
    return false;
}

void GameManager::SortRenderQueue()
{
    sort( m_RenderQueue.begin() , m_RenderQueue.end() , CompareHeight );
}

void GameManager::ResetRenderQueue()
{
    m_RenderQueue.clear();
}

void GameManager::RenderQueue()
{
    for( auto & obj: m_RenderQueue )
    {
        obj->Render();
    }
}

void GameManager::AddToGameQueue( UnitPtr & unit )
{
    m_GameQueue.push_back( unit );
}

void GameManager::SortGameQueue()
{
    sort( m_GameQueue.begin() , m_GameQueue.end() , CompareSpeed );
}

void GameManager::ResetGameQueue()
{
    m_GameQueue.clear();
}

bool GameManager::CompareSpeed( const UnitPtr & first, const UnitPtr & second )
{
    if( !first || !second )
        return false;
    if( first->GetSpeed() == second->GetSpeed() )
    {
        if( first->GetWaiting() != second->GetWaiting() )
            return first->GetWaiting();
        if( first->GetLifePerUnit() == second->GetLifePerUnit() )
        {
            if( first->GetDefense() != second->GetDefense() )
                return first->GetDefense() < second->GetDefense();
            if( first->GetAttack() != second->GetAttack() )
                return first->GetAttack() < second->GetAttack();
            return first->GetPlayer() == SIDE::LEFT;
        }
        else
            return first->GetLifePerUnit() < second->GetLifePerUnit();
    }
    else if( !first->GetWaiting() && !second->GetWaiting() )
        return first->GetSpeed() < second->GetSpeed();
    else if( first->GetWaiting() && second->GetWaiting() )
        return first->GetSpeed() > second->GetSpeed();
    else
        return first->GetWaiting();
}

bool GameManager::IsGameQueueEmpty()
{
    return m_GameQueue.empty();
}

void GameManager::AddNode( Node & n )
{
    m_AllNodes.push_back( make_shared<Node>( n ) );
}

void GameManager::ResetNodes()
{
    for( auto & pNode : m_AllNodes )
    {
        pNode->Reset();
    }
}

void GameManager::BindNodes()
{
    auto GetNode = [&]( int index, int xshift , int yshift )->NodePtr
    {
        int x = index % GRID_COLUMN;
        int y = index / GRID_COLUMN;
        x += xshift;
        y += yshift;
        if( x < 0 || x >= GRID_COLUMN || y < 0 || y >= GRID_ROW )
        {
            return NodePtr{};
        }
        return m_AllNodes[ y * GRID_COLUMN + x ];
    };
    for( int i = 0; i < (int)m_AllNodes.size(); i++ )
    {
        int y = i / GRID_COLUMN;
        if( y % 2 != 0 )
        {
            m_AllNodes[i]->neighbours[0] = GetNode( i, -1 ,  0 );
            m_AllNodes[i]->neighbours[1] = GetNode( i,  0 ,  1 );
            m_AllNodes[i]->neighbours[2] = GetNode( i,  1 ,  1 );
            m_AllNodes[i]->neighbours[3] = GetNode( i,  1 ,  0 );
            m_AllNodes[i]->neighbours[4] = GetNode( i,  1 , -1 );
            m_AllNodes[i]->neighbours[5] = GetNode( i,  0 , -1 );        
        }
        else
        {
            m_AllNodes[i]->neighbours[0] = GetNode( i, -1 ,  0 );
            m_AllNodes[i]->neighbours[1] = GetNode( i, -1 ,  1 );
            m_AllNodes[i]->neighbours[2] = GetNode( i,  0 ,  1 );
            m_AllNodes[i]->neighbours[3] = GetNode( i,  1 ,  0 );
            m_AllNodes[i]->neighbours[4] = GetNode( i,  0 , -1 );
            m_AllNodes[i]->neighbours[5] = GetNode( i, -1 , -1 );        
        }
    }
}

NodePtr GameManager::FindNode( Grid * g )
{
    for( auto & tmp: m_AllNodes )
    {
        if( tmp->pGrid == g )
        {
            return tmp;
        }
    }
    return nullptr;
}

Grid* GameManager::GetSecondUnitGrid( Grid* first, Grid* second )
{
    if( !first || !second )
        return nullptr;
    if( first == second )
        return nullptr;
    if( first->GetMiddle().y == second->GetMiddle().y && abs(first->GetMiddle().x - second->GetMiddle().x) <= (int)first->GetWidth() )
        return second;
    NodePtr firstNode = FindNode( first );
    SDL_Point fPoint = first->GetMiddle();
    SDL_Point sPoint = second->GetMiddle();
    for( auto & neighbour : firstNode->neighbours )
    {
        if( neighbour && neighbour->pGrid )
        {
            SDL_Point nPoint = neighbour->pGrid->GetMiddle();
            if( nPoint.y == fPoint.y )
            { 
                if( sPoint.x <= fPoint.x && nPoint.x <= fPoint.x )
                    return neighbour->pGrid;
                else if( sPoint.x > fPoint.x && nPoint.x > fPoint.x )
                    return neighbour->pGrid;
            }
        }
    }
    return nullptr;
}

void GameManager::ResetGrids()
{
    for( auto & pNode : m_AllNodes )
    {
        pNode->pGrid->SetRange( false );
    }
}

void GameManager::AddShotObject( ShotObjectPtr shot )
{
    m_ShotObjects.push_back( shot );
}

Dialog * GameManager::FindScreen( string n )
{
    for( auto & tmp: m_ScreenList )
    {
        if( tmp->GetName() == n )
        {
            return tmp;
        }
    }
    return nullptr;
}

ShotObjectPtr GameManager::FindShotObject( string& unitName )
{
    for( auto shotObj : m_ShotObjects )
    {
        if( shotObj && shotObj->GetName().compare( unitName ) == 0 )
            return shotObj;
    }
    return nullptr;
}

void GameManager::SetLoadingScreen()
{
    Texture loadingScreen = Texture{ loadingScreenFile, true, "loadingScreen" };
    SDL_Rect dst;
    dst.x = dst.y = 0;
    dst.w = eWindow::WIDTH;
    dst.h = eWindow::HEIGHT;
    loadingScreen.RenderTexture( &dst );
    SDL_RenderPresent( ren );
}

bool GameManager::LoadFromFile( const string& fileName )
{
    rapidxml::xml_document<> Doc;
	ifstream XmlFile{ fileName };
    Dialog * pDialog = nullptr;
	if( !XmlFile.is_open() )
    {
		return false;
    }
    SetLoadingScreen();
    string content((std::istreambuf_iterator<char>(XmlFile)), std::istreambuf_iterator<char>());
	XmlFile.close();

	Doc.parse<0>( &content[0] );
    auto RootNode = Doc.first_node();
    rapidxml::xml_node<char> * tempNode = nullptr;

	if( !RootNode )
    {
		return false;
    }

    while( RootNode )
    {
        string key = RootNode->name();
        if( key == string( "Castles" ) )
        {
            OptionsScreen * options = dynamic_cast<OptionsScreen*>( Game.FindScreen( string( "OptionScreen" ) ) );
            if( options )
            {
                options->SetCastles( RootNode->first_node() );
            }
        }
        else if( key == string( "Animations" ) )
        {
            tempNode = RootNode->first_node();
            LoadAnimations( tempNode );
        }
        else
        {
            if( auto _sound = RootNode->first_attribute( "sound" ) )
                SaveMusicFileNames( key, _sound->value() );
            tempNode = RootNode->first_node();
	        if( tempNode && tempNode->name() == string("texture") )
                LoadTextures( tempNode );
        }
        RootNode = RootNode->next_sibling();
    }
    return true;
}

void GameManager::LoadTextures( rapidxml::xml_node<char> * node )
{
    while( node )
    {
        string name, file;
        bool mouseOn = false, flipped = false;
        if( auto _name = node->first_attribute( "name" ) )
            name = _name->value();

        if( auto _file = node->first_attribute( "file" ) )
            file = _file->value();

        if( auto _load = node->first_attribute( "load" ) )
        {
            m_GameImages.push_back( new Image{ file } );
            if( _load->value() == "T" )
            {
                mouseOn = true;
            }
            if( auto _flip = node->first_attribute( "flip" ) )
            {
                if( _flip->value() == "T" )
                    flipped = true;
            }
            m_GameImages[ m_GameImages.size() - 1 ]->Load( mouseOn, flipped );
            m_GameImages[ m_GameImages.size() - 1 ]->SetName( name );
        }
        else
        {
            m_GameImages.push_back( new Image{ file, true, name } );
        }
        node = node->next_sibling();
    }
}

void GameManager::LoadAnimations( rapidxml::xml_node<char> * node )
{
    float frameT = 0.10F;

    while( node && node->name() == string( "AnimationSet" ) )
    {
        string name, file, soundtrackFile;
        if( auto _name = node->first_attribute( "name" ) )
            name = _name->value();
        m_Animations.push_back( make_shared<AnimationSet>( AnimationSet{ name } ) );
        auto animNode = node->first_node();
        while( animNode && animNode->name() == string( "Animation" ) )
        {
            if( auto _name = animNode->first_attribute( "name" ) )
                name = _name->value();
            if( auto _frameT = animNode->first_attribute( "frameT" ) )
            {
                frameT = float( atof( _frameT->value() ) );
            }
            else
            {
                frameT = 0.10F;
            }
            if( auto _name = animNode->first_attribute( "sound" ) )
                soundtrackFile = _name->value();
            AnimationPtr tmpAnim;
            if( name.length() && isdigit( name[0]) )
            {
                STATE::Type curState = ( STATE::Type )( atoi( name.c_str() ) );
                tmpAnim = make_shared<Animation>( Animation{ curState, frameT, soundtrackFile } );
            }
            else
                tmpAnim = make_shared<Animation>( Animation{ name, frameT, soundtrackFile } );
            for( auto texNode = animNode->first_node(); texNode != nullptr; texNode = texNode->next_sibling() )
            {
                if( auto _name = texNode->first_attribute( "name" ) )
                    name = _name->value();
                if( auto _file = texNode->first_attribute( "file" ) )
                    file = _file->value();
                bool mouseOn = false, flip = false;
                if( auto _load = texNode->first_attribute( "load" ) )
                {
                    if( _load->value() == string( "T" ) )
                        mouseOn = true;

                    if( auto _flip = texNode->first_attribute( "flip" ) )
                    {
                        if( _flip->value() == string( "T" ) )
                            flip = true;
                    }
                }
                tmpAnim->m_Frames.push_back( make_shared<Image>( file, 0, 0 ) );
                tmpAnim->m_Frames.back()->Load( mouseOn, flip );
            }
            *( m_Animations[m_Animations.size() - 1] ) += tmpAnim;
            animNode = animNode->next_sibling();
        }
        node = node->next_sibling();
    }
}

void GameManager::SaveMusicFileNames( string screenName, string soundtrackFile )
{
    if( auto screen = FindScreen( screenName ) )
        screen->SetSoundName( soundtrackFile );
}