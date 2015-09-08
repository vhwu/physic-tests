#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "verlet.h"
#include <vector>

class Bend;
struct Connected{
     QList<Tri*> connectedTri;
     QList<Link*> connectedLink;
     QList<int> points;
     Connected(){
         connectedTri = QList<Tri*>();
         connectedLink = QList<Link*>();
         points = QList<int>();
     }
};
class TriangleMesh: public Verlet
{
public:
    TriangleMesh(const Vector2& dimension, float width,
                 const Vector3& start, VerletManager* vm,
                 bool shear = true, bool tearable = false);
    ~TriangleMesh();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    float _scalar[NUM]; //for triangleMesh: 1/numTriangles, for averaging

    //Normal-based: force per-particle based on normal of triangle in relation to wind
    //Averaged: force per-particle is averaged depending on how many triangles it belongs
        //to, so that central particles don't receive more force
    //0: normal-based, non-averaged: works best for creating surface for player
    //1: normal-based, averaged: most physically accurate
    //2: no normals, non-average
    //3: no normals, averaged
    int windMode = 0;

    //Shear
    Bend *createShear(int a, int b, int c, Link* seg1 = NULL, Link* seg2 = NULL);
    void removeShear(Bend *l);

    //Tear
    bool tearable;


    //Duplicates l and any shears over it, then calls insertPoint if necessary
    void tearLink(Link* l);
    //returns list of torn edges a is connected to, not those equivalent to l
    bool checkTorn(int a);
    //Duplicates index so edges can be torn apart
    //void insertPoint(int index_a, Tri* t1, Link* l1, Tri* t2, Link* l2);
    int insertPoint(int index_a, Connected &c1, Connected &c2);
    //copies all data from index into a new point at returned index
    int duplicatePoint(int index_a);
    //Finds all links + triangles adjoining l1 and attached to index
    Connected findConnecting(int index_a, Tri*& t1, Link*& l1);
    void handleShears(bool tear, QList<Bend*> duplicates, Link* l,
                          QList<Link*> l1_links, QList<Link*> l2_links);
    bool assignLink(QList<Link*> connected, Bend* b, Link *l);

    void findConnecting(int index_a, Tri*& t1, Link*& l1, QList<Tri*>& triangles,
                         QList<Link*>& links, QList<int>& points);
    int insertPoint(int index_a, Tri* t1, Link* l1, Tri* t2, Link* l2,Connected c1, Connected c2);

private:
    //Called in constructor to add triangles
    void triangulate(int a, int b, int c);

    std::vector<Tri*> _triangles;
    QHash<int, int> numTri; //how many triangles each index is part of, for averaging

    //For tearing
    QHash<Link*, QList<Bend*> > link_to_shear;
    QHash<Link*, QList<Tri*> > link_to_tri;

    //Per-vertex normals
    void drawVertexNormals(Graphic *g);

    //Get this up while we still remember everything
    void visualizeTear(int index, int index2, const Connected& c1, const Connected& c2);
    void drawTear(Graphic* g);

    //Visualizations
    QList<Tri*> connected_t1;
    QList<Link*> connected_l1;
    QList<Tri*> connected_t2;
    QList<Link*> connected_l2;
    QList<Bend*> cross_a;
    QList<Bend*> cross_b;
    QList<Bend*> shear_a;
    QList<Bend*> shear_b;

    Link* newLink1 = NULL;
    Link* newLink2 = NULL;
};

#endif // TRIANGLEMESH_H
