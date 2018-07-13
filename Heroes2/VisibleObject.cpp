#include "VisibleObject.h"
#include "Image.h"

SDL_RendererFlip VisibleObject::GetFlip()const
{
    return m_Flip;
}

ImagePtr VisibleObject::GetImage()const
{
    return m_CurImage;
}

int VisibleObject::GetY()const
{
    return m_Position.y;
}

int VisibleObject::GetHeightForSorting()const
{
    if( !m_CurImage )
        return 0;
    auto r = m_CurImage->GetRect();
    return r.y + r.h;
}

SDL_Point VisibleObject::GetPosition()const
{
    return m_Position;
}

void VisibleObject::SetImage( ImagePtr image )
{
    m_CurImage = image;
}

void VisibleObject::SetFlip( SDL_RendererFlip f )
{
    m_Flip = f;
}

void VisibleObject::SetPosition( int x, int y )
{
    m_Position.x = x;
    m_Position.y = y;
}

void VisibleObject::Render()
{
    m_CurImage->RenderTexture();
}