#ifndef VERLETMANAGER_H
#define VERLETMANAGER_H

#include "engine/common/manager.h"
#include <vector>
#include "link.h"
#include "verlet.h"
#include "rope.h"
#include "engine/common/raytracer.h"

class Ellipsoid;
class VerletManager: public Manager
{
public:
    VerletManager();
    ~VerletManager();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void addVerlet(Verlet* v);
    Verlet* getVerlet(int i){return verlets[i];}

    Vector3 collideTerrain(Entity* e);
    bool rayTrace(RayTracer* ray, HitTest &result, bool selectable=false);

    //Settings
    //Vector3 gravity = Vector3(0,-1.5,0);
    Vector3 gravity = Vector3(0,0,0);
    int _numSolves =3;
    bool solve = false;
    void enableSolve(){solve = !solve;}
private:
    //Verlet objects manager maintains
    std::vector<Verlet*> verlets;

    //Helpers for onTick, to cycle through each step
    //Updates positions of all points w/ velocity + acc
    //void verlet(float seconds);
    //Adjusts positions to satisfy listed constraints
    //void constraints();
};

#endif // VERLETMANAGER_H
