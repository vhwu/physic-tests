#ifndef MANAGER_H
#define MANAGER_H

#include <vector.h>
#include <QMouseEvent>
#include <QKeyEvent>

class Entity;
class Graphic;
class World;

class Manager
{
public:
    Manager(World* w);
    virtual ~Manager();

    virtual void onTick(float seconds)=0;
    virtual void onDraw(Graphic *g)=0;
    virtual void onUI(Graphic *){}

    //Collision of terrain with entities- returns 'mtv'
    virtual Vector3 collideTerrain(Entity* ){return Vector3(0,0,0);}

    //Pass to specific entities, if necessary
    virtual void wheelEvent(QWheelEvent *){}
    virtual void mousePressEvent(QMouseEvent* ){}
    virtual void mouseMoveEvent(QMouseEvent* ){}
    virtual void mouseReleaseEvent(QMouseEvent* ){}

    virtual void keyPressEvent(QKeyEvent* ){}
    virtual void keyReleaseEvent(QKeyEvent* ){}
protected:
    World* world;
};

#endif // MANAGER_H
