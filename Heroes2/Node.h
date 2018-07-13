#ifndef __NODE_H__
#define __NODE_H__

#include "Grid.h"

struct Node
{
    NodePtr neighbours[6];
    int cost  = 0;
    NodePtr pPrev;
    Grid * pGrid = nullptr;
    bool IsOnOpen = false;
    
    void Reset();
};

#endif