#include "rotationconstraint.h"
#include <math.h>

RotationConstraint::RotationConstraint(int i, Axis a, const Vector3& c, float r,
                                       Verlet* verlet,  bool s):
    Constraint(i,a,verlet,s)
{
    center = c;
    radius = r;  
    currentPos = v->getPoint(i);

    //By default, point the axis the player doesn't control to center
    control[a]=&center.xyz[a];
}

RotationConstraint::~RotationConstraint()
{}

void RotationConstraint::constrain(){
    Vector3 p = v->getPoint(index);

    //For non-specified axis: solve it, or defer solving to another constraint
    p.xyz[axis] = solveAxis(axis,p.xyz[axis]);

    //For two axes: ensure they're on the circumference of the rotation's circle
    float v1 = p.xyz[nextA] - center.xyz[nextA];
    float v2 = p.xyz[prevA] - center.xyz[prevA];
    float magV = sqrt(v1*v1 + v2*v2);
    float a1 = center.xyz[nextA] + v1 / magV * radius;
    float a2 = center.xyz[prevA] + v2 / magV * radius;

    //Commented out: selectable constraint will only move if it's selected
//    if(selectable&&!selected){
//        p.xyz[nextA]=currentPos.xyz[nextA];
//        p.xyz[prevA]=currentPos.xyz[prevA];
//    }
//    else{
        p.xyz[nextA] = a1;
        p.xyz[prevA] = a2;
//    }

    v->setPos(index,p);
    currentPos = p;
    center.xyz[axis]=currentPos.xyz[axis]; //update center, for visualization
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
