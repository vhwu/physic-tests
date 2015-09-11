#include "verletmanager.h"
#include <iostream>
#include "engine/common/graphic.h"
#include "engine/common/raytracer.h"
#include "engine/common/entity.h"
#include "engine/geometric/geometricmanager.h"
#include "verletcube.h"

VerletManager::VerletManager(): Manager()
{}

VerletManager::~VerletManager()
{
    for (std::vector<Verlet*>::iterator it = verlets.begin() ;
         it != verlets.end(); ++it)
        delete (*it);
    verlets.clear();
}

bool VerletManager::rayTrace(RayTracer* ray, HitTest &result, bool selectable){
    HitTest temp;
    for(Verlet * v: verlets){
        bool hit;
        if(selectable)
            hit = ray->hitVerlet(v,v->getSelectable(),temp);
        else
            hit = ray->hitVerlet(v,temp);
        if(hit&&(temp.t<result.t))
            result = temp;
    }
    return result.hit;
}

void VerletManager::addVerlet(Verlet* v){
    verlets.push_back(v);
}

/*
void VerletManager::verlet(float seconds){
    for(Verlet * v: verlets)
        v->verlet(seconds);
}

void VerletManager::constraints(){
    for(Verlet * v: verlets){
        v->linkConstraint();
        v->pinConstraint();
        v->translateConstraint();
    }
}
*/

void VerletManager::onTick(float seconds){
    if(solve){

        foreach(Verlet* v, verlets){

            v->verlet(seconds);
            for(int i=0; i<_numSolves; i++){
                v->translateConstraint();
                v->linkConstraint();
                v->translateConstraint();
                v->pinConstraint();
            }
            v->onTick(seconds);
        }

//        verlet(seconds);
//        for(int i=0; i<_numSolves; i++)
//            constraints();
//        for(int i=0; i<verlets.size(); i++)
//            verlets.at(i)->onTick(seconds);

    }
}

void VerletManager::onDraw(Graphic *g){
    for(Verlet * v: verlets)
        v->onDraw(g);
}

Vector3 VerletManager::collideTerrain(Entity* e){
    Vector3 mtv = Vector3(0,0,0);
    for(Verlet * v: verlets)
        mtv += v->collide(e);
    return mtv;
}
