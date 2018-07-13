#include "Object.h"

Object::~Object() {}

bool Object::IsActive()const
{
    return m_IsActive;
}

string Object::GetName()const
{
    return m_Name;
}

void Object::SetIsActive( bool act )
{
    m_IsActive = act;
}

void Object::SetName( string name )
{
    m_Name = name;
}

