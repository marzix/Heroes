#include "StatsInfoBoard.h"
#include "GameManager.h"
#include "Unit.h"

#pragma warning(disable : 4996)

StatsInfoBoard::StatsInfoBoard( Unit* unit, PLAYER_COLOR::Color color )
: m_Owner( unit )
{
    m_Name = PLAYER_COLOR::ToString( color );
}

map<string, ImagePtr> StatsInfoBoard::s_HeadersImgMap;
SDL_Rect StatsInfoBoard::s_InfoCellRect;

void StatsInfoBoard::InitializeHeaders()
{
    int x, y;
    s_HeadersImgMap =
    {
        { "Atak",       make_shared<Image>( TIMES, "Atak: ", 10, GetRGBA( eColor::WHITE ) ) },
        { "Obrona",     make_shared<Image>( TIMES, "Obrona: ", 10, GetRGBA( eColor::WHITE ) ) },
        { "Zycie",      make_shared<Image>( TIMES, "Zycie: ", 10, GetRGBA( eColor::WHITE ) ) },
        { "Pozostalo",  make_shared<Image>( TIMES, "Pozostalo: ", 10, GetRGBA( eColor::WHITE ) ) },
        { "Szybkosc",   make_shared<Image>( TIMES, "Szybkosc: ", 10, GetRGBA( eColor::WHITE ) ) }
    };
    s_InfoCellRect.x = x = 9;
    s_InfoCellRect.y = y = 97;
    s_InfoCellRect.h = 19;
    s_InfoCellRect.w = 90;
    string Headers[] = { "Atak", "Obrona", "Zycie", "Pozostalo", "Szybkosc" };
    for( auto header: Headers )
    {
        if( auto img = s_HeadersImgMap[header] )
        {
            img->SetRect( x, y );
            y += s_InfoCellRect.h;
        }
    }
}

void StatsInfoBoard::Initialize()
{
    if( !m_Owner )
        return;
    auto & Game = GameManager::GetSingleton();
    if( m_CurImage = make_shared<Image>( Game.FindImage( "stats" + m_Name ) ) )
    {
        if( m_Owner->GetPlayer() == SIDE::LEFT )
            m_Position.x = 0;
        else
            m_Position.x = eWindow::WIDTH - m_CurImage->GetRect().w;
        m_Position.y = eWindow::HEIGHT / 2 - m_CurImage->GetRect().h / 2;
        m_CurImage->SetRect( m_Position.x, m_Position.y );
        int x = 0, y = 0;
        if( auto avatar = Game.FindImage( m_Owner->GetName() + "Avatar" ) )
        {
            m_UnitDescr.SetAvatarImage( *avatar );
            x = m_CurImage->GetRect().w / 2;
            y = m_Position.y + m_CurImage->GetRect().h / 12 + m_UnitDescr.GetAvatarImage().GetRect().h / 2;
            m_UnitDescr.SetAvatarImagePos( x + m_Position.x, y );
        }
        uint attack = m_Owner->GetAttack();
        uint defense = m_Owner->GetDefense();
        uint life = m_Owner->GetLifePerUnit();
        uint speed = m_Owner->GetSpeed();
        char temp[10];
        auto ToStr = [&]( int val )->const char*
        {
            sprintf( temp , "%d" , val );
            return temp;
        };

        m_NumbersImgMap = 
        {
            { "Atak",       make_shared<Image>( TIMES, ToStr( attack  ), 10, GetRGBA( eColor::WHITE ) ) },
            { "Obrona",     make_shared<Image>( TIMES, ToStr( defense ), 10, GetRGBA( eColor::WHITE ) ) },
            { "Zycie",      make_shared<Image>( TIMES, ToStr( life    ), 10, GetRGBA( eColor::WHITE ) ) },
            { "Pozostalo",  make_shared<Image>( TIMES, ToStr( life    ), 10, GetRGBA( eColor::WHITE ) ) },
            { "Szybkosc",   make_shared<Image>( TIMES, ToStr( speed   ), 10, GetRGBA( eColor::WHITE ) ) }
        };
        x = s_InfoCellRect.x;
        y = s_InfoCellRect.y;
        string Headers[] = { "Atak", "Obrona", "Zycie", "Pozostalo", "Szybkosc" };
        for( auto header: Headers )
        {
            if( m_NumbersImgMap[header] && s_HeadersImgMap[header] )
            {
                m_NumbersImgMap[header]->SetRect( x + s_HeadersImgMap[header]->GetRect().w, y );
                y += s_InfoCellRect.h;
            }
        }
    }
}

void StatsInfoBoard::Reset()
{}

void StatsInfoBoard::Actualize()
{
    if( !m_Owner || !m_IsActive )
        return;
    char temp[10];
    sprintf( temp , "%d" , m_Owner->GetLifeRemaining() );
    if( m_NumbersImgMap["Pozostalo"] )
    {
        if( strcmp( m_NumbersImgMap["Pozostalo"]->GetTexName().c_str(), temp ) != 0 )
            m_NumbersImgMap["Pozostalo"]->SetTexture( TIMES, temp, 10, GetRGBA( eColor::WHITE ) );
    }
}

void StatsInfoBoard::Render()
{
    if( !m_IsActive || !m_Owner )
        return;
    SDL_Rect dst = m_CurImage->GetRect();
    m_CurImage->RenderTexture();
    //dst = m_UnitDescr.GetAvatarImage().GetRect();
    //dst.x += m_Position.x;
    m_UnitDescr.Render();
    if( const ImagePtr number = m_Owner->GetQuantityImg() )
    {
        auto avatar = m_UnitDescr.GetAvatarImage().GetRect();
        dst.w = (int)( number->GetRect().w * 1.2f );
        dst.h = (int)( number->GetRect().h * 1.2f );
        dst.x += avatar.w - dst.w;
        dst.y = avatar.y + avatar.h - dst.h;
        number->RenderTexture( SDL_FLIP_NONE, 0.0f, &dst );
    }
    string Headers[] = { "Atak", "Obrona", "Zycie", "Pozostalo", "Szybkosc" };
    for( auto header: Headers )
    {
        if( m_NumbersImgMap[header] && s_HeadersImgMap[header] )
        {
            dst = s_HeadersImgMap[header]->GetRect();
            dst.x += m_Position.x;
            dst.y += m_Position.y;
            s_HeadersImgMap[header]->RenderTexture( SDL_FLIP_NONE, 0.0f, &dst );
            dst = m_NumbersImgMap[header]->GetRect();
            dst.x = m_Position.x + m_NumbersImgMap["Pozostalo"]->GetRect().x;
            dst.y += m_Position.y;
            m_NumbersImgMap[header]->RenderTexture( SDL_FLIP_NONE, 0.0f, &dst );
        }
    }
}

int StatsInfoBoard::GetHeightForSorting()const
{
    return eWindow::HEIGHT;
}

const Image& StatsInfoBoard::GetAvatar()
{
    return m_UnitDescr.GetAvatarImage();
}
