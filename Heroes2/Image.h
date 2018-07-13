#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "Texture.h"

class Image
{
    friend class Animation;
public:
    Image() = default;
    Image( string file, string texName = "" );
    Image( string file, bool flag, string texName = "" );
    Image( string file, string text, int size, SDL_Color c );
    Image( string file, int x, int y );
    Image( const Image & rhs );
    Image( const Image * img );

    Image & operator=( Image & );
    Image & operator=( Image && )=default;

    TexturePtr  GetTex();
    SDL_Rect    GetRect()const;
    string      GetTexName()const;
    SDL_Point   GetTexCenter()const;
    void        SetRect( int x, int y, const SDL_RendererFlip & flip = SDL_FLIP_NONE );
    void        SetRect( SDL_Rect rectangle );
    void        SetName( string name );
    void        SetTexture( string file, string text, int size, SDL_Color c );
    void        SetTextureCenter();
    void        SetTexCenter( int x, int y )const;

    void RenderTexture( const SDL_RendererFlip flip = SDL_FLIP_NONE, double rotation = 0.0f, SDL_Rect* dst = nullptr );
    void RenderTexture( Uint8 alpha, const SDL_RendererFlip flip = SDL_FLIP_NONE );
    bool Load( bool mouseOver = false, bool flipped = false, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND );

private:
    TexturePtr m_Texture;
    SDL_Rect m_DstRect;
};

#endif