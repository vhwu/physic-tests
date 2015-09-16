#ifndef SETUPCIRCLE_H
#define SETUPCIRCLE_H

#include "vector.h"
#include "engine/common/axis.h"
#include <vector>

//Circle for setting up verlet in levels
//Add to onDraw to visualize
class Graphic;

//Use for arranging verlets in a radial configuration
struct SetupInfo
{
    std::vector<Vector3> startPos;
    std::vector<float> angles;
    float triSize;

    SetupInfo(std::vector<Vector3> pos,std::vector<float> a, float size){
        startPos = pos;
        angles = a;
        triSize = size;
    }
};

class SetupCircle
{
public:
    SetupCircle(float r, const Vector3& c, Axis a);
    ~SetupCircle();

    float radius;
    Vector3 center;
    Axis axis;

    //Visualization
    void onDraw(Graphic* g); //0 degrees in gray, 'angle' in red    
    void setAngle(int a){angle=a;}

    Vector3 getPoint(float angle);
    SetupInfo positionVerlets(int numVerlets, float percentileVerlets, int numTri);
private:
    //Stored for convenience- the two axes used for calculations
    int prevA;
    int nextA;
    int angle;
};

#endif // SETUPCIRCLE_H
