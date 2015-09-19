#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "vector.h"
#include "engine/verlet/verlet.h"
#include <vector>
#include <QList>

class Triangle;
class Ellipsoid;
class Constraint;

struct HitTest
{
    bool hit;
    float t;
    Vector3 normal;
    int id;  //for verlet collisions: point hit
    Triangle* tri; //for triangle + mesh collisions
    Verlet* v;  //for verlet collisions
    Constraint* c;  //for constraint collisions
    Tri* verletTri;

    HitTest() : hit(false), t(INFINITY), id(-1), tri(NULL), v(NULL) {}
    HitTest(bool h, float f, Triangle* tr){hit = h; t=f; tri=tr;}
};

class RayTracer
{
public:
    Vector3 source;
    Vector3 direction;

    //Cursor ray: to track what's under the cursor
    //@params: modelview from Camera, mouseX + mouseY from World
    RayTracer(double modelview[16], int mouseX, int mouseY);
    //Call each time camera moves, usually onTick in game-side world
    void update(double modelview[16], int mouseX, int mouseY);

    //Normal ray- cannot call getRayForPixel
    RayTracer(const Vector3& s, const Vector3& d);
    virtual ~RayTracer();

    Vector3 getPoint(float t) {return source+direction*t;}
    float hitPlane(const Vector3& point, const Vector3& normal);
    Vector3 getPointonPlane(const Vector3& point, const Vector3& normal);

    //If true is returned, 'result' is filled in with information about the collision
    bool hitAABB(const Vector3 &min, const Vector3 &max, HitTest &result);
    bool hitEllipsoid(Ellipsoid* e, HitTest &result);
    bool hitEllipsoid(const Vector3& pos, const Vector3& dim, int id, HitTest &result);
    bool hitMesh(std::vector<Triangle*> triangles, HitTest &result);
    bool hitTriangle(Triangle* t, HitTest &result);
    bool hitTriangle(Triangle t, HitTest &result);
    bool hitTriangle(Tri* t, HitTest &result);
    bool hitMesh(std::vector<Tri *> t, HitTest &result);
    bool hitMesh(std::vector<Triangle> t, HitTest &result);
    bool hitVerlet(Verlet* t, HitTest &result);
    bool hitVerlet(Verlet* t, QList<int> points, HitTest &result);
    bool hitConstraint(Constraint* c, HitTest &result);

private:
    //For cursor ray: returns direction for ray directly to screen coordinate
    Vector3 getRayForPixel(int x, int y) const;

    // Stores the viewport at the time of the constructor
    int viewport[4];
    // Stores a ray along each frustum corner at the time of the constructor
    Vector3 ray00, ray10, ray01, ray11;

    float solveQuadratic(float a, float b, float c);
};

#endif // RAYTRACER_H
