#ifndef CONSTRAINTMANAGER_H
#define CONSTRAINTMANAGER_H

#include "engine/common/manager.h"
#include "engine/common/raytracer.h"
#include <vector>

class Constraint;
class ConstraintManager: public Manager
{
public:
    ConstraintManager();
    ~ConstraintManager();

    void onTick(float seconds);
    void onDraw(Graphic *g);
    void solveConstraints();

    void addConstraint(Constraint *c);
    //Raytraces points for all constraints, and returns whether there was a hit
    bool rayTrace(RayTracer* ray, HitTest &result);

    //SELECTION
    //Sets hover to true for given constraint
    void setHover(Constraint* c);
    //Turns off hover for all constraints
    void resetHover();
    //Sets select to true for given constraint
    void setSelection(Constraint* c);
    //Turns off selected for all constraints
    void resetSelection();
private:
    //Verlet objects manager maintains
    std::vector<Constraint*> constraints;
};

#endif // CONSTRAINTMANAGER_H
