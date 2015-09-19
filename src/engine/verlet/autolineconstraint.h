#ifndef AUTOLINECONSTRAINT_H
#define AUTOLINECONSTRAINT_H

#include "translationconstraint.h"

//Constraint which limits point's movement to a specified axis within a range (min-max)
//Automated
class AutoLineConstraint: public TranslationConstraint
{
public:
    AutoLineConstraint(int i, Axis a, float range, float speed, Verlet* verlet,
                       Position p =MID);
    ~AutoLineConstraint();

    void constrain();
private:
    float translation;
    float speed;
};

#endif // AUTOLINECONSTRAINT_H
