#ifndef CIRCLE_H
#define CIRCLE_H

#include "axis.h"

//Utility for circle calculations + visualization
class Graphic;
class Circle
{
public:
    Circle(float r, const Vector3& c, Axis a);
    ~Circle();

    void onDraw(Graphic* g);

    void setCenter(Axis a, float v){center.xyz[a]=v;}

    float getRadius(){return radius;}
    Vector3 getCenter(){return center;}
    Axis getAxis(){return axis;}

    Vector3 getPoint(float angle);
    float getAngle(const Vector3& point);
    float getAngle(const Vector3& p1, const Vector3& p2);
    Vector3 onCircumference(const Vector3& p);
private:
    float radius;
    Vector3 center;
    Axis axis;

    int nextA;
    int prevA;
};

#endif // CIRCLE_H
