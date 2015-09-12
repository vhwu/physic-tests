#ifndef VERLETCONSTRAINT_H
#define VERLETCONSTRAINT_H

#include "constraint.h"

//Constraint which allows for links between 2 different verlets
class VerletConstraint: public Constraint
{
public:
    VerletConstraint(int i1, Verlet* verlet1, int i2, Verlet* verlet2, bool s=false);
    virtual ~VerletConstraint();

    void constrain();
    void onDraw(Graphic* g);  //Visualizes arc of constraint
private:
    Verlet* v2;
    int index2;
    float restLength;
    //precalculated for squareroot approximation
    float restLengthSq;
};

#endif // VERLETCONSTRAINT_H
