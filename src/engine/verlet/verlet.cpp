#include "verlet.h"
#include "verletmanager.h"
#include "link.h"
#include "engine/common/graphic.h"
#include "engine/common/raytracer.h"
#include "engine/geometric/geometricmanager.h"
#include "engine/common/entity.h"
#include "engine/common/ellipsoid.h"

#include <iostream>

Verlet::Verlet(VerletManager* m):
    playerInfluence(.005)
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
        Vector3 vel = pos-prevPos;
        pos += .99*vel+acc*seconds*seconds;
        prevPos = temp;
        //reset
        _acc[i]=Vector3(0,0,0);
    }
}
void Verlet::onTick(float ){}

void Verlet::onDraw(Graphic *g){
    g->setLineWidth(1);
    g->setColor(Vector3(1,1,1));
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
    Vector3 playerVel = e->getVel();
    Vector3 center = el->getPos()+toMove;
    float radius = e->getDim().x;
    float radiusSquared = radius * radius;

    bool solve = _manager->solve;                   //determines whether points will move
    float count = 0;                                //how many points hit
    Vector3 translation = Vector3(0,0,0);           //accumulative mtv
    Vector3 velocity = Vector3(0,0,0);              //accumulative velocity

    for(int i=0; i<numPoints; i++) {
        Vector3 dist =_pos[i]-center;
        if(radiusSquared>dist.lengthSquared()){     //colliding
            count++;

            //Find overlap of point w/ sphere (mtv) + apply this difference to acc. mtv
            Vector3 unit = dist; unit.normalize();

            float offset = radius-dist.length();    //% of distance point is to center of sphere
            Vector3 overlap = unit*offset;
            translation-=overlap;

            //Move point by (scaled) difference and a percentage of player's velocity
            if(solve){
                Vector3 displacement = overlap*sphereInfluence;
                Vector3 impartVel = playerVel*playerInfluence;
                _pos[i]+=displacement+impartVel;
            }

            Vector3 vel = _pos[i]-_prevPos[i];
            velocity+= vel;

            //WIP
            //To provide positional offset for player, should cloth be going too fast to be fixed w/ applying force
            if(vel.y>0&&offset>.14){
                velocity+=overlap*-5;
                translation+=vel;
            }
        }
    }

    if(count>0){
        translation/=count*.5;                  //lower = jittery, higher = doesn't compensate for collisions
        velocity/=count;
        float n = playerVel.dot(translation);
         if(n<0){                                //If player is moving into the surface of the verlet,
             Vector3 nForce = translation*(-n);  //Apply normal force as scaled by offset
             velocity+=nForce;
         }
         e->_verletAcc=velocity;                 //Only write over upon contact- value still used during deceleration
    }
    e->setMove(toMove+translation);
    return translation;
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
