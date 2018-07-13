#include "Descriptor.h"

Descriptor::Descriptor( Image n, Image a )
: m_Avatar( a )
, m_NameImage( n )
{}

const Image& Descriptor::GetNameImage()const
{
    return m_NameImage;
}

const Image& Descriptor::GetAvatarImage()const
{
    return m_Avatar;
}

void Descriptor::SetAvatarImage( Image & av )
{
    auto pos = m_Avatar.GetRect();
    m_Avatar = av;
    m_Avatar.SetRect( pos.x, pos.y );
}

void Descriptor::SetAvatarImagePos( int x, int y )
{
    m_Avatar.SetRect( x, y );
}

void Descriptor::SetNameImage( Image & n )
{
    auto pos = m_NameImage.GetRect();
    m_NameImage = n;
    m_NameImage.SetRect( pos.x, pos.y );
}

void Descriptor::SetNameImagePos( int x, int y )
{
    m_NameImage.SetRect( x, y );
}

void Descriptor::SetDescriptorPosition( int x, int y )
{
    m_Avatar.SetRect( x, y );
    x = x + m_Avatar.GetRect().w / 2 - m_NameImage.GetRect().w / 2;
    y += m_Avatar.GetRect().h + 2; 
    m_NameImage.SetRect( x, y );
}

bool Descriptor::operator==( const Descriptor & rDesc )
{
    if( m_NameImage.GetTexName() == rDesc.GetNameImage().GetTexName() )
        return true;
    else
        return false;
}

void Descriptor::Render()
{
    if( m_NameImage.GetTex() )
        m_NameImage.RenderTexture();
    m_Avatar.RenderTexture();

}

const string Descriptor::GetName()const
{
    return m_NameImage.GetTexName();
}
