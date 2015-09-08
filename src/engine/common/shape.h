#ifndef SHAPE_H
#define SHAPE_H

#include "vector.h"
class Graphic;
struct Collision;
class Ellipsoid;

class Shape
{
public:
    Shape(const Vector3& pos, const Vector3& dimension);
    virtual ~Shape();
    Vector3 getPos(){return _pos;}
    void setPos(const Vector3& p){_pos=p;}
    Vector3 getDimension(){return _dim;}
    void translate(const Vector3& t);
    virtual void onDraw(Graphic* g)=0;

    virtual Collision collide(Shape* shape);
    virtual Collision collideEllipsoid(Ellipsoid* e);
protected:
    Vector3 _pos;
    Vector3 _dim;  //radius x height for cylinder
};

#endif // SHAPE_H


