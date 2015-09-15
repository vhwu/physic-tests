#ifndef ANGLECONSTRAINT_H
#define ANGLECONSTRAINT_H

#include "constraint.h"

//Constraint, based on existing rotation constraint (RC), which keeps point at specified angle
//from RC's point
class RotationConstraint;
class AngleConstraint: public Constraint
{
public:
    AngleConstraint(int i, RotationConstraint *rc, bool s=false);
    ~AngleConstraint();

    void constrain();
private:
    float angle;
    RotationConstraint* rc;
};

#endif // ANGLECONSTRAINT_H
