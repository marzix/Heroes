#ifndef __GRID_H__
#define __GRID_H__

#include "Image.h"

class Grid
{
public:
    Grid() = default;
    Grid( const Image * gridImg, const Image * gridImgRange, int x, int y, uint row, uint col );
    
    Unit *      GetUnit();
    Image       GetImg()const;
    bool        GetOccupied()const;
    SDL_Point   GetMiddle()const;
    SDL_Rect    GetRect()const;
    uint        GetWidth()const;
    uint        GetHeight()const;
    uint        GetRow()const;
    uint        GetCol()const;
    bool        InRange()const;
    void        SetUnit( Unit * u );
    void        SetOccupied( bool val );
    void        SetRange( bool val );

    bool IsMouseOn( SDL_Point mousePos );
    void Render();

private:
    Image m_Hex;
    Image m_HexInRange;
    Unit * m_Unit = nullptr;
    bool m_Occupied = false;
    bool m_InRange = false;
    uint m_Row = 0;
    uint m_Col = 0;

    static SDL_Point s_Center;
    static int s_RadiusSquare;
};

#endif