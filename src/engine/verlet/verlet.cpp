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
    Vector3 force = e->getAcc();
        if(force.y<0)
            force.y = force.y*=3;

    Vector3 center = el->getPos()+toMove;
    float radius = e->getDim().x;
    //determines whether to move points themselves
    bool solve = _manager->solve;

    float count = 0;  //how many points hit
    Vector3 translation = Vector3(0,0,0); //accumulative mtv
    Vector3 velocity = Vector3(0,0,0); //accumulative velocity

    //TESTING
    Vector3 vel = Vector3(0,0,0);

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

            if(solve)
                _pos[i]=_pos[i]-(extra*sphereInfluence);
            translation+=extra;

            //TESTING
//            _acc[i]+=force;

            vel = _pos[i]-_prevPos[i];
            velocity += vel;

//            if(vel.y>0&&factor<-.14){
//                factor*=-1;
//                std::cout<<"test:"<<factor<<std::endl;
//                float text = factor-.14;
//                text*=8;
//                translation.y+=text;
//                if(factor>.17){
//                    Vector3 v = e->getVel();
//                    v.y+=((factor-.17)*300);
//                    e->setVel(v);
//                std::cout<<"jump"<<std::endl;
//                }
//            }

        }
     }

    if(count>0){
        //lower = jittery, higher = doesn't compensate for collisions
        count *= .5;
        translation/=count; //divide accumulative mtv by points hit
    }
    if(_manager->solve&&count>0){
//        std::cout<<velocity<<std::endl;
//        std::cout<<"v:"<<e->getVel()<<std::endl;
        Vector3 horizontal = Vector3(velocity.x,0,velocity.z);
        horizontal = horizontal/(count*2);

        e->verletAcc=horizontal;//*60;

//        Vector3 actualVel = e->getVel();
//        actualVel.y = 0;
//        float align = 0;

//        Vector3 currAcc = e->verletAcc;
//        currAcc.y = 0;

//        if(actualVel.lengthSquared()>0 && horizontal.lengthSquared()>0){
//            Vector3 h = horizontal;
//            Vector3 a = actualVel;
//            h.normalize();
//            a.normalize();
////            horizontal.normalize();
////            actualVel.normalize();

////            align = a.dot(h);
//            align = actualVel.dot(horizontal);

////            if(currAcc.lengthSquared()>0)
////                currAcc.normalize();
//////            horizontal.normalize();
////            std::cout<<"curr acc:"<<currAcc<<std::endl;
////            std::cout<<"ver acc:"<<horizontal<<std::endl;
////            if(currAcc.lengthSquared()>0)
////            std::cout<<currAcc.dot(horizontal)<<std::endl;
//            std::cout<<"align:"<<align<<std::endl;
//                 std::cout<<"v length:"<<horizontal.length()<<std::endl;
//           std::cout<<"p length:"<<actualVel.length()<<std::endl;


//           float scalar = (align+1)*.5;

//           float l = horizontal.length();
//           float p = actualVel.length();
//           if(p<=0.0001)
//               p=2.38;
//   //        float lengthScalar = 290;
//   //        if(actualVel.length()>0)
//   //            lengthScalar = horizontal.length()/actualVel.length();
//   //        std::cout<<"length scale:"<<lengthScalar<<std::endl;
//           std::cout<<"verlet mag:"<<horizontal.length()<<std::endl;
////           if(horizontal.lengthSquared()!=0){

////               //verlet mag:.03 - .04
////               //framerate: .015
////               horizontal.normalize();
////               //figure out magnitude ratio, and alos remember about + versus - dots
////           }
//   //        e->verletAcc=horizontal*(1-scalar); //*lengthScalar;
////           std::cout<<12/l<<std::endl;
//           e->verletAcc=horizontal*100;//*(6.5/l);//*(3.6/l);//*(1-scalar); //*lengthScalar;


//   //        std::cout<<"scalar:"<<1-scalar<<std::endl;
//           std::cout<<e->verletAcc<<std::endl;
//           //you need to isolate out another horizontal acceleration: 'verlet'
//           //actually, maybe you need a variable for horizontal vel (both control and verlet)... so you add them, and get dot
//           //though, what do you do once you leave the verlet? how to you make verletAcc degrade?
        }
//        else{
//            e->verletAcc=Vector3(0,0,0);
//        }

//    }

    toMove+=translation;
    e->setMove(toMove);
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
