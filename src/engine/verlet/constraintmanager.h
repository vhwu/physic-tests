#ifndef CONSTRAINTMANAGER_H
#define CONSTRAINTMANAGER_H

#include "engine/common/manager.h"
#include "engine/common/raytracer.h"
#include <vector>

class Constraint;

//Manages constraints
//Access to select/ deselect constraints
class ConstraintManager: public Manager
{
public:
    ConstraintManager(World *w);
    ~ConstraintManager();

    void onTick(float seconds);
    void onDraw(Graphic *g);
    void solveConstraints();

    void addConstraint(Constraint *c);
    Constraint* getConstraint(int c){return constraints[c];}

    //Raytraces points for all constraints, and returns whether there was a hit
    bool rayTrace(RayTracer* ray, HitTest &result);
private:
    //Verlet objects manager maintains
    std::vector<Constraint*> constraints;
};

#endif // CONSTRAINTMANAGER_H
