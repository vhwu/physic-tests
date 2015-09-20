#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"
#include "shape.h"
class Ellipsoid;
class Graphic;
struct Collision;

//Held within World's _entities list
//Gameside, retain pointer to specific entity if necessary to call
//it in specific input events, liked player
class Entity
{
public:
    Entity(const Vector3& pos= Vector3(0,0,0),
           const Vector3& dim= Vector3(0,0,0));
    Entity(Shape* s);
    virtual ~Entity();

    //Gets movement vector from _vel + _acc (reset) and stores in _toMove
    virtual void onTick(float seconds);
    virtual void onDraw(Graphic *g);
    virtual void onUI(Graphic *);

    //Getters
    Shape* getShape(){return _shape;}
    Vector3 getPos(){return _shape->getPos();}
    Vector3 getDim(){return _shape->getDimension();}
    Vector3 getMove(){return _toMove;}
    Vector3 getVel(){return _vel;}
    Vector3 getAcc(){return _acc;}
    Vector3 getMtv(){return _mtv;}

    Ellipsoid* getEllipsoid();

    //Setters
    void setPos(const Vector3& p){_shape->setPos(p);}
    void setVel(const Vector3& v){_vel = v;}
    void setAcc(const Vector3& a){_acc = a;}
    void setMove(const Vector3& move){_toMove = move;}
    void setMtv(const Vector3& mtv){_mtv = mtv;}
    void setVisible(bool v){visible = v;}

    //Additive
    void applyAcc(const Vector3& a);
    virtual void move(const Vector3& translate)=0;

    virtual void onCollide(Entity *, const Vector3& mtv);
    Collision collide(Entity* e);
    Vector3 verletAcc;
protected:
    Shape* _shape;
    Vector3 _vel, _acc;
    //For continuous collision detection:
    //Stores movement vector for manager to use in collide() calculations
    Vector3 _toMove;
    //Store mtv as 'callback'
    Vector3 _mtv;
    //Whether it'll be drawn
    bool visible = true;
};

#endif // ENTITY_H
