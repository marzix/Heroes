#ifndef __STATS_INFO_BOARD_H__
#define __STATS_INFO_BOARD_H__

#include "VisibleObject.h"
#include "Descriptor.h"

class StatsInfoBoard;
using StatsInfoBoardPtr = shared_ptr<StatsInfoBoard>;

class StatsInfoBoard: public VisibleObject
{
public:
    StatsInfoBoard( Unit* unit, PLAYER_COLOR::Color color );

    static void InitializeHeaders();

    void    Initialize()override;
    void    Reset()override;
    void    Actualize()override;
    void    Render()override;
    int     GetHeightForSorting()const override;
    const Image&  GetAvatar();

private:
    static map<string, ImagePtr> s_HeadersImgMap;
    static SDL_Rect s_InfoCellRect;

    Descriptor m_UnitDescr;
    map<string, ImagePtr> m_NumbersImgMap;
    Unit* m_Owner;
};

#endif
