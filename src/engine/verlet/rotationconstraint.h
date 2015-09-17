#ifndef ROTATIONCONSTRAINT_H
#define ROTATIONCONSTRAINT_H

#include "constraint.h"
#include "engine/common/circle.h"

//Constraint which limits point's movement to circle of radius r, normal of Axis a, around center c
class Circle;
class RotationConstraint: public Constraint
{
public:
    RotationConstraint(int i, Axis a, const Vector3& center, float radius,
                       Verlet* verlet,  bool s=false);
    virtual ~RotationConstraint();

    void constrain();
    void onDraw(Graphic* g);  //Visualizes arc of constraint

    Vector3 getNormal(Camera* );
    Circle* getCircle(){return circle;}
private:
    Circle* circle;
};

#endif // ROTATIONCONSTRAINT_H
