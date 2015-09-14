#include "constraint.h"

Constraint::Constraint(int i, Axis a, Verlet* verlet, bool s)
{
    v = verlet;
    axis = a;
    index = i;

    selectable = s;
    selected = false;
    hovered = false;

    nextA=next(a);
    prevA=prev(a);
}

Constraint::~Constraint()
{}

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

float& Constraint::access(Axis a){
    return currentPos.xyz[a];
}

void Constraint::onDraw(Graphic* g){
    //Point: green if selectable, yellow if hovered, red if selected
    if(selectable){
        g->setColor(Vector3(0,1,0));
        if(hovered)
            g->setColor(Vector3(1,1,0));
        if(selected)
            g->setColor(Vector3(1,0,0));
        g->transform(&Graphic::drawUnitSphere,v->getPoint(index),0,
                     Vector3(.15,.15,.15));
    }
}

void Constraint::onTick(float ){}
