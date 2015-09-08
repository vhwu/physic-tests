#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "vector.h"
#include "shape.h"

class Ellipsoid: public Shape
{
public:
    Ellipsoid(const Vector3& pos, const Vector3& dimension);
    ~Ellipsoid();

    void onDraw(Graphic* g);
    Collision collide(Shape* shape);
    Collision collideEllipsoid(Ellipsoid* e);
};

#endif // ELLIPSOID_H
