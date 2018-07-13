#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "VisibleObject.h"

class Dialog : public VisibleObject
{
public:
    Dialog();

    ImagePtr GetBackground();
    Dialog * GetNext();
    Dialog * GetPrev();
    void     SetSoundName( string fileName );

    bool    Quit( SDL_Event const & e )const;

protected:
    Dialog * m_Next = nullptr;
    Dialog * m_Previous = nullptr;

    string m_SoundTrackName;
};

#endif