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
class Bend;

struct Tri
{
    int a, b, c;
    Vector3 vertices[3];
    Link* edges[3];
    Vector3 normal;
    float windForce; //value between 0 + 1 representing wind influence

    Tri(){}
    Tri(int _a, int _b, int _c){
        a = _a; b = _b; c= _c;
    }

    bool operator == (const Tri &t)
        const {return (a==t.a&&b==t.b&&c==t.c);}

    void drawNormal(Graphic* g){
        Vector3 center = (vertices[0]+vertices[1]+vertices[2])/3.0;
        g->drawLine(center, center+normal);
    }

    void replaceLink(Link* orig, Link*& l){
        if(edges[0]==orig)
            edges[0]=l;
        if(edges[1]==orig)
            edges[1]=l;
        if(edges[2]==orig)
            edges[2]=l;
    }
    void replaceIndex(int index, int index2){
        if(a==index)
            a=index2;
        else if(b==index)
            b=index2;
        else if(c==index)
            c=index2;
        else
            std::cout<<"None to replace"<<std::endl;
    }
};

struct LinkMap{
    QList<Link*> links;
    QList<Bend*> crosses;
    QList<Bend*> shears;

    LinkMap(){}
};

class Verlet
{
public:
    Verlet(VerletManager* m);
    virtual ~Verlet();

    //Maximum points, for declaring arrays
    static const int NUM = 10000;
    VerletManager* _manager;
    //between 0 and 1: how much cloth is influenced by collisions
    float sphereInfluence = 1;
    Vector3 rayTraceSize = Vector3(.2,.2,.2);

    int getSize(){return numPoints;}
    Vector3 getPoint(const int& id){return _pos[id];}
    QPair<int,int> getCoord(const int& id);

    //Fix point at specified index to its _pos
    void createPin(int index);
    //Sets point at specified index to given pos
    void setPos(int index, const Vector3& pos);

    //Update
    virtual void onTick(float seconds);
    void verlet(float seconds);  //sets pos + prevPos
    Vector3 collide(Entity* e);

    //Solve individual constraints
    void boxConstraint(const Vector3& _boxMin, const Vector3& _boxMax);
    void linkConstraint();
    void pinConstraint();

    //Draw
    virtual void onDraw(Graphic *g);
    void drawLink(Link* l, Graphic* g);
    void drawGradientLink(Link* l, Graphic* g);
    void drawBend(Bend* b, Graphic* g);

    //Map from indices to links, for tearing
    //QHash<int, QList<Link*> > link_map;
    LinkMap _linkMap[NUM];
    virtual void removeLink(Link* l);
    virtual void tearLink(Link* l);
    Link* findLink(int a, int b);
protected:
    //Setting triangle vertices + normals after movement
    void calculate(Tri* t);
    //Calculate + apply wind based on normals + windMode
    void applyWind(Tri* t);

    //Creates new point (at index numPoints) w/ given position
    int createPoint(const Vector3& pos);
    //Specify indices of two pre-existing points
    virtual Link* createLink(int a, int b,bool sort = true);

    //Actual number of points
    int numPoints = 0;

    //For calculating paths
    int row = 0;
    int col = 0;

    //Points
    Vector3 _pos[NUM];
    Vector3 _prevPos[NUM];
    Vector3 _acc[NUM];
    Vector3 _normal[NUM];

    //Constraints
    typedef struct{
        int index;
        Vector3 pinPos;
    }Pin;
    std::vector<Pin> pins;
    std::vector<Link*> links;
};

#endif // VERLET_H
