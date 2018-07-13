#ifndef __SHOT_OBJECT_H__
#define __SHOT_OBJECT_H__

#include "VisibleObject.h"

class ShotObject;
using ShotObjectPtr = shared_ptr<ShotObject>;

struct vec2
{
    vec2() = default;
    vec2( float _x, float _y ): x(_x), y(_y) {}
    float   Length() { return sqrt( x * x + y * y ); }
    vec2  Normalized()
    {
        float len = Length();
        return vec2( x/len, y/len );
    };

    float x = 0.0f;
    float y = 0.0f;
};

class ShotObject: public VisibleObject
{
public:
    ShotObject() = default;
    ShotObject( Unit* owner, string name, SHOT_TYPE type, SDL_Point position );
    ShotObject( string ownerName, ImagePtr currImg, SHOT_TYPE type, vector<SDL_Point> positions );

    void Initialize()override;
    void Actualize()override;
    void Render()override;
    void Reset()override;
    void SetOwner( Unit* owner );
    void SetPosition( string animName );
    int  GetHeightForSorting()const override;

private:
    SHOT_TYPE m_ShotType = SHOT_TYPE::BULLET;
    float m_Speed = 0.0f;
    float m_CurrentTime = 0.0f;
    float m_Rotation = 0.0f;
    Unit* m_Owner = nullptr;
    //normalized vector of shot flight trajectory
    vec2 m_Trajectory;
    float m_LifeTime = 0.0f;
    vector<SDL_Point> m_Positions;
};

#endif
