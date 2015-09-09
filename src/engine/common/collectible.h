#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "engine/common/shape.h"
#include "vector.h"

class Shape;
class Entity;
class Collectible
{
public:
    Collectible();
    virtual ~Collectible();

    virtual void onTick(float seconds);
    virtual void onDraw(Graphic* g);
    virtual void onCollect(Entity* e);

    Vector3 getPos(){return shape->getPos();}
    bool remove(){return toRemove;}
    bool collect(){return toCollect;}

    float range; //how far entity has to be to collect this
protected:
    bool toCollect;
    bool toRemove;
    Shape* shape;
};

#endif // COLLECTIBLE_H

