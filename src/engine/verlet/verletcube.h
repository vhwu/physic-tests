#ifndef VERLETCUBE_H
#define VERLETCUBE_H

#include "verlet.h"

class VerletCube: public Verlet
{
public:
    //@param: opposite corners of the cube
    VerletCube(const Vector3& min, const Vector3& max, VerletManager* vm);
    ~VerletCube();
};

#endif // VERLETCUBE_H
