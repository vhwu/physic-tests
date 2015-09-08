#include "shape.h"
#include "collision.h"

Shape::Shape(const Vector3& pos, const Vector3& dimension)
{
    _pos = pos;
    _dim = dimension;
}

Shape::~Shape()
{

}

void Shape::translate(const Vector3& t){
    _pos+=t;
}

Collision Shape::collide(Shape* ){
    return Collision();}

Collision Shape::collideEllipsoid(Ellipsoid* ){
    return Collision();
}

