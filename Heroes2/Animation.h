#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Image.h"
#include "Object.h"

class Animation : public Object
{
public:
    Animation() = default;
    Animation( STATE::Type name, float frameTime, string soundFile );
    Animation( string name, float frameTime, string soundFile );
    ~Animation();

    void    Animate( VisibleObject& obj );
    int     Find( Image* current );

    virtual void Initialize()override;
    virtual void Reset()override;
    virtual void Actualize()override;

    const Animation & operator+=( ImagePtr & img );
 
    vector<ImagePtr> m_Frames;
    float m_FrameTime = 0.0f;
    float m_CurrentTime = 0.0f;
    uint m_CurrentFrame = 0;
    string m_SoundName;
};

#endif