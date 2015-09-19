#ifndef AUTOROTCONSTRAINT_H
#define AUTOROTCONSTRAINT_H

#include "rotationconstraint.h"

//Constraint which limits point's movement to circle of radius r, normal of Axis a, around center c
//Automated
class AutoRotConstraint: public RotationConstraint
{
public:
    AutoRotConstraint(int i, Axis a, const Vector3& center, float radius,
                      float speed, Verlet* verlet);
    ~AutoRotConstraint();

    void constrain();
private:
    float speed;
    float angle;
};

#endif // AUTOROTCONSTRAINT_H
