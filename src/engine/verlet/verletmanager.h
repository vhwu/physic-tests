#ifndef VERLETMANAGER_H
#define VERLETMANAGER_H

#include "engine/common/manager.h"
#include <vector>
#include "link.h"
#include "verlet.h"
#include "rope.h"
#include "engine/common/raytracer.h"

class Ellipsoid;
class ConstraintManager;
class VerletManager: public Manager
{
public:
    VerletManager(World* w, ConstraintManager* cm);
    ~VerletManager();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void addVerlet(Verlet* v);
    Verlet* getVerlet(int i){return verlets[i];}

    Vector3 collideTerrain(Entity* e);
    void setPlayerInfluence(float f);
    bool rayTrace(RayTracer* ray, HitTest &result);

    //Settings
    //Vector3 gravity = Vector3(0,-1.5,0);
    Vector3 gravity = Vector3(0,0,0);
    int _numSolves =3;
    bool solve = false;
    void enableSolve(){solve = !solve;}
private:
    //Verlet objects manager maintains
    std::vector<Verlet*> verlets;
    //Handles the 'global' Constraints that user can manipulate
    ConstraintManager* _constraintManager;
    //Helpers for onTick, to cycle through each step
    //Updates positions of all points w/ velocity + acc
    void verlet(float seconds);
    //Adjusts positions to satisfy per-verlet constraints: linked and pin
    void constraints();

    //For collisions: how much verlets are influenced by entity's velocity upon collision
    float playerInfluence;
};

#endif // VERLETMANAGER_H
