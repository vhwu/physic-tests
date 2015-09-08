#ifndef MANAGER_H
#define MANAGER_H

#include <vector.h>
class Entity;
class Graphic;

class Manager
{
public:
    Manager();
    ~Manager();

    virtual void onTick(float seconds)=0;
    virtual void onDraw(Graphic *g)=0;
    virtual void onUI(Graphic *g);

    //Collision of terrain with entities- returns 'mtv'
    virtual Vector3 collideTerrain(Entity* e);

};

#endif // MANAGER_H
