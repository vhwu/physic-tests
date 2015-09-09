#include "translationconstraint.h"
#include "engine/common/graphic.h"

TranslationConstraint::TranslationConstraint(int i, Axis a, float _min, float _max, Verlet *verlet)
{
    index = i;
    axis = a;
    min = _min;
    max = _max;
    v = verlet;
    pinPos= v->getPoint(index);
}

TranslationConstraint::TranslationConstraint(int i, Axis a, float range, Verlet* verlet){
    index = i;
    axis = a;
    v = verlet;
    pinPos= v->getPoint(index);

    float pos;

    if(axis==X)
        pos = pinPos.x;
    else if(axis==Y)
        pos = pinPos.y;
    else
        pos = pinPos.z;

    min = pos-range;
    max = pos+range;
}

void TranslationConstraint::constrain(){
    Vector3 p = v->getPoint(index);
    if(axis==X)
        v->setPos(index, matchAxis(clamp(p.x)));
    else if(axis==Y)
        v->setPos(index, matchAxis(clamp(p.y)));
    else
        v->setPos(index, matchAxis(clamp(p.z)));
}

float TranslationConstraint::clamp(float value){
    float v = value;
    if(v<min)
        v=min;
    if(v>max)
        v=max;
    return v;
}

Vector3 TranslationConstraint::matchAxis(float value){
    if(axis==X)
        return Vector3(value,pinPos.y,pinPos.z);
    else if(axis==Y)
        return Vector3(pinPos.x,value,pinPos.z);
    else
        return Vector3(pinPos.x,pinPos.y,value);
}

void TranslationConstraint::onDraw(Graphic* g){
    g->setColor(Vector4(1,1,1,.5));
    g->transform(&Graphic::drawUnitSphere,v->getPoint(index),0,
        Vector3(.15,.15,.15));
    g->drawGradientLine(Vector3(1,0,0),Vector3(0,1,0),matchAxis(max),matchAxis(min));
}

TranslationConstraint::~TranslationConstraint()
{

}

