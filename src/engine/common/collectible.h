#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "engine/common/shape.h"
#include "vector.h"

class Shape;
class Entity;

//Represents a collectible object
class Collectible
{
public:
    Collectible();
    virtual ~Collectible();

    virtual void onTick(float seconds);
    virtual void onDraw(Graphic* g);
    //Called when Collectible is in range of Entity.
    //Default: sets collect + remove to true
    virtual void onCollect(Entity* e);

    Vector3 getPos(){return shape->getPos();}
    bool getRemove(){return remove;}
    bool getCollect(){return collect;}

    float range; //how far away Entity has to be to collect Collectible
protected:
    //Set to true after onCollect() is triggered
    bool collect;
    //Set to true when Collectible should be removed by its manager
    bool remove;
    Shape* shape;
};

#endif // COLLECTIBLE_H

