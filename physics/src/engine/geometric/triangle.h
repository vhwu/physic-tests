#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include <QPair>
#include "engine/common/obj.h"
#include "collisioninfo.h"

struct Hitbox;
class Ellipsoid;
class Graphic;
class Triangle
{
public:
    Triangle();
    // Vertices must be in CCW order
    Triangle(Vector3 v1, Vector3 v2, Vector3 v3);
    Triangle(OBJ::Triangle t, OBJ* _obj);

    Vector3 vertices[3];
    Vector3 normal;
    std::vector<QPair<int,int> > edges;

    friend std::ostream& operator<<(std::ostream& os, const Triangle& t)
    {
        os << "Triangle:" << t.vertices[0] << "," << t.vertices[1] << "," << t.vertices[2];
        return os;
    }

    CollisionInfo collide(Ellipsoid* e, const Vector3& d);

    //Whether specified point (coplaner to triangle) is within triangle
    bool within(const Vector3& x);

    void onDraw(Graphic* g);
    void drawNormal(Graphic* g);
    void drawWithOutline(Graphic* g);

    //Adjusts min + max if any of triangle's points lie outside of them
    void adjustHitbox(Vector3& min, Vector3& max);

private:
    //Collision helpers
    CollisionInfo collideEdge(const Vector3& s, const Vector3& v, int a, int b);
    CollisionInfo collideVertex(int index, const Vector3& s, const Vector3& v);

    float signedDistTo(const Vector3& point);
    bool quadratic(float a, float b, float c, float* t);
    Triangle scale(Vector3 basis);
};

#endif // TRIANGLE_H
