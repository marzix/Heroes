#include "Texture.h"
#include "GameManager.h"

Texture::Texture( string file, string texName )
{   
    if( texName == "" )
    {
        m_Name = file;
    }
    else
    {
        m_Name = texName;
    }
    m_Center.x = 0;
    m_Center.y = 0;
}

Texture::Texture( string file, bool flag, string texName )
{
    m_Texture = IMG_LoadTexture( GameManager::GetSingleton().ren, file.c_str() );
    if( !m_Texture )
    {
        logSDLError( "LoadTexture" );
    }
    else
    {
        m_Center.x = 0;
        m_Center.y = 0;
        if( texName == "" )
        {
            m_Name = file;
        }
        else
        {
            m_Name = texName;
        }
    }
}

Texture::Texture( const Texture & rhs )
{
    m_Texture = rhs.m_Texture;
    m_Center = rhs.m_Center;
    m_Name = rhs.m_Name;
    m_Pixels = rhs.m_Pixels;
    m_Pitch = rhs.m_Pitch;
}

Texture::Texture( Texture && rhs )
{
    m_Texture = rhs.m_Texture;
    m_Center = rhs.m_Center;
    rhs.m_Texture = nullptr;
    m_Name = rhs.m_Name;
    m_Pixels = rhs.m_Pixels;
    m_Pitch = rhs.m_Pitch;
}

Texture::Texture( string file, string text, short int size, SDL_Color & color )
: m_Name( text )
{
	TTF_Font * font = TTF_OpenFont( file.c_str(), size );
	if( font == nullptr )
    {
		logSDLError( "TTF_OpenFont" );
	}

	SDL_Surface * surf = TTF_RenderText_Blended( font, text.c_str(), color );
	if( surf == nullptr )
    {
		TTF_CloseFont( font );
		logSDLError( "TTF_RenderText" );
	}
    
    m_Texture = SDL_CreateTextureFromSurface( GameManager::GetSingleton().ren, surf );
	if ( m_Texture == nullptr )
    {
		logSDLError( "CreateTextureFromSurface" );
	}

    m_Center.x = m_Center.y = 0;
	SDL_FreeSurface( surf );
	TTF_CloseFont( font );
}

Texture::~Texture()
{
    SDL_DestroyTexture( m_Texture );
}

Texture & Texture::operator=( const Texture & rhs )
{
    m_Texture = rhs.m_Texture;
    m_Center = rhs.m_Center;
    m_Pixels = rhs.m_Pixels; 
    m_Pitch = rhs.m_Pitch;
    m_Name = rhs.m_Name;

    return *this;
}

bool Texture::Load( bool mouseOver, bool flipped, SDL_BlendMode blendMode )
{
    if( m_Texture )
        return true;

    SDL_Surface * loadedSurface = IMG_Load( m_Name.c_str() );
    if( !loadedSurface )
    {
        logSDLError( "IMG_Load" );
        return false;
    }

    SDL_Surface * formattedSurface = 0;
    if( loadedSurface->format->BitsPerPixel != 32 )
    {
        formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0 );
    }
    else
        formattedSurface = loadedSurface;

    if( !formattedSurface )
    {
        logSDLError( "ConvertSurfaceFormat" );
        return false;
    }
    
    SDL_Rect newDimentions = CountDimentions( formattedSurface );
    KeyColor( formattedSurface, mouseOver, blendMode );
    if( formattedSurface->w != newDimentions.w || formattedSurface->h != newDimentions.h )
    {
        if( SDL_Surface * newSurface = ResizeSurface( formattedSurface, newDimentions ) )
        {
            SDL_FreeSurface( formattedSurface );
            formattedSurface = newSurface;
        }
    }
    if( flipped )
        FlipSurfaceVertically( formattedSurface );
    
    m_Texture = SDL_CreateTextureFromSurface( GameManager::GetSingleton().ren,  formattedSurface );

    if( !m_Texture )
    {
        logSDLError( "CreateTextureFromSurface" );
        return false;
    }

    SDL_FreeSurface( loadedSurface );
    SDL_FreeSurface( formattedSurface );
    
    SDL_SetTextureBlendMode( m_Texture , blendMode );
    return true;
}

SDL_Surface * Texture::ResizeSurface( SDL_Surface * src, SDL_Rect dst )
{
    SDL_Surface * newSurface = SDL_CreateRGBSurface
        ( 0, dst.w, dst.h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask );
    if(  !SDL_MUSTLOCK( src ) || SDL_LockSurface( src ) == 0  )
    {
        if( SDL_BlitSurface ( src, &dst, newSurface, nullptr ) < 0 )
        {
            logSDLError( "BlitSurface" );
            return false;
        }
    }
    return newSurface;
}

void Texture::KeyColor( SDL_Surface * formattedSurface, bool mouseOver, SDL_BlendMode blendMode )
{
    if( blendMode == SDL_BLENDMODE_BLEND && ( !SDL_MUSTLOCK( formattedSurface ) || SDL_LockSurface( formattedSurface ) == 0 ) )
    {
        
        Uint32 * pixels = (Uint32 *)formattedSurface->pixels;
        int pixelCount = ( formattedSurface->w ) * formattedSurface->h;

        for( int i = 0; i < pixelCount; ++i ) 
        {
            if( pixels[i] == eColor::VIOLET || pixels[i] == eColor::GREEN )
            {
                if( mouseOver )
                {
                    pixels[i] = eColor::YELLOW;
                }
                else
                {
                    pixels[i] = eColor::SHADOW;
                }
            }
            if( pixels[i] == eColor::CYAN || ( !mouseOver && pixels[i] == eColor::YELLOW ) )
            { 
                pixels[i] = eColor::_TRANSPARENT; 
            }
            else if( pixels[i] == eColor::MAGENTA || pixels[i] == eColor::LIGHT_MAGENTA )
            {
                pixels[i] = eColor::SHADOW;
            }
        } 

        if( SDL_MUSTLOCK( formattedSurface ) )
        {
            SDL_UnlockSurface( formattedSurface );
        }
    }
}

