#include "rope.h"
#include <iostream>

Rope::Rope(int units, const Vector3& start,
           const Vector3& interval, VerletManager* vm): Verlet(vm)
{
    for(int i = 0; i<units; i++)
        createPoint(start+i*interval);

    for(int i = 0; i<units-1; i++)
        createLink(i,i+1);

}

Rope::~Rope()
{

}

