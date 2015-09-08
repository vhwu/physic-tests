#include "verletworld.h"
#include "engine/common/camera.h"
#include "player.h"
#include "engine/common/graphic.h"
#include "engine/common/obj.h"
#include "engine/geometric/geometricmanager.h"
#include "engine/verlet/verletmanager.h"
#include "engine/verlet/trianglemesh.h"
#include "engine/common/ellipsoid.h"
#include <queue>

VerletWorld::VerletWorld(Screen* s): World(s)
{
    _height = 3;
    _camera = new Camera(Vector2(800, 600), false);
    _camera->translate(Vector3(0,_height,0));

    _player = new Player(_camera,_height);
    _player->setPos(Vector3(0,3,-1));
    this->addEntity(_player);

    _manager = new VerletManager();
    this->addManager(_manager);

    //_gManager = new GeometricManager("island", true);
    //_gManager = new GeometricManager("one", true);

    _gManager = new GeometricManager("lowpoly", true);
    //_gManager = new GeometricManager(":/levels/mountain_maya.obj", false);
    //_gManager = new GeometricManager(":/levels/island01.obj", false);
    this->addManager(_gManager);

    //VERLETS

    //TriangleMesh* tear1 = new TriangleMesh(Vector2(15,40), .15, Vector3(-1,5,0), _manager, true, true);
    TriangleMesh* tear1 = new TriangleMesh(Vector2(20,20), .3, Vector3(-1,6,0), _manager, true, true);
    tear1->createPin(0);
    tear1->createPin(4);
    tear1->createPin(9);
    tear1->createPin(14);
    tear1->createPin(19);

    _manager->addVerlet(tear1);

    windDirection = Vector3(0,0,0);

    if(mouseMove)
        _ray = new RayTracer(_camera->modelview, mouseX, mouseY);
    else
        _ray = new RayTracer(_camera->getPos(), _camera->getLook());

}

VerletWorld::~VerletWorld()
{
    delete _camera;
    delete _player;
    delete _manager;
    delete _gManager;

}

