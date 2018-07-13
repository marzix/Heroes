#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Declarations.h"

class Object
{
public:
    Object() = default;
    virtual ~Object();

    virtual bool    IsActive()const;
    virtual string  GetName()const;
    virtual void    SetIsActive( bool act );
    virtual void    SetName( string name );
    
    virtual void Initialize() = 0;
    virtual void Reset() = 0;
    virtual void Actualize() = 0;

protected:
    bool m_IsActive = false;
    string m_Name;
};

#endif