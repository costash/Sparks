#ifndef BFSQUEUEELEMENT_H_
#define BFSQUEUEELEMENT_H_

#include "Location.h"

/*
    struct for an element in the BFS Queue
*/
struct BfsQueueElement
{
    Location loc;
    int root;   //root index, starting point

    BfsQueueElement( Location l, int r)
    {
        loc = l;
        root = r;
    }

    BfsQueueElement()
    {
        loc = Location();
        root = -1;
    }
};

#endif
