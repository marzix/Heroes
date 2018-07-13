#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "Declarations.h"

class Cursor
{
public:
    Cursor();

    void                CheckMousePos();
    void                RenderCursor();
    void                BlockCursor();
    void                SetMouseOn();
    void                SetTexture( STATE::Type s, TYPE::Name t, bool inRange );
    void                SetRotation( double rotation );
    void                SetDefault();
    void                ResetMouseOn();
    VisibleObject*      GetMouseOn();
    SDL_Point const &   GetMousePos()const;
    bool                IsMouseOn( VisibleObject* pObj );
    Uint32              WasButtonPressed( Uint32 button = 0 );

private:

    double m_Rotation = 0.0f;
    map<string, ImagePtr> m_Cursors;
    ImagePtr m_CurrentImage;    
    SDL_Point m_MousePos;
    VisibleObject* m_MouseOn = nullptr;
};

#endif