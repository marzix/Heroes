#include "Hero.h"
#include "Animation.h"
#include "Image.h"

Hero::Hero( Image heroBody, int x, int y )
{
    m_HeroBody = heroBody;
    m_Position.x = x;
    m_Position.y = y;
    m_HeroBody.SetRect( x, y, m_Flip );
}

void Hero::Initialize()
{
    m_HeroBody.SetRect( m_Position.x, m_Position.y );
    SetFlagPos();
}

void Hero::Actualize()
{
    if( !m_FlagAnim )
        return;
    if( m_FlagAnim->IsActive() )
        m_FlagAnim->Animate( *this );
    else
    {
        m_FlagAnim->Reset();
        m_FlagAnim->SetIsActive( true );
    }
    SetFlagPos();
}

void Hero::Render()
{
    m_CurImage->RenderTexture( m_Flip );
    m_HeroBody.RenderTexture( m_Flip );
}

void Hero::RenderDescriptor()
{
    m_HeroDescr.Render();
}

void Hero::Reset()
{
}

void Hero::SetFlagAnimation( AnimationPtr animation )
{
    m_FlagAnim = animation;
    m_CurImage = animation->m_Frames[0];
}

void Hero::SetFlagPos()
{
    SDL_Rect r = m_HeroBody.GetRect();
    if( m_Flip == SDL_FLIP_HORIZONTAL )
        m_FlagPos.x = r.x + r.w / 4;
    else
        m_FlagPos.x = r.x + r.w * 3 / 4;
    m_FlagPos.y = r.y + 35;
    m_CurImage->SetRect( m_FlagPos.x, m_FlagPos.y, m_Flip );
}

void Hero::SetHeroDescr( Descriptor& rDescr )
{
    auto r = m_HeroDescr.GetAvatarImage().GetRect();
    m_HeroDescr = rDescr;
    m_HeroDescr.SetDescriptorPosition( r.x, r.y );
    m_Name = rDescr.GetName();
}

void Hero::SetHeroDescrPos( int x, int y )
{
    m_HeroDescr.SetDescriptorPosition( x, y );
}

void Hero::SetBodyTex( Image body )
{
    m_HeroBody = body;
    m_HeroBody.SetRect( m_Position.x, m_Position.y );
}

Descriptor& Hero::GetHeroDescr()
{
    return m_HeroDescr;
}