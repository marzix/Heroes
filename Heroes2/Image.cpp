#include "Image.h"

Image::Image( string file, string texName )
{
    m_Texture = make_shared<Texture>( file, texName );
    m_DstRect.x = m_DstRect.y = 0;
}

Image::Image( string file, bool flag, string texName )
{
    m_Texture = make_shared<Texture>( file, flag, texName );
    if( m_Texture->GetTex() )
    {
        SDL_QueryTexture( m_Texture->GetTex(), nullptr, nullptr, &m_DstRect.w, &m_DstRect.h );
    }
    m_DstRect.x = m_DstRect.y = 0;
}

Image::Image( string file, string text, int size, SDL_Color c )
{
    m_Texture = make_shared<Texture>( file, text, size, c );
    if( m_Texture->GetTex() )
    {
        SDL_QueryTexture( m_Texture->GetTex(), nullptr, nullptr, &m_DstRect.w, &m_DstRect.h );
    }
    m_DstRect.x = m_DstRect.y = 0;
}

Image::Image( string file, int x, int y )
{
    m_Texture = make_shared<Texture>( file );
    m_DstRect.x = x;
    m_DstRect.y = y;
    SDL_QueryTexture( m_Texture->GetTex(), nullptr, nullptr, &m_DstRect.w, &m_DstRect.h );
}

Image::Image( const Image & rhs )
{
    m_Texture = rhs.m_Texture;
    m_DstRect = rhs.m_DstRect;
}

Image::Image( const Image * img )
{
    if( img )
    {
        m_Texture = img->m_Texture;
        m_DstRect = img->m_DstRect;
    }
}

Image & Image::operator=( Image & rhs )
{
    m_Texture = rhs.m_Texture;
    m_DstRect = rhs.m_DstRect;

    return *this;
}

bool Image::Load( bool mouseOver, bool flipped, SDL_BlendMode blendMode )
{
    if( m_Texture && m_Texture->Load( mouseOver, flipped, blendMode ) )
    {
        SDL_QueryTexture( m_Texture->GetTex(), nullptr, nullptr, &m_DstRect.w, &m_DstRect.h );
        return true;
    }
    else
    {
        return false;
    }
}

void Image::SetName( string name )
{
    if( m_Texture )
        m_Texture->SetTexName( name );
}

TexturePtr Image::GetTex()
{
    return m_Texture;
}

SDL_Rect Image::GetRect()const
{
    return m_DstRect;
}

void Image::SetRect( int x, int y, const SDL_RendererFlip & flip )
{
    if( !m_Texture )
        return;
    if( flip == SDL_FLIP_NONE ) 
    {
        m_DstRect.x = x - m_Texture->GetCenter().x;
    }
    else
    {
        m_DstRect.x = x - ( m_DstRect.w - m_Texture->GetCenter().x );
    }
    m_DstRect.y = y - m_Texture->GetCenter().y;
}

void Image::SetRect( SDL_Rect rectangle )
{
    m_DstRect = rectangle;
}

void Image::RenderTexture( const SDL_RendererFlip flip, double rotation, SDL_Rect* dst )
{
    if( !dst )
        dst = &m_DstRect;
    m_Texture->RenderTexture( dst, flip, rotation );
}

void Image::RenderTexture( Uint8 alpha, const SDL_RendererFlip flip )
{
    m_Texture->RenderTexture( &m_DstRect, alpha, flip );
}

string Image::GetTexName()const
{
    return m_Texture->GetTexName();
}

SDL_Point Image::GetTexCenter()const
{
    return m_Texture->GetCenter();
}

void Image::SetTexture( string file, string text, int size, SDL_Color c )
{
    if( m_Texture->GetTex() )
    {
        m_Texture->FreeTexture();
    }
    m_Texture = make_shared<Texture>( file, text, size, c );
    SDL_QueryTexture( m_Texture->GetTex(), nullptr, nullptr, &m_DstRect.w, &m_DstRect.h );
}

void Image::SetTextureCenter()
{
    m_Texture->SetCenter( m_DstRect.w / 2, m_DstRect.h / 2 );
}

void Image::SetTexCenter( int x, int y )const
{
    m_Texture->SetCenter( x, y );
}
