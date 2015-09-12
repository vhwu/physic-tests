#ifndef ROTATIONCONSTRAINT_H
#define ROTATIONCONSTRAINT_H

#include "constraint.h"

//Constraint which limits point's movement to a circle of radius r on Axis a around center c
class RotationConstraint: public Constraint
{
public:
    RotationConstraint(int i, Axis a, const Vector3& c, float r,
                       Verlet* verlet,  bool s=false);
    virtual ~RotationConstraint();

    void constrain();
    void onDraw(Graphic* g);  //Visualizes arc of constraint
private:
    Vector3 center;
    float radius;
    float stationary; //Value of unchanging axis
};

#endif // ROTATIONCONSTRAINT_H
