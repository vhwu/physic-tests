#ifndef ALIGNCONSTRAINT_H
#define ALIGNCONSTRAINT_H

#include "constraint.h"

//Constraint, based on existing translation constraint (TC), which creates divisions
    //between its minimum + current position, for other constraints to point to as controls
//Note: does not control a point itself
class TranslationConstraint;
class AlignConstraint:public Constraint
{
public:
    AlignConstraint(int num, TranslationConstraint* t);
    ~AlignConstraint();

    void constrain();
    float translations[20];
private:
    TranslationConstraint* tc;
    int numDivisions;
};

#endif // ALIGNCONSTRAINT_H
