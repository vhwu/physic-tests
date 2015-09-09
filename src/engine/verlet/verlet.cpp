#include "verlet.h"
#include "verletmanager.h"
#include "link.h"
#include "engine/common/graphic.h"
#include "engine/common/raytracer.h"
#include "engine/geometric/geometricmanager.h"
#include "engine/common/entity.h"
#include "engine/common/ellipsoid.h"

#include <iostream>

Verlet::Verlet(VerletManager* m)
{
    _manager = m;
}

Verlet::~Verlet()
{
    for (std::vector<Link*>::iterator it = links.begin() ; it != links.end(); ++it)
        delete (*it);
    links.clear();
}

//***************************creating*****************************//
void Verlet::setPos(int index, const Vector3& pos){
    _pos[index]=pos;
}

int Verlet::createPoint(const Vector3& pos){
    _pos[numPoints] = pos;
    _prevPos[numPoints] = pos;
    numPoints++;
    return numPoints-1;
}

void Verlet::createPin(int index){
    Pin p = {index, _pos[index]};
    pins.push_back(p);
}
//***************************editing links*****************************//
Link* Verlet::createLink(int a, int b){
    float length = (_pos[b]-_pos[a]).length();
    Link* l = new Link(a, b, length);
    links.push_back(l);
    return l;
}
//***************************for update*****************************//
//Applies gravity
//Updates positions of all particles w/ velocity + acc
//Resets normals + acceleration
void Verlet::verlet(float seconds){
    for(int i=0; i<numPoints; i++) {
        Vector3& pos = _pos[i];
        Vector3 temp = pos;
        Vector3& prevPos = _prevPos[i];
        //apply gravity
        Vector3& acc = _acc[i]+=_manager->gravity;
        //update positions
        pos += .99*(pos-prevPos)+acc*seconds*seconds;
        prevPos = temp;
        //reset
        _acc[i]=Vector3(0,0,0);
    }
}
void Verlet::onTick(float ){}

void Verlet::onDraw(Graphic *g){
    g->setLineWidth(1);
    foreach(Link* l, links)
        drawLink(l,g);
}

void Verlet::drawLink(Link *l, Graphic *g){
     if(l!=NULL)
         g->drawLine(_pos[l->pointA],_pos[l->pointB]);
}

void Verlet::drawGradientLink(Link* l, Graphic* g){
    if(l!=NULL){
        Vector3 color1 = Vector3(1,1,0);
        Vector3 color2 = Vector3(0,1,1);
        g->drawGradientLine(color1,color2,_pos[l->pointA],_pos[l->pointB]);
    }
}
//***************************updating triangles in mesh*****************************//
void Verlet::calculate(Tri* t){
    t->vertices[0]=_pos[t->a];
    t->vertices[1]=_pos[t->b];
    t->vertices[2]=_pos[t->c];

    t->normal =  (t->vertices[1] - t->vertices[0]).cross(t->vertices[2] - t->vertices[0]);
    t->normal.normalize();
}

//***************************collisions*****************************//

Vector3 Verlet::collide(Entity *e){
    Ellipsoid* el = e->getEllipsoid();
    Vector3 toMove = e->getMove();
    Vector3 center = el->getPos()+toMove;
    float radius = e->getDim().x;
    //determines whether to move points themselves
    bool solve = _manager->solve;
    //if player is on ground (geometric collision), verlet won't influence it
    bool hitGeo = e->getMtv()!=Vector3(0,0,0);

    float count = 0;  //how many points hit
    Vector3 translation = Vector3(0,0,0); //accumulative mtv

    for(int i=0; i<numPoints; i++) {
        Vector3 dist =_pos[i]-center; //distance between entity + point
        float radiusSquared = radius * radius;
        if(radiusSquared>dist.lengthSquared()){  //colliding
            count++;

            //mtv
            Vector3 unit = dist;
            unit.normalize();
            float factor = dist.length()-radius;
            Vector3 extra = unit*factor;

            if(solve&&!hitGeo)
                _pos[i]=_pos[i]-(extra*sphereInfluence);
            else if(solve)  //give extra allowance if sphere won't move
                _pos[i]=_pos[i]-(extra*sphereInfluence*2);

            translation+=extra;
        }
     }
    if(count>0){
        //lower = jittery, higher = doesn't compensate for collisions
        count *= .5;
        translation/=count; //divide accumulative mtv by points hit
    }
    if(!hitGeo||!solve){  //sphere can be moved by points
        toMove+=translation;
        e->setMove(toMove);
        e->setMtv(translation);
    }
    return Vector3(0,0,0);
}

//***************************constraints*****************************//
void Verlet::pinConstraint(){
    for(Pin p: pins)
        _pos[p.index] = p.pinPos;
}

//Uses squareroot approximation
void Verlet::linkConstraint(){
    for(Link* l: links) {
        Vector3& posA = _pos[l->pointA];
        Vector3& posB = _pos[l->pointB];

        Vector3 delta = posB-posA;
        //the closer delta.dot(delta) is to restLengthSq, the smaller the displacement
        delta*=l->restLengthSq / (delta.dot(delta)+l->restLengthSq) - .5;
        posA -= delta;
        posB += delta;
    }
}
