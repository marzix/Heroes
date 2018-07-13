#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include "Declarations.h"
#include "Image.h"

class Descriptor
{
public:
    Descriptor() = default;
    Descriptor( Image n, Image a );

    const Image& GetNameImage()const;
    const Image& GetAvatarImage()const;
    void SetNameImage( Image & n );
    void SetNameImagePos( int x, int y );
    void SetAvatarImage( Image & av );
    void SetAvatarImagePos( int x, int y );
    //sets avatar in the x y position and the name
    //underneath and centered
    void SetDescriptorPosition( int x, int y );
    bool operator==( const Descriptor & );
    void Render();
    const string GetName()const;

private:
    Image m_NameImage;
    Image m_Avatar;
};

#endif