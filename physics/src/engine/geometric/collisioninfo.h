#ifndef COLLISIONINFO_H
#define COLLISIONINFO_H

#include "vector.h"

struct CollisionInfo
{
public:
    CollisionInfo(float _t, const Vector3& _sContactPt, const Vector3& _sNormal,
                  const Vector3& _eContactPt, const Vector3& _eNormal);
    //assign sphere info, and write ellipsoid info manually
    CollisionInfo(float _t, const Vector3& _sContactPt, const Vector3& _sNormal);
    CollisionInfo(); //default, called if there was no collision
    ~CollisionInfo();

    float t;  //set to 0 if there was no collision
    Vector3 sphere_normal;
    Vector3 sphere_contactPt;
    Vector3 ellipsoid_normal;
    Vector3 ellipsoid_contactPt;
};

#endif // COLLISIONINFO_H
