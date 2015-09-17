#include "angleconstraint.h"
#include "rotationconstraint.h"

AngleConstraint::AngleConstraint(int i, Verlet* verlet, RotationConstraint* r, bool s):
    Constraint(i,r->getAxis(),verlet,s)
{
    rc = r;
    currentPos = verlet->getPoint(index);

    //by default, match unspecified axis w/ that of rc
    control[axis]=&rc->access(axis);

    //set to current angle between point of this constraint, and rc's point
    angle = rc->getCircle()->getAngle(v->getPoint(i),rc->getPos());
}

AngleConstraint::~AngleConstraint()
{}

void AngleConstraint::constrain(){
    Circle* c = rc->getCircle();
    Vector3 control = rc->getPos();

    float baseAngle = c->getAngle(control); //Find angle of rc's point
    float finalAngle = baseAngle + angle;   //Offset it
    Vector3 point = c->getPoint(finalAngle);

    //For non-specified axis: solve it, or defer solving to another constraint
    point.xyz[axis] = solveAxis(axis,point.xyz[axis]);

    v->setPos(index,point);
    currentPos = point;
}

