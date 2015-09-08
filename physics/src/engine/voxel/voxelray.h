#ifndef RAY_H
#define RAY_H

#include "vector.h"

class Camera;
class VoxelManager;
class Graphic;

//For raycasting. Include in onTick w/in world to update based on camera movement.
//Include 'traverse' w/in world's onTick for raycasting. Info storage: RayCastInfo
class VoxelRay
{
public:
    VoxelRay(Camera* c, VoxelManager* m);
    ~VoxelRay();

    //updates position based on camera, calls traverse to set intersect info
    void onTick(float seconds);

    //Returns point on ray, given length l
    Vector3 getPoint(int l);

    //Info about last intersection: absolute pos of block hit, and one prior to it
    Vector3 _hit, _prev;
    //face of _hit that was intersected
    int _face;
    //Whether there was an intersection this tick
    bool _intersect;
private:
    Vector3 _point,_direction;
    Camera* _cam;
    VoxelManager* _manager;

    //Raycasting range- how many units traverse() will look
    static const int range = 100;

    //Stores raycasting info in intersect, _hit, _prev, and _face
    void traverse();
};

#endif // RAY_H