void VerletWorld::onTick(float seconds){
    //Wind direction
     if(windStart){
         Vector3 source = _camera->getPos()+_camera->getLook();
         Vector3 direction = -1*_camera->getLook();
         windStartPos = _ray->getPointonPlane(source,direction);
         windStart = false;
     }
     if(windConst){
         Vector3 source = _camera->getPos()+_camera->getLook();
         Vector3 direction = -1*_camera->getLook();
         windEndPos = _ray->getPointonPlane(source,direction);
         if(windStartPos!=windEndPos){
             Vector3 wind = windEndPos-windStartPos;
             float factor = wind.lengthSquared()*_manager->windScalar;
             _manager->windPow = (factor>_manager->windPowMax) ? _manager->windPowMax : factor;
             wind.normalize();
             wind = Vector3(wind.x,0,-wind.y); //ability to direct wind towards/ away from you
             windDirection = wind;
         }
     }

    //geometric collisions
    Vector3 mtv = _gManager->collideTerrain(_player);
    _player->setMtv(mtv);

    //verlet collisions: offset player if not colliding with ground
    _player->setMove(Vector3(0,0,0));
    _manager->collideTerrain(_player);

    _player->move(_player->getMove());

    //raytrace to find hovered point
    if(mouseMove){
        _ray->update(_camera->modelview, mouseX, mouseY);
    }
    else{
        _ray->source = _camera->getPos();
        _ray->direction = _camera->getLook();
    }

    HitTest trace;
    _manager->rayTrace(_ray, trace);
    hit = trace;

    //dragging
    if(dragMode){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(0.01, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }

    //tearing
    if(tearMode){
        if(hit.hit&&tear_ptA<0){
            tear_ptA = hit.id;
            tearVerlet = hit.v;
        }
        else if(tear_ptA>0&&tear_ptB<0){
            QList<Link*> proximity = tearVerlet->_linkMap[tear_ptA].links;
            QList<int> p;
            QHash<int,Link*> pairs;
            HitTest h;
            foreach(Link* l, proximity){
                if(l->pointA==tear_ptA){
                    p.push_back(l->pointB);
                    pairs[l->pointB]=l;
                }
                else if(l->pointB==tear_ptA){
                    p.push_back(l->pointA);
                    pairs[l->pointA]=l;
                }
            }

            _ray->hitVerlet(tearVerlet,h);
            if(h.hit&&p.contains(hit.id)){
                tear_ptB= hit.id;
                tearLink = pairs[tear_ptB];
            }

            Vector3 tearMouse = _ray->getPointonPlane(tearVerlet->getPoint(tear_ptA),-1*_camera->getLook());
            float d = (tearMouse-tearVerlet->getPoint(tear_ptA)).lengthSquared();
            if(tear_ptB<0&&d>.5){
                int near = -1;
                float closest = 100;
                foreach(int i,p){
                    float dist = (tearMouse-tearVerlet->getPoint(i)).lengthSquared();
                    if(dist<closest){
                        closest = dist;
                        near = i;
                    }
                }
                if(near>0){
                    tear_ptB=near;
                    tearLink = pairs[tear_ptB];
                }
            }

        }

        else if(tear_ptA>0&&tear_ptB>0&&tearLink!=NULL&&tearTimer==0){//&&tear_ptB!=tear_prevA){
            tearVerlet->tearLink(tearLink);
            tearLink = NULL;
            tear_prevA = tear_ptA;
            tear_ptA = tear_ptB;
            tear_ptB = -1;
            tearTimer=tearDelay;
        }
        else if(tearTimer!=0)
            tearTimer--;
    }

    //update verlet
    _manager->setWind(windDirection);
    _player->applyAcc(Vector3(0,6,0));
    World::onTick(seconds);
}

void VerletWorld::onDraw(Graphic *g){
    World::onDraw(g);


    g->setColor(Vector3(1,1,1));
    g->opaque(true);

    if(dragMode){
        //draw selected point in red
        g->transform(&Graphic::drawUnitSphere,draggedVerlet->getPoint(draggedPoint),0,
            Vector3(.1,.1,.1));
        //draw mouse pos in cyan
        g->setColor(Vector3(0,1,1));
        g->transform(&Graphic::drawUnitSphere,draggedMouse,0,
            Vector3(.1,.1,.1));
        //draw interpolated position in green
        g->setColor(Vector3(1,0,0));
        g->transform(&Graphic::drawUnitSphere,interpolate,0,
                     Vector3(.1,.1,.1));
    }

    else if(hit.hit){
        //draw point that's being hovered in yellow
        g->setColor(Vector3(1,1,0));
        g->transform(&Graphic::drawUnitSphere,hit.v->getPoint(hit.id),0,
            Vector3(.1,.1,.1));
    }

    //Visualize wind
    //if(windComplete){
    if(windConst){
        Vector3 colorStart = Vector3(0,1,0);
        Vector3 colorEnd = Vector3(1,0,0);
        g->drawGradientLine(colorStart,colorEnd,windStartPos,windEndPos);
    }
}

void VerletWorld::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    if(event->button() == Qt::LeftButton&& hit.hit){
        dragMode = true;
        draggedPoint = hit.id;
        draggedVerlet = hit.v;
        interpolate = hit.v->getPoint(hit.id);
    }
    if(event->button() == Qt::RightButton)
        tearMode=true;
    if(event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
        _manager->enableSolve();
}

void VerletWorld::mouseMoveEvent(QMouseEvent *event){
    World::mouseMoveEvent(event);

}

void VerletWorld::mouseReleaseEvent(QMouseEvent *event){
    World::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton)
        dragMode = false;
    if(event->button() == Qt::RightButton){
        tearMode = false;
        tear_ptA = -1;
        tear_ptB = -1;
    }
}

void VerletWorld::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}
void VerletWorld::keyPressEvent(QKeyEvent *event){
    World::keyPressEvent(event);
    _player->keyPressEvent(event);
    if(event->key() == Qt::Key_Shift){
        windStart = true;
        windComplete = false;
        windConst = true;
    }
}
void VerletWorld::keyReleaseEvent(QKeyEvent *event){
    _player->keyReleaseEvent(event);
    if(event->key() == Qt::Key_J) _manager->enableSolve();
    if(event->key() == Qt::Key_Down) windDirection = Vector3(0,0,1);
    if(event->key() == Qt::Key_Up) windDirection = Vector3(0,0,-1);
    if(event->key() == Qt::Key_Left) windDirection = Vector3(-1,0,0);
    if(event->key() == Qt::Key_Right) windDirection = Vector3(1,0,0);
    if(event->key() == Qt::Key_O) windDirection = Vector3(0,0,0);
    if(event->key() == Qt::Key_Shift){
        windEnd = true;
        windComplete = true;
        windConst = false;
    }
}
