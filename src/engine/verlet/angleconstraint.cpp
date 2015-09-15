#include "angleconstraint.h"
#include "rotationconstraint.h"
#include <math.h>

#define PI 3.14159265

AngleConstraint::AngleConstraint(int i, RotationConstraint* rc, bool s):
    Constraint(i,rc->getAxis(),rc->getVerlet(),s)
{
    this->rc=rc;
    control[axis]=&rc->access(axis);

    //Calculate angle between two points
    Vector3 p1_3 = v->getPoint(i);
    Vector3 p2_3 = rc->getPos();
    Vector2 p1 = Vector2(p1_3.xyz[prevA],p1_3.xyz[nextA]);
    Vector2 p2 = Vector2(p2_3.xyz[prevA],p2_3.xyz[nextA]);

    float angle = atan((p1.y - p2.y) / (p2.x - p1.x)) * (180 / PI);
    std::cout<<angle<<std::endl;
}

AngleConstraint::~AngleConstraint()
{}

void AngleConstraint::constrain(){

}

