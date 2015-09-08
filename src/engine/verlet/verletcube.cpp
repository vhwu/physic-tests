#include "verletcube.h"

VerletCube::VerletCube(const Vector3& min, const Vector3& max,
                       VerletManager* vm): Verlet(vm)
{
    //0: top far left
    //1: top far right
    //2: top near right
    //3: top near left
    //4: bot far left
    //5: bot far right
    //6: bot near right
    //7: bot near left
    createPoint(min);   //0
    createPoint(Vector3(max.x, min.y, min.z));   //1
    createPoint(Vector3(max.x, min.y, max.x));   //2
    createPoint(Vector3(min.x, min.y, max.z));   //3
    createPoint(Vector3(min.x, max.y, min.z));   //4
    createPoint(Vector3(max.x, max.y, min.z));   //5
    createPoint(max);   //6
    createPoint(Vector3(min.x, max.y, max.z));   //7

    //link top
    createLink(0,1);
    createLink(1,2);
    createLink(2,3);
    createLink(3,0);
    //link bot
    createLink(4,5);
    createLink(5,6);
    createLink(6,7);
    createLink(7,4);
    //link sides
    createLink(0,4);
    createLink(1,5);
    createLink(2,6);
    createLink(3,7);

    //cross top
    createLink(0,2);
    createLink(1,3);
    //cross bot
    createLink(4,6);
    createLink(5,7);
    //side1
    createLink(0,7);
    createLink(3,4);
    //side2
    createLink(2,5);
    createLink(1,6);
    //side 3
    createLink(3,6);
    createLink(2,7);
    //side 4
    createLink(0,5);
    createLink(1,4);

    //through middle
    createLink(0,6);
    createLink(1,7);
    createLink(2,4);
    createLink(3,5);
}

VerletCube::~VerletCube()
{

}

