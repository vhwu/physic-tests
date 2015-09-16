#include "entity.h"
#include "ellipsoid.h"
#include "collision.h"

#include <iostream>

Entity::Entity(const Vector3& , const Vector3& ):
    _vel(Vector3(0,0,0)),
    _acc(Vector3(0,0,0))
{
}

Entity::Entity(Shape* s):_shape(s), _vel(Vector3(0,0,0)), _acc(Vector3(0,0,0)){
}

Entity::~Entity()
{
}

void Entity::onTick(float seconds)
{
    _vel += _acc*seconds;
    Vector3 translate =seconds*_vel + .5f*_acc*seconds*seconds;
    _toMove = translate;

    _acc = Vector3(0,0,0);
}

void Entity::onUI(Graphic* ){}

void Entity::onDraw(Graphic* g){
    if(visible)
        _shape->onDraw(g);
}

void Entity::applyAcc(const Vector3& a){
    _acc+=a;
}

void Entity::onCollide(Entity* , const Vector3& mtv){
    Vector3 toMove = this->getMove();
    toMove+=mtv;
   this->setMove(toMove);
}

Collision Entity::collide(Entity* e){
    Shape* s1 = this->getShape();
    Shape* s2 = e->getShape();
    return s1->collide(s2);
}

Ellipsoid* Entity::getEllipsoid(){
    return new Ellipsoid(getPos(), getDim());
}
