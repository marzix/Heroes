#include "OptionsScreen.h"
#include "GameManager.h"
#include "BattleScreen.h"
#include "AudioManager.h"

OptionsScreen::OptionsScreen( string n )
{
    m_Name = n;
}

OptionsScreen::~OptionsScreen()
{
    delete m_Players[1];
    delete m_Players[0];
}

void OptionsScreen::Initialize()
{
    auto& Game = GameManager::GetSingleton();
    m_Previous = Game.FindScreen( "MenuScreen" );
    m_Next = Game.FindScreen( "BattleScreen" );
    int x, y;
    m_Frame = make_shared<Image>( Game.FindImage( "frame" ) );
    m_Frame->SetRect( eWindow::WIDTH / 2, eWindow::HEIGHT / 10 );
    SDL_Rect r = m_Frame->GetRect(); 
    m_CurImage = make_shared<Image>( Game.FindImage( "OSbackground" ) );
    SetText();
    SetBars();

    x = m_Frame->GetRect().x + 17;
    y = m_Frame->GetRect().y + m_Frame->GetRect().h - 55;  
    m_ButtonList.push_back(  
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonStart1" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>(  Game.FindImage( "buttonStart2" ) ), 
            [=](){ this->SetBattle(); } ) );

    x += m_ButtonList[0]->GetImage()->GetRect().w + 2;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonBack1" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( Game.FindImage( "buttonBack2" ) ), 
            PreviousScreen ) );

    x = m_Text[3]->GetRect().x - 10;
    y = m_Text[5]->GetRect().y + m_Text[5]->GetRect().h + 5;
    Image* button1 = Game.FindImage( "buttonPrev1" );
    Image* button2 = Game.FindImage( "buttonPrev2" );
    assert( button1 );
    assert( button2 );
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ), 
            [=]() { SwitchCastle( 0, true ); } ) );

    x += m_ButtonList[2]->GetImage()->GetRect().w;

    x += m_CastleList[0].GetCastleDescr().GetAvatarImage().GetRect().w;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ), 
            [=]() { SwitchCastle( 0 ); }, SDL_FLIP_HORIZONTAL ) );

    x += m_ButtonList[3]->GetImage()->GetRect().w + 5;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ), 
            [=]() { SwitchHero( 0, true ); } ) );

    x += m_ButtonList[4]->GetImage()->GetRect().w + m_CastleList[0].GetHero( 0 ).GetAvatarImage().GetRect().w;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ), 
            [=]() { SwitchHero( 0 ); }, SDL_FLIP_HORIZONTAL ) );

    x = m_ButtonList[2]->GetPosition().x;
    y = m_PlayerOptionList[1].GetBar()->GetRect().y + 5;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ),  
            [=]() { SwitchCastle( 1, true ); } ) );

    x = m_ButtonList[3]->GetPosition().x;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ),  
            [=]() { SwitchCastle( 1 ); }, SDL_FLIP_HORIZONTAL ) );

    x = m_ButtonList[4]->GetPosition().x;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ), 
            [=]() { SwitchHero( 1, true ); } ) );

    x = m_ButtonList[5]->GetPosition().x;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( *button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( *button2 ), 
            [=]() { SwitchHero( 1 ); }, SDL_FLIP_HORIZONTAL ) );

    r = m_Text[6]->GetRect();
    x = r.x + r.w + 26;
    y = r.y - 2;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonDiff11" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( Game.FindImage( "buttonDiff12" ) ), 
            [=]() { string s = "brak limitu";
                    ChangeText( m_Text[m_Text.size() - 2], s, 0 );
                    GameManager::GetSingleton().SetRoundTime( 0 ); } ) );

    r.w = m_ButtonList[ m_ButtonList.size() - 1]->GetImage()->GetRect().w;
    x += r.w + 2;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonDiff21" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( Game.FindImage( "buttonDiff22" ) ), 
            [=]() { string s = "30 sekund";
                    ChangeText( m_Text[m_Text.size() - 2], s, 30 );
                    GameManager::GetSingleton().SetRoundTime( 30 ); } ) );
    x += r.w + 2;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonDiff31" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( Game.FindImage( "buttonDiff32" ) ),  
            [=]() { string s = "20 sekund";
                    ChangeText( m_Text[m_Text.size() - 2], s, 20 );
                    GameManager::GetSingleton().SetRoundTime( 20 ); } ) );
    x += r.w + 2;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonDiff41" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( Game.FindImage( "buttonDiff42" ) ),  
            [=]() { string s = "15 sekund";
                    ChangeText( m_Text[m_Text.size() - 2], s, 15 );
                    GameManager::GetSingleton().SetRoundTime( 15 ); } ) );
    x += r.w + 2;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( Game.FindImage( "buttonDiff51" ) ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( Game.FindImage( "buttonDiff52" ) ),  
            [=]() { string s = "10 sekund";
                    ChangeText( m_Text[m_Text.size() - 2], s, 10 );
                    GameManager::GetSingleton().SetRoundTime( 10 ); } ) );

    x = m_Text[2]->GetRect().x - 14;
    y = m_Text[2]->GetRect().y + 46;
    button1 = Game.FindImage( "buttonName" );
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( button1 ), 
            [=]() { ChangePlayerName( 0 ); } ) );
    m_ButtonList.back()->SetName( "buttonName0" );

    y += m_PlayerOptionList[0].GetBar()->GetRect().h + 1;
    m_ButtonList.push_back( 
        make_shared<Button>( x, y, make_shared<Image>( button1 ), 
                                   nullptr, nullptr,
                                   make_shared<Image>( button1 ), 
            [=]() { ChangePlayerName( 1 ); } ) );
    m_ButtonList.back()->SetName( "buttonName1" );

    r = m_Frame->GetRect();
    r.x += 22;
    r.y += 90;
    m_ButtonList.push_back( 
        make_shared<Button>( r.x, r.y, make_shared<Image>(  Game.FindImage( "buttonUnchecked" ) ), 
            nullptr, nullptr,
            make_shared<Image>(  Game.FindImage( "buttonChecked" ) ), 
            ChangeIsAI ) );
    m_ButtonList.back()->SetStayPushed( true );

    for( auto& button : m_ButtonList )
    {
        if( button )
            button->Initialize();
    }
    InitializePlayers();
}

