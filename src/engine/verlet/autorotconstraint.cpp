#include "autorotconstraint.h"

AutoRotConstraint::AutoRotConstraint(int i, Axis a, const Vector3 &center, float radius,
                                     float sp, Verlet *verlet):
    RotationConstraint(i,a,center,radius,verlet,false)
{
    speed = sp;
    angle = circle->getAngle(currentPos);
}

AutoRotConstraint::~AutoRotConstraint()
{}

void AutoRotConstraint::constrain(){
    Vector3 p = v->getPoint(index);

    //For non-specified axis: solve it, or defer solving to another constraint
    float s = solveAxis(axis,p.xyz[axis]);
    p.xyz[axis] = s;
    circle->setCenter(axis,s);

    angle+=speed;
    Vector3 a = circle->getPoint(angle);

    v->setPos(index,a);
    currentPos = a;
}

