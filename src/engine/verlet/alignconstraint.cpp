#include "alignconstraint.h"
#include "translationconstraint.h"

AlignConstraint::AlignConstraint(int num, TranslationConstraint *t):
    Constraint(0,t->getAxis(),t->getVerlet(),false)
{
    numDivisions = num;
    tc = t;
}

AlignConstraint::~AlignConstraint()
{

}

void AlignConstraint::constrain(){
    float currentHeight = tc->getPos().xyz[axis];
    float minHeight = tc->getMin();
    float difference = currentHeight-minHeight;
    float interval = 0;
    if(difference>0){
        interval = difference/numDivisions;
    }
    for(int i= 0; i<11;i++)
        translations[i]=minHeight+i*interval;
}
