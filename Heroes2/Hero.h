#ifndef __HERO_H__
#define __HERO_H__

#include "VisibleObject.h"
#include "Descriptor.h"

class Hero;
using HeroPtr = shared_ptr<Hero>;

class Hero: public VisibleObject
{
public:
    Hero() = default;
    Hero( Image heroBody, int x, int y );

    void SetHeroDescr( Descriptor & rDescr );
    void SetHeroDescrPos( int x, int y );
    void SetFlagAnimation( AnimationPtr );
    void SetFlagPos();
    void SetBodyTex( Image );
    Descriptor& GetHeroDescr();

    void Initialize()override;
    void Actualize()override;
    void Reset()override;
    void Render()override;
    void RenderDescriptor();

private:
    AnimationPtr m_FlagAnim;
    Image m_HeroBody;
    SDL_Point m_FlagPos;
    Descriptor m_HeroDescr;
};

#endif