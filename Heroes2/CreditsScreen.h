#ifndef __CREDITS_H__
#define __CREDITS_H__

#include "Declarations.h"
#include "Dialog.h"

class CreditsScreen: public Dialog
{
public:
    CreditsScreen( string n );
    ~CreditsScreen();

    void Initialize()override;
    void Reset()override;
    void Actualize()override;
    void Render()override;
    void SetTextPos( int x, int y );

    const CreditsScreen & operator+=( ImagePtr & );

private:
    vector<ImagePtr> m_Credits;
};

#endif