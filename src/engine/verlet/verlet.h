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

    //Maximum points, for declaring arrays
    static const int NUM = 5000;
    VerletManager* _manager;
    //between 0 and 1: how much cloth is influenced by collisions
    float sphereInfluence = 1;
    Vector3 rayTraceSize = Vector3(.2,.2,.2);

    int getSize(){return numPoints;}
    Vector3 getPoint(const int& id){return _pos[id];}

    //CONSTRAINTS
    //Fix point at specified index to its _pos
    void createPin(int index);
    //Fix point at specified index to a range within one axis
    void createTranslate(int index, Axis a, float range);

    //Selection
    void addSelect(int index){
        selectable.push_back(index);
    }
    bool canSelect(int index){
        return (std::find(selectable.begin(), selectable.end(), index) != selectable.end());
    }

    //Sets point at specified index to given pos
    void setPos(int index, const Vector3& pos);

    //Update
    virtual void onTick(float seconds);
    void verlet(float seconds);  //sets pos + prevPos
    Vector3 collide(Entity* e);

    //Solve individual constraints
    void linkConstraint();
    void pinConstraint();
    void translateConstraint();

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
    std::vector<TranslationConstraint> t_constraints;
    std::vector<int> selectable;
};

#endif // VERLET_H
