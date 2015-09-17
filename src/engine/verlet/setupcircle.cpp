#include "setupcircle.h"
#include "engine/common/graphic.h"

SetupCircle::SetupCircle(float r, const Vector3 &c, Axis a)
{
    circle = new Circle(r,c,a);
    angle = 0;
}

SetupCircle::~SetupCircle()
{
    delete circle;
}

SetupInfo SetupCircle::positionVerlets(int numVerlets, float percentileVerlets, int numTri){
    float interval = 360.0/(float)numVerlets; //degrees from start of one verlet to next
    float percent = interval*percentileVerlets; //degrees one verlet occupies along circle
    float distance = (circle->getPoint(0)-circle->getPoint(percent)).length(); //distance btwn verlet's 2 points on the circle
    float triSize = distance/(float)numTri;

    std::vector<Vector3> startPos(numVerlets);
    std::vector<float> angle(numVerlets);

    for(int i=0; i<numVerlets; i++){
        float offset = percent*.5;
        startPos[i]=circle->getPoint(i*interval);
        angle[i]=i*interval+offset;
    }

    SetupInfo toReturn = SetupInfo(startPos,angle,triSize);
    return toReturn;
}

void SetupCircle::onDraw(Graphic *g){
    g->setColor(Vector4(0,1,1,.5));
    circle->onDraw(g);

    //Visualize 0 degrees
    g->transform(&Graphic::drawUnitSphere,circle->getPoint(0),0,
                 Vector3(.1,.1,.1));

    //Visualize set angle
    g->setColor(Vector3(1,0,0));
    g->transform(&Graphic::drawUnitSphere,circle->getPoint(angle),0,
                 Vector3(.1,.1,.1));
}