SDL_Rect Texture::CountDimentions( SDL_Surface * image )
{
    int firstOffsetX = 0, lastOffsetX = 0, firstOffsetY = 0, lastOffsetY = 0;
    const short int PIXEL_SIZE = 4;
    SDL_Rect dst;
    dst.w = dst.h = 0;
    if( !SDL_MUSTLOCK( image ) || SDL_LockSurface( image ) == 0 ) 
    {
        int temp;
        Uint32 * pixels = ( Uint32 * )image->pixels;
        int w = image->pitch / PIXEL_SIZE;
        int pixelCount = w * image->h;

        for( int i = 0; i < pixelCount; i++ ) 
        {
            if( pixels[i] != eColor::CYAN && firstOffsetY == 0 )
            { 
                 firstOffsetY = i / w;
            }   
            temp = pixelCount - i - 1;
            if( pixels[temp] != eColor::CYAN && lastOffsetY == 0 )
            {
                lastOffsetY = temp / w;
            }
            if( firstOffsetY != 0 && lastOffsetY != 0 )
            {
                dst.h = lastOffsetY - firstOffsetY;
                break;
            }
        } 
        for( int i = 0; i < w; i++ )
        {
            for( int j = 0; j < image->h; j++ )
            {
                temp = j * w + i;
                if( pixels[temp] != eColor::CYAN && firstOffsetX == 0 )
                {
                    firstOffsetX = i;
                }
                temp = pixelCount - ( j * w + i + 1 );
                if( pixels[temp] != eColor::CYAN && lastOffsetX == 0 )
                {
                    lastOffsetX = w - i;
                }
                if( firstOffsetX != 0 && lastOffsetX != 0 )
                {
                    dst.w = lastOffsetX - firstOffsetX;
                    break;
                }
            }
            if( dst.w != 0 )
            {
                break;
            }
        }
        m_Center.x = w / 2 - firstOffsetX;
        m_Center.y = image->h / 2 - firstOffsetY;
        dst.x = firstOffsetX;
        dst.y = firstOffsetY;
        if( SDL_MUSTLOCK( image ) )
        {
            SDL_UnlockSurface( image );
        }
    }
    return dst;
}

void Texture::FlipSurfaceVertically( SDL_Surface* image )
{
    if( !SDL_MUSTLOCK( image ) || SDL_LockSurface( image ) == 0 ) 
    {
        Uint32 * m_Pixels = ( Uint32 * )image->pixels; 
        for( int i = 0; i < image->h; i++ )
        {
            for( int j = 0; j < image->w / 2; j++ )
            {
                int front = i * image->w + j;
                int back = (i + 1) * image->w - j - 1;
                Uint32 temp = m_Pixels[front];
                m_Pixels[front] = m_Pixels[back];
                m_Pixels[back] = temp;
            }
        }
        if( SDL_MUSTLOCK( image ) )
            SDL_UnlockSurface( image );
    }
}

void Texture::CreateRenderTarget()
{
    auto& Game = GameManager::GetSingleton();
    if( m_Texture = SDL_CreateTexture( Game.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, eWindow::WIDTH, eWindow::HEIGHT ) )
    {
        SDL_SetRenderTarget( Game.ren, m_Texture );
    }
}

void Texture::RenderTarget()
{
    SDL_SetRenderTarget( GameManager::GetSingleton().ren, NULL );
    RenderTexture( nullptr );
}

void Texture::RenderTexture( SDL_Rect * dst, SDL_RendererFlip flip, double rotation )
{
    if( SDL_RenderCopyEx( GameManager::GetSingleton().ren, m_Texture, nullptr, dst, rotation, &m_Center, flip ) == -1 )
    {
        logSDLError( "RenderCopy" );
    }
}

void Texture::RenderTexture( SDL_Rect * dst, Uint8 alpha, SDL_RendererFlip flip )
{
    SDL_SetTextureAlphaMod( m_Texture, alpha );
    RenderTexture( dst, flip );
}

SDL_Texture * Texture::GetTex() const
{
    return m_Texture;
}

SDL_Point Texture::GetCenter()const
{
    return m_Center;
}

void * Texture::getPixels() const
{
    return m_Pixels;
}

int Texture::getPitch() const
{
    return m_Pitch;
}

void Texture::FreeTexture()
{
    SDL_DestroyTexture( m_Texture );
    m_Texture = nullptr;
}

string Texture::GetTexName()const
{
    return m_Name;
}

bool Texture::lockTexture() 
{ 
    bool success = true; 

    if( m_Pixels ) 
    {
        success = false; 
    }
    else 
    { 
        if( SDL_LockTexture( m_Texture, nullptr, &m_Pixels, &m_Pitch ) != 0 ) 
        { 
            logSDLError( "LockTexture" ); 
            success = false; 
        } 
    } 
    return success;
} 

bool Texture::unlockTexture() 
{ 
    bool success = true;

    if( m_Pixels == nullptr ) 
    {
        success = false; 
    }
    else 
    { 
        SDL_UnlockTexture( m_Texture ); 
        m_Pixels = nullptr; 
        m_Pitch = 0; 
    } 
    return success; 
}

void Texture::SetTexName( string n )
{
    m_Name = n;
}

void Texture::SetCenter( int x, int y )
{
    m_Center.x = x;
    m_Center.y = y;
}