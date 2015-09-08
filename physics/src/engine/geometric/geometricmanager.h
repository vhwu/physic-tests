#ifndef GEOMETRICMANAGER_H
#define GEOMETRICMANAGER_H

#include "engine/common/manager.h"
#include <QString>
#include <vector>
#include "triangle.h"


class Ellipsoid;
class CollisionInfo;
class OBJ;
class GeometricManager: public Manager
{
public:
    GeometricManager(const QString &level, bool data);
    ~GeometricManager();

    void onTick(float seconds);
    void onDraw(Graphic *g);
    void drawTriangle(Graphic* g, int id);

    Triangle getTriangle(int id){return _triangles[id];}
    std::vector<Triangle> getTriangles(){return _triangles;}
    Vector3 collideTerrain(Entity* e);
private:
    std::vector<Triangle> _triangles;  //representation for collisions
    //Returns closest collision against _triangles,
    //if any are encountered when moving ellipsoid e by vector v
    CollisionInfo collideTriangles(Ellipsoid* e, const Vector3& v);
    //Collides() e by its _move, and resets _move based on result of slide()
    CollisionInfo sweep(Entity* e);

    OBJ* _obj;
    QString levelTexture;
};

#endif // GEOMETRICMANAGER_H
