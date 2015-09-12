#include "constraint.h"

Constraint::Constraint(int i, Axis a, Verlet* verlet, bool s)
{
    v = verlet;
    axis = a;
    index = i;

    selectable = s;
    selected = false;
    hovered = false;
}

Constraint::~Constraint()
{}

void Constraint::constrain(){}

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
