#include "geometricmanager.h"
#include "engine/common/obj.h"
#include "engine/common/ellipsoid.h"
#include "engine/common/graphic.h"
#include "engine/common/raytracer.h"
#include "engine/common/entity.h"

GeometricManager::GeometricManager(World* w, const QString &level, bool data): Manager(w)
{
    _obj = new OBJ();
    QString prefix = ":/levels/level/level_";
    levelTexture = level;

    //Read in level data
    if(data)
        _obj->read(prefix+level+".obj");
    else //only obj data, no level texture
        _obj->read(level);

    //create triangle representation for collision
    for(int i = 0; i<_obj->triangles.size(); i++){
        OBJ::Triangle t = _obj->triangles.at(i);
        Triangle tri = Triangle(t, _obj);
        _triangles.push_back(tri);
    }
}

GeometricManager::~GeometricManager()
{
    delete _obj;
}

void GeometricManager::onTick(float ){}

void GeometricManager::onDraw(Graphic *g){
    //Draw world's geometry
    g->setColor(Vector4(1,1,1,1));
    g->bindTexture(levelTexture);
    _obj->draw();
    //foreach(Triangle t, bot->_triangles)
    //    t.onDraw(g);
    g->unbindTexture();
}

void GeometricManager::drawTriangle(Graphic* g, int id){
    _triangles[id].onDraw(g);

}

//***********************Collisions************************//
Vector3 GeometricManager::collideTerrain(Entity* e){
    //Horizontal + vertical components handled separately
    Vector3 toMove = e->getMove();
    Vector3 horizontal = Vector3(toMove.x, 0, toMove.z);
    Vector3 vertical = Vector3(0, toMove.y, 0);

    //horizontal
    e->setMove(horizontal);
    for(int i=0; i<3;i++)
        sweep(e);

    //vertical- use to set mtv for detecting if entity is on ground
    e->setMove(vertical);
    return sweep(e).ellipsoid_normal;
}

CollisionInfo GeometricManager::sweep(Entity* e){
    //get collision
    Vector3 move = e->getMove();
    CollisionInfo c = collideTriangles(e->getEllipsoid(), e->getMove());

    //if there's a collision, find the new displacement by projecting the remaining velocity
    if(c.t!=0){
        //With ramp hack
        Vector3 newVel;
        Vector3 remaining = move*(1-c.t);
        Vector3 normal = c.ellipsoid_normal;
        normal.normalize();
        Vector3 up = Vector3(0,1,0);
        if(up.dot(normal)==0)
            newVel = remaining-(remaining.dot(normal))*normal;
        else{
            float scalar = normal.dot(remaining)/ normal.dot(up);
            Vector3 direction = remaining-up*scalar;
            float length = (remaining - (normal.dot(remaining))*normal).length();
            newVel = direction;
            newVel.normalize();
            newVel*=length;
        }

        //Move and set the new displacement
        Vector3 toMove = e->getMove()*c.t;
        e->move(toMove);
        e->move(.001*c.ellipsoid_normal);
        e->setMove(newVel);
    }
    //If there's no collision, move the entity by its full displacement
    else{
        e->move(e->getMove());
        e->setMove(Vector3(0,0,0));
    }
    return c;
}

CollisionInfo GeometricManager::collideTriangles(Ellipsoid* e, const Vector3& v){
    CollisionInfo least = CollisionInfo();

    for (Triangle t: _triangles){
        CollisionInfo c = t.collide(e, v);
        if(c.t>0 && (least.t==0||c.t<least.t)){
            least = c;
        }
    }
    return least;
}

