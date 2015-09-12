#include "rotationconstraint.h"
#include <math.h>

RotationConstraint::RotationConstraint(int i, Axis a, const Vector3& c, float r,
                                       Verlet* verlet,  bool s):
    Constraint(i,a,verlet,s)
{
    center = c;
    radius = r;
    Vector3 point = v->getPoint(index);
    if(axis==X)
        stationary = point.x;
    else if(axis==Y)
        stationary = point.y;
    else
        stationary = point.z;
}

RotationConstraint::~RotationConstraint()
{}

void RotationConstraint::constrain(){
    Vector3 p = v->getPoint(index);
    float x = p.x;
    float y = p.y;
    float z = p.z;

    if(axis==X){
        x = stationary;
        float v1 = y - center.y;
        float v2 = z - center.z;
        float magV = sqrt(v1*v1 + v2*v2);
        float a1 = center.y + v1 / magV * radius;
        float a2 = center.z + v2 / magV * radius;
        y = a1;
        z = a2;
    }
    else if(axis==Y){
        y = stationary;
        float v1 = x - center.x;
        float v2 = z - center.z;
        float magV = sqrt(v1*v1 + v2*v2);
        float a1 = center.x + v1 / magV * radius;
        float a2 = center.z + v2 / magV * radius;
        x = a1;
        z = a2;
    }
    else{
        z = stationary;
        float v1 = x - center.x;
        float v2 = y - center.y;
        float magV = sqrt(v1*v1 + v2*v2);
        float a1 = center.x + v1 / magV * radius;
        float a2 = center.y + v2 / magV * radius;
        x = a1;
        y = a2;
    }
    v->setPos(index,Vector3(x,y,z));
}

void RotationConstraint::onDraw(Graphic* g){
    //Arc: gray if unselectable, green if selectable
    if(selectable)
        g->setColor(Vector3(0,1,0));
    else
        g->setColor(Vector3(.5,.5,.5));
    g->drawCircle(axis,radius,center);

    Constraint::onDraw(g);
}
