#include "Grid.h"

int Grid::s_RadiusSquare = 24 * 24;
SDL_Point Grid::s_Center{ 22, 27 };

Grid::Grid( const Image * gridImg, const Image * gridImgRange, int x, int y, uint row, uint col )
: m_Occupied( false )
, m_Hex( gridImg )
, m_HexInRange( gridImgRange )
, m_Row( row )
, m_Col( col )
{
    m_Hex.SetRect( x, y );
}

Image Grid::GetImg()const
{
    return m_Hex;
}

void Grid::Render()
{
    if( m_Occupied || !m_InRange )
    {
        m_Hex.RenderTexture();
    }
    else
    {
        m_HexInRange.SetRect( m_Hex.GetRect().x, m_Hex.GetRect().y );
        m_HexInRange.RenderTexture();
    }
}

bool Grid::GetOccupied()const
{
    return m_Occupied;
}

Unit * Grid::GetUnit()
{
    return m_Unit;
}

SDL_Point Grid::GetMiddle()const
{
    SDL_Point middle;
    SDL_Rect grid = m_Hex.GetRect();
    middle.x = grid.x + grid.w / 2;
    middle.y = grid.y + grid.h / 2;
    return middle;
}

SDL_Rect Grid::GetRect()const
{
    return m_Hex.GetRect();
}

uint Grid::GetWidth()const
{
    return m_Hex.GetRect().w;
}

uint Grid::GetHeight()const
{
    return m_Hex.GetRect().h;
}

uint Grid::GetRow()const
{
    return m_Row;
}

uint Grid::GetCol()const
{
    return m_Col;
}

bool Grid::InRange()const
{
    return m_InRange;
}

void Grid::SetOccupied( bool val )
{
    m_Occupied = val;
    if( val == false )
    {
        SetUnit( nullptr );
    }
}

void Grid::SetUnit( Unit * u )
{
    m_Unit = u;
    if( m_Unit )
    {
        m_Occupied = true;
    }
    else
    {
        m_Occupied = false;
    }
}

void Grid::SetRange( bool val )
{
    m_InRange = val;
}

bool Grid::IsMouseOn( SDL_Point mousePos )
{
    int xSquare = s_Center.x - mousePos.x;
    xSquare *= xSquare;
    int ySquare = s_Center.y - mousePos.y;
    ySquare *= ySquare;

    if( ySquare + xSquare <= s_RadiusSquare )
    {
        return true;
    }
    return false;
}