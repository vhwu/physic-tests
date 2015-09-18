#ifndef TRANSLATIONCONSTRAINT_H
#define TRANSLATIONCONSTRAINT_H

#include "constraint.h"
enum Position
{
    BOT, MID, TOP
};

//Constraint which limits point's movement to a specified axis within a range (min-max)
class TranslationConstraint: public Constraint
{
public:
    TranslationConstraint(int i, Axis a, float range, Verlet* verlet, bool s=false,
                          Position p =MID);

    virtual ~TranslationConstraint();

    void constrain();
    void onDraw(Graphic* g);  //Visualizes range of constraint

    float getMin(){return min;}
    float getMax(){return max;}
private:
    Vector3 pinPos; //Original position, to reference for 2 axis that don't change
    float min;
    float max;
};

#endif // TRANSLATIONCONSTRAINT_H
