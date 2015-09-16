#include "setupcircle.h"
#include "engine/common/graphic.h"
#include <math.h>

#define PI 3.14159265

SetupCircle::SetupCircle(float r, const Vector3 &c, Axis a)
{
    radius = r;
    center = c;
    axis = a;

    prevA = prev(a);
    nextA = next(a);
    angle = 0;
}

SetupCircle::~SetupCircle()
{}

Vector3 SetupCircle::getPoint(float angle){
    Vector3 point = center;
    point.xyz[prevA] = center.xyz[prevA]+(radius*sin(angle*PI/180));
    point.xyz[nextA] = center.xyz[nextA]+(radius*cos(angle*PI/180));
    return point;
}

SetupInfo SetupCircle::positionVerlets(int numVerlets, float percentileVerlets, int numTri){
    float interval = 360.0/(float)numVerlets; //degrees from start of one verlet to next
    float percent = interval*percentileVerlets; //degrees one verlet occupies along circle
    float distance = (getPoint(0)-getPoint(percent)).length(); //distance btwn the 2 points on the circle
    float triSize = distance/(float)numTri;

    std::vector<Vector3> startPos(numVerlets);
    std::vector<float> angle(numVerlets);

    for(int i=0; i<numVerlets; i++){
        float offset = percent*.5;
        startPos[i]=getPoint(i*interval);
        angle[i]=i*interval+offset;
    }

    SetupInfo toReturn = SetupInfo(startPos,angle,triSize);
    return toReturn;
}

void SetupCircle::onDraw(Graphic *g){
    g->setColor(Vector4(0,1,1,.5));
    g->drawCircle(axis,radius,center);

    //Visualize 0 degrees
    g->transform(&Graphic::drawUnitSphere,getPoint(0),0,
                 Vector3(.1,.1,.1));

    //Visualize set angle
    g->setColor(Vector3(1,0,0));
    g->transform(&Graphic::drawUnitSphere,getPoint(angle),0,
                 Vector3(.1,.1,.1));
}
