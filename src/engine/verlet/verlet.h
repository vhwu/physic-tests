#ifndef VERLET_H
#define VERLET_H

#include <vector>
#include "link.h"
#include "vector.h"
#include <QHash>
#include <QPair>
#include "engine/common/graphic.h"

class VerletManager;
class GeometricManager;
class Graphic;
class Entity;
class TranslationConstraint;
class Constraint;

enum Axis
{
    X, Y, Z
};
struct Tri
{
    int a, b, c;
    Vector3 vertices[3];
    Vector3 normal;

    Tri(){}
    Tri(int _a, int _b, int _c){
        a = _a; b = _b; c= _c;
    }
};

class Verlet
{
public:
    Verlet(VerletManager* m);
    virtual ~Verlet();

    VerletManager* _manager;
    //Maximum points, for declaring arrays
    static const int NUM = 5000;
    //between 0 and 1: how much cloth is influenced by collisions
    float sphereInfluence = 1;
    Vector3 rayTraceSize = Vector3(.5,.5,.5);

    int getSize(){return numPoints;}
    Vector3 getPoint(const int& id){return _pos[id];}
    //Sets point at specified index to given pos
    void setPos(int index, const Vector3& pos);

    //CONSTRAINTS
    //Creation
    void createPin(int index); //Fix point at specified index to its _pos
    TranslationConstraint *createTranslate(int index, Axis a, float range, bool s=0); //Fix point at specified index to a range within one axis
    //Solving
    void linkConstraint();
    void pinConstraint();
    void translateConstraint();

    //Update
    virtual void onTick(float seconds);
    void verlet(float seconds);  //sets pos + prevPos
    Vector3 collide(Entity* e);

    //Draw
    virtual void onDraw(Graphic *g);
    void drawLink(Link* l, Graphic* g);
    void drawGradientLink(Link* l, Graphic* g);
protected:
    //Setting triangle vertices + normals after movement
    void calculate(Tri* t);

    //Creates new point (at index numPoints) w/ given position
    int createPoint(const Vector3& pos);
    //Specify indices of two pre-existing points
    virtual Link* createLink(int a, int b);

    //Actual number of points
    int numPoints = 0;

    //For calculating paths
    int row = 0;
    int col = 0;

    //Points
    Vector3 _pos[NUM];
    Vector3 _prevPos[NUM];
    Vector3 _acc[NUM];

    //Constraints
    typedef struct{
        int index;
        Vector3 pinPos;
    }Pin;
    std::vector<Pin> pins;
    std::vector<Link*> links;
    std::vector<TranslationConstraint*> constraints;
};

#endif // VERLET_H
