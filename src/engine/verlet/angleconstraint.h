#ifndef ANGLECONSTRAINT_H
#define ANGLECONSTRAINT_H

#include "constraint.h"

//Constraint, based on existing rotation constraint (RC), which keeps point at constant angle from RC's point
//Note: cannot be used on points within same verlet- will cause system to go into perpetual motion
    //Possibility: have system apply angle offset to both points. May cause overwriting data problems
class RotationConstraint;
class AngleConstraint: public Constraint
{
public:
    AngleConstraint(int i, Verlet* v, RotationConstraint *rc, bool s=false);
    ~AngleConstraint();

    void constrain();
    void onDraw(Graphic *g);
    Vector3 getPoint(float angle);
private:
    float angle;
    RotationConstraint* rc;
};

#endif // ANGLECONSTRAINT_H
