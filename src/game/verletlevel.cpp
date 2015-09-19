#include "verletlevel.h"
#include "engine/verlet/constraint.h"
#include "engine/geometric/triangle.h"
#include "engine/common/ellipsoid.h"

VerletLevel::VerletLevel(Screen *s): World(s),
    _height(3),
    _startPos(Vector3(0,0,0)),
    _mouseSpeed(0.12)
{
    _player = new Player(_camera,_height);
    _player->setPos(_startPos);
    this->addEntity(_player);

    //MANAGERS
    _cManager = new ConstraintManager(this);
    this->addManager(_cManager);
    _vManager = new VerletManager(this,_cManager);
    this->addManager(_vManager);
}

VerletLevel::~VerletLevel()
{}

void VerletLevel::onTick(float seconds){
    //Verlet collisions: offset player if not colliding with ground
    Vector3 source = _player->getPos();
    Vector3 direction = _player->getMove();
    RayTracer* r = new RayTracer(source,direction);

    //Raycast against verlets to find point
    HitTest test;
    bool hitVerlet = _vManager->rayTrace(r,test);

    //If point if hit, raycast against point's triangles
    HitTest test2;
    bool hitTriangle;
    if(hitVerlet){
        std::vector<Tri*> hitTriMesh = test.v->triPP[test.id];
        hitTriangle = r->hitMesh(hitTriMesh, test2);
    }

    //Find collision info from the triangle
    float percent;
    CollisionInfo c;
    if(hitTriangle){
        hitTri = test2.verletTri;
        Triangle geoT = Triangle(hitTri->vertices[0],hitTri->vertices[1],hitTri->vertices[2]);
        Ellipsoid* e = new Ellipsoid(source,Vector3(.5,.5,.5));
        c = geoT.collide(e,direction);
        percent = c.t;
//        std::cout<<percent<<std::endl;
        hitPoint = r->getPointonPlane(hitTri->vertices[0], hitTri->normal);
        delete e;
    }
    delete r;


    _player->setMtv(_vManager->collideTerrain(_player));
    if(hitTriangle&&percent>0&&percent<1){
//        collideTriangle(_player,c);
//        _player->move(_player->getMove());
        Vector3 normal = direction*-1;

//        Vector3 normal = c.ellipsoid_normal;
        normal.normalize();
//        std::cout<<normal*.5<<std::endl;
        if(normal.y<.99){
            float diff = 1.0-normal.y;
            normal+=Vector3(0,diff*.3,0);
//            float diffx = 1.0-normal.x;
//            normal+=Vector3(diffx*.1,0,0);
//            float diffz = 1.0-normal.z;
//            normal+=Vector3(0,0,diffz*.1);
        }
//        _player->setPos(hitPoint+normal*.5);

//                _player->setPos(hitPoint+Vector3(0,.5,0));  //smooth at beginning


//        _player->move(_player->getMove()*(1.0-percent));
//        _player->setPos(hitPoint+Vector3(0,.5,0));
//        _player->move(_player->getMove());
    }
//    else
//        _player->move(_player->getMove());
//    if((_player->getPos()-hitPoint).length()<.3)
//        _player->setVel(Vector3(0,0,0));
_player->move(_player->getMove());

    //Selection
    HitTest trace;
    _cManager->rayTrace(_ray,trace);
    hit = trace;

    if(dragMode){
        Vector3 normal = selectedC->getNormal(_camera);
        Verlet* draggedVerlet = selectedC->getVerlet();
        int draggedPoint = selectedC->getIndex();
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),normal);
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }
    World::onTick(seconds);


}

void VerletLevel::collideTriangle(Entity* e, CollisionInfo c){
    Vector3 move = e->getMove();
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

void VerletLevel::onDraw(Graphic *g){
    if(hitTri!=NULL){
        g->cull(false);
        g->setColor(Vector3(1,0,0));
        g->drawTriangle(hitTri->vertices,hitTri->normal);
        g->cull(true);

//        g->transform(&Graphic::drawUnitSphere,hitPoint,0,
//                     Vector3(.15,.15,.15));
    }

    World::onDraw(g);

    //Visualize mouse selection + interpolation
    if(dragMode){
        //draw mouse pos in cyan
        g->setColor(Vector3(0,1,1));
        g->transform(&Graphic::drawUnitSphere,draggedMouse,0,
                     Vector3(.1,.1,.1));
        //draw interpolated position in green
        g->setColor(Vector3(0,1,0));
        g->transform(&Graphic::drawUnitSphere,interpolate,0,
                     Vector3(.1,.1,.1));
    }
}

//Right click for freezing/ unfreezing
//Hover/ select
void VerletLevel::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    //if(event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
    if(event->button()==Qt::RightButton)
        _vManager->enableSolve();

    if(event->button() == Qt::LeftButton&& hit.hit){
        selectedC = hit.c;
        selectedC->setSelect(true);
        dragMode = true;
        interpolate = hit.v->getPoint(hit.id);
    }
}

//Set if constraint is hovered
void VerletLevel::mouseMoveEvent(QMouseEvent *event){
    World::mouseMoveEvent(event);
    if(hit.hit){
        if((hoveredC!=NULL)&&(hoveredC!=hit.c)){
            hoveredC->setHover(false);
        }
        hoveredC = hit.c;
        hoveredC->setHover(true);
    }
    else if(hoveredC!=NULL){
        hoveredC->setHover(false);
        hoveredC = NULL;
    }
}

//Reset constraint selection
void VerletLevel::mouseReleaseEvent(QMouseEvent *event){
    World::mouseReleaseEvent(event);

    if(event->button() == Qt::LeftButton){
        if(dragMode){
            selectedC->setSelect(false);
            selectedC = NULL;
        }
        dragMode = false;
    }
}

//Camera zoom
void VerletLevel::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}

//F:freeze
//U:reset player position
void VerletLevel::keyPressEvent(QKeyEvent *event){
    World::keyPressEvent(event);
    if(event->key() == Qt::Key_U)
        _player->resetPos(_startPos);
    _player->keyPressEvent(event);
}

void VerletLevel::keyReleaseEvent(QKeyEvent *event){
     _player->keyReleaseEvent(event);
}
