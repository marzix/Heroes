#ifndef __VISIBLE_OBJECT_H__
#define __VISIBLE_OBJECT_H__

#include "Declarations.h"
#include "Object.h"

class VisibleObject : public Object
{
public:
    SDL_RendererFlip    GetFlip()const;
    ImagePtr            GetImage()const;
    int                 GetY()const;
    //necessary to establish the right render queue order
    virtual int         GetHeightForSorting()const;
    virtual SDL_Point   GetPosition()const;
    void                SetImage( ImagePtr image );
    void                SetFlip( SDL_RendererFlip flip );
    virtual void        SetPosition( int x, int y );

    virtual void        Render();
protected:
    SDL_Point m_Position;         //potrzebne dla jednostki - pozycja to y grida (¿eby y siê nie zmienia³ dla ró¿nych klatek animacji)
    ImagePtr m_CurImage;
    SDL_RendererFlip m_Flip = SDL_FLIP_NONE;
};

#endif