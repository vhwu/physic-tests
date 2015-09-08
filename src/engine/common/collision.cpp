#ifndef COLLISION_H
#define COLLISION_H
#include "vector.h"

struct Collision
{
    bool collide;
    Vector3 mtv;
    Collision(){collide = false; mtv = Vector3(0,0,0);}
    Collision(Vector3 m){collide = true; mtv = m;}
};
#endif // COLLISION_H
