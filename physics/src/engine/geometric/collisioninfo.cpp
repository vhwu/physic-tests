#include "collisioninfo.h"

CollisionInfo::CollisionInfo(float _t, const Vector3& _sContactPt, const Vector3& _sNormal,
                             const Vector3& _eContactPt, const Vector3& _eNormal)
{
    t = _t;
    sphere_normal = _sNormal;
    sphere_contactPt = _sContactPt;
    ellipsoid_normal = _eNormal;
    ellipsoid_contactPt = _eContactPt;
}

CollisionInfo::CollisionInfo(float _t, const Vector3& _sContactPt, const Vector3& _sNormal)
{
    t = _t;
    sphere_normal = _sNormal;
    sphere_contactPt = _sContactPt;
    ellipsoid_normal = Vector3(0,0,0);
    ellipsoid_contactPt = Vector3(0,0,0);
}

CollisionInfo::CollisionInfo()
{
    t = 0; //parameterized between 0 and 1
    sphere_normal = Vector3(0,0,0);
    sphere_contactPt = Vector3(0,0,0);
    ellipsoid_normal = Vector3(0,0,0);
    ellipsoid_contactPt = Vector3(0,0,0);
}


CollisionInfo::~CollisionInfo()
{

}

