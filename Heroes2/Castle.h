#ifndef __CASTLE_H__
#define __CASTLE_H__

#include "Descriptor.h"
#include "Unit.h"

struct UnitDescr
{
    UnitDescr( string _name, int _attack, int _defense, int _life, int _speed, TYPE::Name _type, int _size, int _number );
    Statistics unitStats;
    TYPE::Name unitType;
    string name;
    uint unitSize;
};

class Castle
{
public:
    Castle() = default;
    Castle( Image n, Image castle, Image heroBody );

    Descriptor&         GetCastleDescr();
    Descriptor&         GetHero( unsigned int i );
    const Image&        GetHeroBody()const;
    const string        GetName()const;
    Descriptor*         FindNextHero( const string name );
    Descriptor*         FindPrevHero( const string name );
    vector<UnitDescr>&  GetUnitDescrList();

    void AddHero( Descriptor h );
    void AddUnitDescr( UnitDescr );
    void SetDescriptorPosition( int x, int y );

private:
    Descriptor m_CastleDescr;
    Image m_HeroBody;
    vector<UnitDescr> m_UnitDescriptors;
    vector<Descriptor> m_Heroes;
};

#endif