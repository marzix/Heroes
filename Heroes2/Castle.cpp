#include "Castle.h"

UnitDescr::UnitDescr( string _name, int _attack, int _defense, int _life, int _speed, TYPE::Name _type, int _size, int _number )
: name( _name )
, unitType( _type )
, unitSize( _size )
{
    unitStats.speed = _speed;
    unitStats.lifeRemaining = 100;
    unitStats.attack = _attack;
    unitStats.defense = _defense;
    unitStats.lifePerUnit = _life;
    unitStats.lifeRemaining = _life;
    unitStats.quantity = _number;
}

Castle::Castle( Image n, Image castle, Image body )
: m_CastleDescr( n, castle )
, m_HeroBody( body )
{}

Descriptor& Castle::GetCastleDescr()
{
    return m_CastleDescr;
}

Descriptor& Castle::GetHero( unsigned int i )
{
    return m_Heroes[i];
}

vector<UnitDescr> & Castle::GetUnitDescrList()
{
    return m_UnitDescriptors;
}

const Image& Castle::GetHeroBody()const
{
    return m_HeroBody;
}

const string Castle::GetName()const
{
    return m_CastleDescr.GetName();
}

void Castle::AddHero( Descriptor h )
{
    m_Heroes.push_back( h );
}

void Castle::AddUnitDescr( UnitDescr uptr )
{
    m_UnitDescriptors.push_back( uptr );
}

void Castle::SetDescriptorPosition( int x, int y )
{
    m_CastleDescr.SetDescriptorPosition( x, y );
}

Descriptor* Castle::FindNextHero( const string name )
{
    for( uint i = 0; i < m_Heroes.size(); i++ )
    {
        if( m_Heroes[i].GetName().compare( name ) == 0 )
        {
            if( i + 1 != m_Heroes.size() )
                return &m_Heroes[i + 1];
            else
                return &m_Heroes[0];
        }
    }
    return nullptr;
}

Descriptor* Castle::FindPrevHero( const string name )
{
    for( uint i = 0; i < m_Heroes.size(); i++ )
    {
        if( m_Heroes[i].GetName().compare( name ) == 0 )
        {
            if( i > 0 )
                return &m_Heroes[i - 1];
            else
                return &m_Heroes[m_Heroes.size() - 1];
        }
    }
    return nullptr;
}