void OptionsScreen::SetText()
{
    SDL_Rect r = m_Frame->GetRect();
    int x, y;
    m_Text.push_back( make_shared<Image>( ANDLSO, "Opcje zaawansowane", 20, GetRGBA( eColor::LIGHT_GOLD ) ) );
    x = r.x + r.w / 2 - m_Text[0]->GetRect().w / 2;
    y = r.y + r.h / 30;
    m_Text[0]->SetRect( x, y );
    m_Text.push_back( make_shared<Image>( ANDLSO,   "Ustaw opcje gry oraz podaj imiona graczy", 16, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( TIMES,    "Imie gracza",                              15, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( TIMES,    "Zamek",                                    15, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( TIMES,    "Posiadany",                                15, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( TIMES,    "bohater",                                  15, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( TIMES,    "Czas tury:",                               15, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( TIMES,    "brak limitu",                              15, GetRGBA( eColor::LIGHT_GOLD ) ) );
    m_Text.push_back( make_shared<Image>( ANDLSO,   "Tryb Single Player",                       16, GetRGBA( eColor::LIGHT_GOLD ) ) );

    y += m_Text[0]->GetRect().h;
    x = r.x + r.w / 2 - m_Text[1]->GetRect().w / 2;
    m_Text[1]->SetRect( x, y );

    y += r.h / 5;
    x = r.x + r.w / 5 + 3;
    for( unsigned int i = 2; i < m_Text.size() - 1; i++ )
    {
        m_Text[i]->SetRect( x, y );
        x += m_Text[i]->GetRect().w + 35;
    }

    x = m_Text[4]->GetRect().x + m_Text[4]->GetRect().w / 2 - m_Text[5]->GetRect().w / 2;
    y = m_Text[4]->GetRect().y + m_Text[4]->GetRect().h;
    m_Text[5]->SetRect( x, y );

    x = r.x + 25;
    y = r.y + r.h * 2 / 3 + 5;
    m_Text[6]->SetRect( x, y );
    m_Text[7]->SetRect( x, y + m_Text[6]->GetRect().h );
    r = m_Frame->GetRect();
    r.x += 48;
    r.y += 87;
    m_Text.back()->SetRect( r.x, r.y );
}

vector<SDL_Point> ParseFromString( string x, string y )
{
    vector<SDL_Point> points;
    if( x.empty() || y.empty() )
        return points;
    SDL_Point p;
    size_t first, last;
    first = x.find_first_of( ':', 0 );
    last = x.find_last_of( ':', x.length() );
    if( first == string::npos || last == string::npos )
        return points;
    p.x = atoi( x.substr( 0, first ).c_str() );
    points.push_back( p );
    p.x = atoi( x.substr( first + 1, last ).c_str() );
    points.push_back( p );
    p.x = atoi( x.substr( last + 1 ).c_str() );
    points.push_back( p );

    first = y.find_first_of( ':', 0 );
    last = y.find_last_of( ':', y.length() );
    if( first == string::npos || last == string::npos )
        return points;
    points[0].y = atoi( y.substr( 0, first ).c_str() );
    points[1].y = atoi( y.substr( first + 1, last ).c_str() );
    points[2].y = atoi( y.substr( last + 1 ).c_str() );

    return points;
}

void OptionsScreen::SetCastles( rapidxml::xml_node<char> * rootNode )
{
    auto & Game = GameManager::GetSingleton();
    string name, file;
    int attack, defence, speed, number, size, life, type;
    while( rootNode && ( rootNode->name() == string( "Castle" ) ) )
    {
        name = rootNode->first_attribute( "name" )->value();
        file = rootNode->first_attribute( "file" )->value();
        auto childNode = rootNode->first_node();
        m_CastleList.push_back( Castle{ Image( TIMES, name, 10, GetRGBA( eColor::LIGHT_GOLD ) ), 
                                        Image( file, true, name ), 
                                        Image( Game.FindImage( childNode->first_attribute( "name" )->value() ) ) } );

        childNode = childNode->next_sibling();
        if( childNode->name() == string( "Heroes" ) )
        {
            for( auto heroNode = childNode->first_node(); heroNode != nullptr; heroNode = heroNode->next_sibling() )
            {
                name = heroNode->first_attribute( "name" )->value();
                file = heroNode->first_attribute( "file" )->value();
                m_CastleList[ m_CastleList.size() - 1].AddHero( Descriptor{ Image( TIMES, name, 10, GetRGBA( eColor::LIGHT_GOLD ) ), 
                                                                            Image( file, true, name ) } );
            }
        }
        childNode = childNode->next_sibling();
        if( childNode->name() == string( "Units" ) )
        {
            for( auto unitNode = childNode->first_node(); unitNode != nullptr; unitNode = unitNode->next_sibling() )
            {
                name = unitNode->first_attribute( "name" )->value();

                //unit avatar and shot object
                if( auto unitChildNode = unitNode->first_node() )
                {
                    if( unitChildNode->name() == string( "texture" ) )
                    {
                        file = unitChildNode->first_attribute( "file" )->value();
                        unitChildNode = unitChildNode->next_sibling();
                    }
                    if( unitChildNode && unitChildNode->name() == string( "shot" ) )
                    {
                        string shotName = unitChildNode->first_attribute( "name" )->value();
                        SHOT_TYPE type = StringToShotType( unitChildNode->first_attribute( "type" )->value() );
                        vector<SDL_Point> points = ParseFromString( unitChildNode->first_attribute( "posX" )->value(), unitChildNode->first_attribute( "posY" )->value() );
                        ImagePtr shotImg = make_shared<Image>( shotName, true );
                        Game.AddShotObject( make_shared<ShotObject>( name, shotImg, type, points ) );
                    }
                }

                attack = atoi( unitNode->first_attribute( "attack" )->value() );
                defence = atoi( unitNode->first_attribute( "defence" )->value() );
                speed = atoi( unitNode->first_attribute( "speed" )->value() );
                number = atoi( unitNode->first_attribute( "number" )->value() );
                size = atoi( unitNode->first_attribute( "size" )->value() );
                life = atoi( unitNode->first_attribute( "life" )->value() );
                type = atoi( unitNode->first_attribute( "type" )->value() );
                m_CastleList[ m_CastleList.size() - 1].AddUnitDescr( UnitDescr( name, attack, defence, life, speed, TYPE::Name( type ), size, number ) );                
            }
        }
        rootNode = rootNode->next_sibling();
    }
}

void OptionsScreen::SetBars()
{
    int x = 0, y = 0;
    m_PlayerOptionList.push_back( PlayerOptionPanel{ PLAYER_COLOR::RED, 
        make_shared<Button>( x, y, make_shared<Image>( "../images/AOflgbRn.png", true ), 
                                   make_shared<Image>( "../images/AOflgbRh.png", true ), 
                                   nullptr,
                                   make_shared<Image>( "../images/AOflgbRs.png", true ), [=]() { SwitchColor( PLAYER_COLOR::RED ); } ), 
        make_shared<Image>( "../images/optionBarR1.png", true ) } ); 

    m_PlayerOptionList.push_back( PlayerOptionPanel{ PLAYER_COLOR::BLUE, 
        make_shared<Button>( x, y, make_shared<Image>( "../images/AOflgbBn.png", true ), 
                                   make_shared<Image>( "../images/AOflgbBh.png", true ), 
                                   nullptr,
                                   make_shared<Image>( "../images/AOflgbBs.png", true ), [=]() { SwitchColor( PLAYER_COLOR::BLUE ); } ), 
        make_shared<Image>( "../images/optionBarB1.png", true ) } ); 
    
    m_PlayerOptionList.push_back( PlayerOptionPanel{ PLAYER_COLOR::PURPLE, 
        make_shared<Button>( x, y, make_shared<Image>( "../images/AOflgbPn.png", true ), 
                                   make_shared<Image>( "../images/AOflgbPh.png", true ), 
                                   nullptr,
                                   make_shared<Image>( "../images/AOflgbPs.png", true ), [=]() { SwitchColor( PLAYER_COLOR::PURPLE ); } ), 
        make_shared<Image>( "../images/optionBarP1.png", true ) } ); 
    
    m_PlayerOptionList.push_back( PlayerOptionPanel{ PLAYER_COLOR::GREEN, 
        make_shared<Button>( x, y, make_shared<Image>( "../images/AOflgbGn.png", true ), 
                                   make_shared<Image>( "../images/AOflgbGh.png", true ), 
                                   nullptr,
                                   make_shared<Image>( "../images/AOflgbGs.png", true ), [=]() { SwitchColor( PLAYER_COLOR::GREEN ); } ), 
        make_shared<Image>( "../images/optionBarG1.png", true ) } );

    x = m_Text[2]->GetRect().x - m_Text[2]->GetRect().w / 4 - m_PlayerOptionList[0].GetButton()->GetImage()->GetRect().w;
    y = m_Text[5]->GetRect().y + m_Text[5]->GetRect().h;
    for( auto & tmp: m_PlayerOptionList )
    {
        tmp.SetPosition( x, y );
    }
    y += m_PlayerOptionList[0].GetButton()->GetImage()->GetRect().h;
    m_PlayerOptionList[1].SetPosition( x, y );
}

void OptionsScreen::SetTextPos()
{
    SDL_Rect r = m_Frame->GetRect();
    int x, y;
    x = r.x + r.w / 2 - m_Text[0]->GetRect().w / 2;
    y = r.y + r.h / 30;
    m_Text[0]->SetRect( x, y );

    y += m_Text[0]->GetRect().h;
    x = r.x + r.w / 2 - m_Text[1]->GetRect().w / 2;
    m_Text[1]->SetRect( x, y );

    y += r.h / 5;
    x = r.x + r.w / 4;
    for( unsigned int i = 2; i < m_Text.size(); i++ )
    {
        m_Text[i]->SetRect( x, y );
        x += m_Text[i]->GetRect().w + 15;
    }
}

void OptionsScreen::InitializePlayers()
{
    m_Players[0] = new Player{ "Player1" };
    m_Players[1] = new Player{ "Player2" };
    m_Players[0]->SetColor( PLAYER_COLOR::RED );
    m_Players[1]->SetColor( PLAYER_COLOR::BLUE );
    m_Players[0]->SetHero( make_shared<Hero>() );
    m_Players[1]->SetHero( make_shared<Hero>() );
}

void OptionsScreen::SetBattle()
{
    auto & Game = GameManager::GetSingleton();
    int index = FindCurrentCastle( m_Players[0]->GetCastle().GetName() );
    vector<UnitDescr> unitList = m_CastleList[index].GetUnitDescrList();
    for( auto& descr: unitList )
    {
        UnitPtr unit = make_shared<Unit>( descr.name, descr.unitType, descr.unitSize, descr.unitStats );
        UnitPtr unit1 = make_shared<Unit>( "Beholder", 1, 1, 100, 8, TYPE::ARCHER, 1, 2 );
        m_Players[0]->AddUnit( unit );
    }
    m_Players[0]->SetHeroBodyTex( m_CastleList[index].GetHeroBody() );
    index = FindCurrentCastle( m_Players[1]->GetCastle().GetName() );
    vector<UnitDescr> unitList2 = m_CastleList[index].GetUnitDescrList();
    for( auto& descr: unitList2 )
    {
        UnitPtr unit = make_shared<Unit>( descr.name, descr.unitType, descr.unitSize, descr.unitStats );
        m_Players[1]->AddUnit( unit );
    }
    m_Players[1]->SetHeroBodyTex( m_CastleList[index].GetHeroBody() );
    if( BattleScreen* bScreen = dynamic_cast<BattleScreen*>( m_Next ) )
        bScreen->SetPlayers( m_Players );
    
    NextScreen();
}

void OptionsScreen::Reset()
{
    PlayerOptionPanel* tmp = nullptr;
    m_Players[0]->SetName( "Player1" );
    m_Players[0]->ResetArmy();
    m_Players[1]->SetName( "Player2" );
    m_Players[1]->ResetArmy();
    m_Players[1]->SetIsAI( false );
    tmp = FindOptionPanel( m_Players[0]->GetColor() );
    SDL_Rect r = tmp->GetButton()->GetImage()->GetRect();
    tmp->SetPlayerIndex( -1 );
    m_Players[0]->SetColor( PLAYER_COLOR::RED );
    tmp = FindOptionPanel( PLAYER_COLOR::RED );
    tmp->SetPlayerIndex( 0 );
    tmp->SetPosition( r.x, r.y );
    tmp = FindOptionPanel( m_Players[1]->GetColor() );
    r = tmp->GetButton()->GetImage()->GetRect();
    tmp->SetPlayerIndex( -1 );
    m_Players[1]->SetColor( PLAYER_COLOR::BLUE );
    tmp = FindOptionPanel( PLAYER_COLOR::BLUE );
    tmp->SetPlayerIndex( 1 );
    tmp->SetPosition( r.x, r.y );

    int x, y;
    x = m_ButtonList[2]->GetImage()->GetRect().x + m_ButtonList[2]->GetImage()->GetRect().w;
    y =  m_ButtonList[2]->GetImage()->GetRect().y - 3;
    m_Players[0]->SetCastle( m_CastleList[0].GetCastleDescr() );
    m_Players[0]->SetCastlePos( x, y );
    x = m_ButtonList[4]->GetImage()->GetRect().x + m_ButtonList[4]->GetImage()->GetRect().w;
    y =  m_ButtonList[4]->GetImage()->GetRect().y - 3;
    m_Players[0]->SetHeroDescr( m_CastleList[0].GetHero( 0 ) );
    m_Players[0]->SetHeroDescrPos( x, y );

    x = m_ButtonList[6]->GetImage()->GetRect().x + m_ButtonList[6]->GetImage()->GetRect().w;
    y =  m_ButtonList[6]->GetImage()->GetRect().y - 3;
    m_Players[1]->SetCastle( m_CastleList[0].GetCastleDescr() );
    m_Players[1]->SetCastlePos( x, y );
    y = m_ButtonList[6]->GetImage()->GetRect().y - 3;
    if( auto hero = m_Players[0]->GetHero() )
        x = hero->GetHeroDescr().GetAvatarImage().GetRect().x;
    m_Players[1]->SetHeroDescr( m_CastleList[0].GetHero( 1 ) );
    m_Players[1]->SetHeroDescrPos( x, y );
    x = m_ButtonList[ m_ButtonList.size() - 2 ]->GetPosition().x + m_ButtonList[ m_ButtonList.size() - 2 ]->GetImage()->GetRect().w / 2 - m_Players[0]->GetName()->GetRect().w / 2;
    y = m_ButtonList[ m_ButtonList.size() - 3 ]->GetPosition().y + m_ButtonList[ m_ButtonList.size() - 3 ]->GetImage()->GetRect().h / 2 - m_Players[0]->GetName()->GetRect().h / 2;
    
    m_Players[0]->GetName()->SetRect( x, y );
    y += m_PlayerOptionList[0].GetBar()->GetRect().h;
    m_Players[1]->GetName()->SetRect(x, y );
    ChangeText( m_Text[m_Text.size() - 2], "brak limitu", 0 );
    m_ButtonList.back()->Reset();
    if( AudioManager::GetSingleton().IsPlaying() )
        AudioManager::GetSingleton().Stop();
}

void OptionsScreen::Actualize()
{
    auto & Game = GameManager::GetSingleton();
    SDL_Event e = Game.GetEvent();
    if( Quit( e ) )
    {
        m_IsActive = false;
        return;
    }
    if( enterName != -1 )
    {
        string number = enterName == 0 ? "0" : "1";
        if( Game.WasMouseButtonPressed() )
        {
            if( auto b = FindButton( "buttonName" + number ) )
            {
                if( !Game.IsMouseOn( b ) )
                {
                    m_Players[enterName]->EndEdditing();
                    enterName = -1;
                }
            }
        }
        if( enterName != -1 )
        {
            if( !m_Players[enterName]->ChangeName() )
                enterName = -1;
        }
    }
    else
    {
        for( auto& button : m_ButtonList )
        {
            button->Actualize();
        }
        for( auto panel : m_PlayerOptionList )
        {
            if( panel.GetPlayerIndex() != -1 )
            {
                panel.GetButton()->Actualize();
            }
        }
    }
    AudioManager::GetSingleton().PlayTrack( m_SoundTrackName );
}

void OptionsScreen::Render()
{
    auto & Game = GameManager::GetSingleton();
    m_CurImage->RenderTexture();
    m_Frame->RenderTexture();
    for( auto& panel:  m_PlayerOptionList )
    {
        if( panel.GetPlayerIndex() != -1 )
            panel.Render();
    }
    m_Players[0]->GetCastle().Render();
    m_Players[0]->GetHero()->RenderDescriptor();
    m_Players[0]->GetName()->RenderTexture();
    m_Players[1]->GetCastle().Render();
    m_Players[1]->GetHero()->RenderDescriptor();
    m_Players[1]->GetName()->RenderTexture();

    for( auto& button : m_ButtonList )
    {
        button->Render();
    }
    for( auto& text : m_Text )
    {
        text->RenderTexture( m_Flip );
    }
}

void OptionsScreen::SetName( string n, int index )
{
    m_Players[index]->SetName( n );
}

void OptionsScreen::SetColor( PLAYER_COLOR::Color color, int index )
{
    m_Players[index]->SetColor( color );
}

Player * OptionsScreen::GetPlayer( int index )
{
    return m_Players[index];
}

Castle* OptionsScreen::GetCastle( const string castle )
{
    int i = FindCurrentCastle( castle );
    if( i >= 0 && i < (int)m_CastleList.size() )
        return &m_CastleList[i];
    return nullptr;
}

void OptionsScreen::SetEnterName( int val )
{
    enterName = val;
}

PlayerOptionPanel* OptionsScreen::FindOptionPanel( PLAYER_COLOR::Color color )
{
    for( unsigned int i = 0; i < m_PlayerOptionList.size(); i++ )
    {
        if( m_PlayerOptionList[i].GetColor() == color )
            return &m_PlayerOptionList[i];
    }
    return nullptr;
}

PlayerOptionPanel* OptionsScreen::FindOptionPanel( Button* button )
{
    for( unsigned int i = 0; i < m_PlayerOptionList.size(); i++ )
    {
        if( m_PlayerOptionList[i].GetButton().get() == button )
            return &m_PlayerOptionList[i];
    }
    return nullptr;
}

void OptionsScreen::SwitchPlayerColor( uint index )
{
    PLAYER_COLOR::Color reservedColor = index == 0 ? m_Players[1]->GetColor() : m_Players[0]->GetColor();
    for( unsigned int i = 0; i < m_PlayerOptionList.size(); i++ )
    {
        if( m_PlayerOptionList[i].GetPlayerIndex() == index )
        {
            m_PlayerOptionList[i].SetPlayerIndex( -1 );
            SDL_Rect rect = m_PlayerOptionList[i].GetButton()->GetImage()->GetRect();
            i++;
            if( i >= m_PlayerOptionList.size() )
                i = 0;
            if( m_PlayerOptionList[i].GetColor() == reservedColor )
            {
                i++;
                if( i >= m_PlayerOptionList.size() )
                    i = 0;
            }
            m_PlayerOptionList[i].SetPlayerIndex( index );
            m_PlayerOptionList[i].SetPosition( rect.x, rect.y );
            m_Players[index]->SetColor( m_PlayerOptionList[i].GetColor() );
        }
    }
}

int OptionsScreen::FindCurrentCastle( const string castleName )
{
    for( unsigned int i = 0; i < m_CastleList.size(); i++ )
    {
        if( m_CastleList[i].GetName() == castleName )
        {
            return i;
        }
    }
    return 0;
}

Castle* OptionsScreen::FindNextCastleTex( const string castleName )
{
    int i = FindCurrentCastle( castleName );
    if( i + 1 < (int)m_CastleList.size() )
        return &m_CastleList[i + 1];
    else
        return &m_CastleList[0];        
}

Castle* OptionsScreen::FindPrevCastleTex( const string castleName )
{
    int i = FindCurrentCastle( castleName );
    if( i > 0 )
        return &m_CastleList[i - 1];
    else
        return &m_CastleList[m_CastleList.size() - 1];
}

Descriptor* OptionsScreen::FindNextHero( int playerIndex, bool reverseOrder )
{
    string reservedHero;
    if( playerIndex == 0 && m_Players[1]->GetHero() )
        reservedHero = m_Players[1]->GetHero()->GetHeroDescr().GetName();
    else if( m_Players[0]->GetHero() )
        reservedHero = m_Players[0]->GetHero()->GetHeroDescr().GetName();
    int i = FindCurrentCastle( m_Players[playerIndex]->GetCastle().GetName() );
    Descriptor* pDesc;
    if( auto hero = m_Players[playerIndex]->GetHero() )
    {
        if( !reverseOrder )
        {
            pDesc = m_CastleList[i].FindNextHero( hero->GetHeroDescr().GetName() );
            if( pDesc && pDesc->GetName().compare( reservedHero ) == 0 )
                return m_CastleList[i].FindNextHero( pDesc->GetName() );
            else
                return pDesc;
        }
        else
        {
            pDesc = m_CastleList[i].FindPrevHero( hero->GetHeroDescr().GetName() );
            if( pDesc && pDesc->GetName().compare( reservedHero ) == 0 )
                return m_CastleList[i].FindPrevHero( pDesc->GetName() );
            else
                return pDesc;
        }
    }
    return nullptr;
}

Button* OptionsScreen::FindButton( const string name )
{
    for( auto& button : m_ButtonList )
    {
        if( button && name.compare( button->GetName() ) == 0 )
            return button.get();
    }
    return nullptr;
}