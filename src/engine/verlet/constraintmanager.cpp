#include "constraintmanager.h"
#include "constraint.h"


ConstraintManager::ConstraintManager(): Manager()
{
}

ConstraintManager::~ConstraintManager()
{
    for (std::vector<Constraint*>::iterator it = constraints.begin() ;
         it != constraints.end(); ++it)
        delete (*it);
    constraints.clear();
}

void ConstraintManager::addConstraint(Constraint* c){
    constraints.push_back(c);
}

void ConstraintManager::onTick(float seconds){
    foreach(Constraint* c, constraints)
        c->onTick(seconds);
}

void ConstraintManager::onDraw(Graphic* g){
    foreach(Constraint* c, constraints)
        c->onDraw(g);
}

bool ConstraintManager::rayTrace(RayTracer* ray, HitTest &result){
    HitTest temp;
    for(Constraint* c: constraints){
        bool hit = false;
        if(c->getSelect())
            hit = ray->hitConstraint(c,temp);
        if(hit&&(temp.t<result.t))
            result = temp;
    }
    return result.hit;
    return false;
}

void ConstraintManager::setHover(Constraint *c){
    c->setHover(true);
}

void ConstraintManager::setSelection(Constraint *c){
    c->setSelect(true);
}

void ConstraintManager::resetHover(){
    for(Constraint* c: constraints)
        c->setHover(false);
}

void ConstraintManager::resetSelection(){
    for(Constraint* c: constraints)
        c->setSelect(false);
}
