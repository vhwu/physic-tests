#include "circle.h"
#include "engine/common/graphic.h"
#include <math.h>

#define PI 3.14159265

Circle::Circle(float r, const Vector3 &c, Axis a)
{
    radius = r;
    center = c;
    axis = a;

    prevA = prev(a);
    nextA = next(a);
}

Circle::~Circle()
{

}

Vector3 Circle::getPoint(float angle){
    Vector3 point = center;
    point.xyz[prevA] = center.xyz[prevA]+(radius*sin(angle*PI/180));
    point.xyz[nextA] = center.xyz[nextA]+(radius*cos(angle*PI/180));
    return point;
}

void Circle::onDraw(Graphic* g){
    g->drawCircle(axis,radius,center);
}

float Circle::getAngle(const Vector3 &point){
    float angle = atan2(point.xyz[prevA] - center.xyz[prevA],
                            point.xyz[nextA] - center.xyz[nextA]);  //x and y switched?
    return angle*(180 / PI);
}

float Circle::getAngle(const Vector3 &p1, const Vector3 &p2){
    float angle = atan((p1.xyz[nextA] - p2.xyz[nextA]) / (p2.xyz[prevA] - p1.xyz[prevA])) * (180 / PI);
    angle*= 2; //not sure why this is necessary?
    return angle;
}

Vector3 Circle::onCircumference(const Vector3 &p){
    float v1 = p.xyz[nextA] - center.xyz[nextA];
    float v2 = p.xyz[prevA] - center.xyz[prevA];
    float magV = sqrt(v1*v1 + v2*v2);
    float a1 = center.xyz[nextA] + v1 / magV * radius;
    float a2 = center.xyz[prevA] + v2 / magV * radius;

    Vector3 toReturn = p;
    toReturn.xyz[nextA] = a1;
    toReturn.xyz[prevA] = a2;

    return toReturn;
}
