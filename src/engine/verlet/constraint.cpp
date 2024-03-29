#include "constraint.h"
#include "engine/common/camera.h"
Constraint::Constraint(int i, Axis a, Verlet* verlet, bool s)
{
    v = verlet;
    axis = a;
    index = i;

    selectable = s;
    visPoint = s;
    selected = false;
    hovered = false;

    nextA=next(a);
    prevA=prev(a);
}

Constraint::~Constraint()
{}

Vector3 Constraint::getNormal(Camera* c){
    return -1*c->getLook();
}

void Constraint::constrain(){}


float Constraint::solveAxis(Axis a, float orig){
    //1.Freeze, if 'selected': so that player selection/dragging isn't effected
    if(selected)
        return currentPos.xyz[a];

    //2.Defer, if cede[A]==true: keep (verlet.position.A) intact, so data isn't overwritten
    //before the constraint that A is pointing to can solve
    if(cede[a])
        return orig;

    //3.Otherwise, get value: *control[A]
    else
        return *control[a];
}

void Constraint::assign(Axis a, Constraint *c){
    //if both constraints have the same point, this one will defer solving Axis a
    bool match = (index==c->getIndex() && v==c->getVerlet());
    control[a]=&c->access(a);
    cede[a]=match;
}

void Constraint::assign(Axis a, float &f){
    control[a]=&f;
}

float& Constraint::access(Axis a){
    return currentPos.xyz[a];
}

void Constraint::onDraw(Graphic* g){
    //Point: yellow if selectable, magenta if hovered, teal if selected
    if(visPoint){
        if(selectable)
            g->setColor(color);
        else
            g->setColor(Vector3(.5,.5,.5));
        if(hovered)
            g->setColor(Vector3(1,0,1));
        if(selected)
            g->setColor(Vector3(1,1,0));

        g->transform(&Graphic::drawUnitSphere,v->getPoint(index),0,
                     Vector3(.15,.15,.15));
    }
}

void Constraint::onTick(float ){}
