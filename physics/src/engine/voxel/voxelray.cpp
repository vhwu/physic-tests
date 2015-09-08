#include "voxelray.h"
#include "voxelmanager.h"
#include "constants.h"

#include <iostream>
#include <limits>

#include "engine/common/graphic.h"
#include "engine/common/camera.h"

using constants::getFloating;

VoxelRay::VoxelRay(Camera* c, VoxelManager* m)
{
    _cam = c;
    _manager = m;
}

VoxelRay::~VoxelRay()
{
}

Vector3 VoxelRay::getPoint(int l){
    return _point+Vector3(_direction.x*l, _direction.y*l, _direction.z*l);
}

void VoxelRay::onTick(float seconds){
    _point = _cam->getPos();
    _direction = _cam->getLook();
    traverse();
}

void VoxelRay::traverse(){
    Vector3 d=_direction;
    d.normalize();
    Vector3 p = _point;

    //piecewise direction
    int stepX = (d.x>0) ? 1 : -1;
    int stepY = (d.y>0) ? 1 : -1;
    int stepZ = (d.z>0) ? 1 : -1;

    //All other calculations are based off relative distance
    d.x = fabs(d.x);
    d.y = fabs(d.y);
    d.z = fabs(d.z);

    //t delta to span a cell
    float tDeltaX = (d.x==0) ? 0 : 1.0/d.x;
    float tDeltaY = (d.y==0) ? 0 : 1.0/d.y;
    float tDeltaZ = (d.z==0) ? 0 : 1.0/d.z;

    //integer block coordinates
    int prev_x = floor(p.x);
    int prev_y = floor(p.y);
    int prev_z = floor(p.z);

    //integer block coordinates for previous block
    int x = floor(p.x);
    int y = floor(p.y);
    int z = floor(p.z);

    //distance to next integer boundary. Between -1 and 1 (inclusive)
    //Dependent on direction - difference between pos + next boundary ray will hit
    float distX = (stepX>0) ? 1.0-getFloating(p.x):getFloating(p.x);
    distX = (distX == 0) ? -1: distX;
    float tMaxX = (d.x==0) ? std::numeric_limits<float>::max() : distX * tDeltaX * stepX;
    float distY = (stepY>0) ? 1.0-getFloating(p.y):getFloating(p.y);
    distY = (distY == 0) ? -1: distY;
    float tMaxY = (d.y==0) ? std::numeric_limits<float>::max() : distY * tDeltaY * stepY;
    float distZ = (stepZ>0) ? 1.0-getFloating(p.z):getFloating(p.z);
    distZ = (distZ == 0) ? -1: distZ;
    float tMaxZ = (d.z==0) ? std::numeric_limits<float>::max() : distZ * tDeltaZ * stepZ;

    bool intersect = false;

    //test for intersections
    int i = 0;
    for(i; i<range && !intersect; i++){
        prev_x = x;
        prev_y = y;
        prev_z = z;
        if(tMaxX<tMaxY && tMaxX<tMaxZ){
            tMaxX += tDeltaX;
            x += stepX;
        }
        else if( tMaxY<tMaxZ){
            tMaxY += tDeltaY;
            y += stepY;
        }
        else{
            tMaxZ += tDeltaZ;
            z += stepZ;
        }
        intersect = !(_manager->checkBlock(Vector3(x,y,z)));
    }
    //No intersection within range
    if(i==range)
        _intersect = false;
    //Intersection: determine face that was hit
    else{
        int face = NONE;
        if(x==prev_x+1) face = LEFT;
        else if(x==prev_x-1) face = RIGHT;
        else if(y==prev_y+1) face = BOT;
        else if(y==prev_y-1) face = TOP;
        else if(z==prev_z+1) face = BACK;
        else if(z==prev_z-1) face = FRONT;

        _intersect = true;
        _face = face;
        _hit = Vector3(x,y,z);
        _prev = Vector3(prev_x, prev_y, prev_z);
    }
}

