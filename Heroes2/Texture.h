#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Declarations.h"

class Texture
{
public:
    Texture() = default;
    Texture( const Texture & rhs );
    Texture( Texture && rhs );
    Texture( string file, string texName = "" );
    Texture( string file, bool flag, string texName = "" );
    Texture( string file, string text, short int size, SDL_Color & c );
    ~Texture();

    Texture & operator=( const Texture & rhs );

    SDL_Texture*    GetTex() const;
    SDL_Point       GetCenter()const;
    string          GetTexName()const;
    void            SetTexName( string n );
    void            SetCenter( int x, int y );

    bool Load( bool mouseOver, bool flipped, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND );
    void CreateRenderTarget();
    void RenderTarget();
    void RenderTexture( SDL_Rect * dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double rotation = 0.0f );
    void RenderTexture( SDL_Rect * dst, Uint8 alpha, SDL_RendererFlip flip = SDL_FLIP_NONE );
    void FreeTexture();

private:
    SDL_Surface*    ResizeSurface( SDL_Surface * src, SDL_Rect dst );
    SDL_Rect        CountDimentions( SDL_Surface * image );
    void            KeyColor( SDL_Surface * formattedSurface, bool mouseOver, SDL_BlendMode blendMode );
    void            FlipSurfaceVertically( SDL_Surface* image );

    bool    lockTexture(); 
    bool    unlockTexture(); 
    void*   getPixels() const; 
    int     getPitch() const;

    SDL_Point m_Center;   
    SDL_Texture * m_Texture = nullptr;
    void * m_Pixels = nullptr; 
    int m_Pitch = 0;
    string m_Name;
};

#endif