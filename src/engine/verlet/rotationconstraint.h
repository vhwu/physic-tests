#ifndef ROTATIONCONSTRAINT_H
#define ROTATIONCONSTRAINT_H

#include "constraint.h"

//Constraint which limits point's movement to circle of radius r, normal of Axis a, around center c
class RotationConstraint: public Constraint
{
public:
    RotationConstraint(int i, Axis a, const Vector3& center, float radius,
                       Verlet* verlet,  bool s=false);
    virtual ~RotationConstraint();

    void constrain();
    void onDraw(Graphic* g);  //Visualizes arc of constraint
    Vector3 getCenter(){return center;}
    float getRadius(){return radius;}
private:
    Vector3 center;
    float radius;
};

#endif // ROTATIONCONSTRAINT_H
