#include "translationconstraint.h"
#include "engine/common/graphic.h"

TranslationConstraint::TranslationConstraint(int i, Axis a, float range,
                                             Verlet* verlet, bool s,Position p):
    Constraint(i,a,verlet,s)
{
    pinPos= v->getPoint(index);
    currentPos = pinPos;
    float pos = pinPos.xyz[a];

    if(p==BOT){
        min = pos;
        max = pos+range*2;
    }
    else if(p==MID){
        min = pos-range;
        max = pos+range;
    }
    else{
        max = pos;
        min = pos-range*2;
    }

    //By default, point the two axes the player doesn't control to pinPos
    control[prevA]=&pinPos.xyz[prevA];
    control[nextA]=&pinPos.xyz[nextA];

    color = colors[0];
}

void TranslationConstraint::constrain(){
    Vector3 p = v->getPoint(index);

    //For 'specified' axis, have it:
    if(selected||!selectable)  //'freeze' if points is selectable but unselected
        p.xyz[axis] = clamp(p.xyz[axis],min,max);
    else                       //movable within range otherwise
        p.xyz[axis] = currentPos.xyz[axis];

    //For other two axes: solve, defer solving, or freeze if specified axis is selected
    p.xyz[nextA]=solveAxis((Axis)nextA,p.xyz[nextA]);
    p.xyz[prevA]=solveAxis((Axis)prevA,p.xyz[prevA]);

    v->setPos(index,p);
    currentPos = p;
}

void TranslationConstraint::onDraw(Graphic* g){
    //Range: gray if unselectable, yellow if selected, cyan if selectable
    if(visRange){
        Vector3 minRange = currentPos;
        minRange.xyz[axis]=min;
        Vector3 maxRange = currentPos;
        maxRange.xyz[axis]=max;

        if(selected)
            g->setColor(Vector3(1,1,0));
        else if(selectable)
            g->setColor(Vector3(0,1,1));
        else
            g->setColor(Vector3(.5,.5,.5));

        g->drawLine(minRange,maxRange);
    }
    Constraint::onDraw(g);
}

TranslationConstraint::~TranslationConstraint()
{

}

