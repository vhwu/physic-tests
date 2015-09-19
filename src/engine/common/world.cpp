#include "world.h"
#include "graphic.h"
#include "entity.h"
#include "manager.h"
#include "camera.h"
#include "engine/common/collision.h"
#include "engine/common/screen.h"
#include <iostream>

World::World(Screen* s):
    _gravity(Vector3(0,-9.8,0))
{
    _screen = s;
    endGame = false;
    _camera = new Camera(Vector2(800, 600), false);

    //Raytracing while toggling between stationary + moveable mouse
    if(mouseMove)
        _ray = new RayTracer(_camera->modelview, mouseX, mouseY);
    else
        _ray = new RayTracer(_camera->getPos(), _camera->getLook());
}

World::~World()
{
    qDeleteAll(_managers);
    _managers.clear();
    qDeleteAll(_entities);
    _entities.clear();
    delete _camera;
    delete _ray;
}

void World::onTick(float seconds)
{
    foreach (Entity* e, _entities){
//        e->applyAcc(_gravity);
        e->onTick(seconds);
    }
    foreach (Entity* e, _entities){
         foreach (Entity* e2, _entities){
             if(e!=e2){
                Collision c = e->collide(e2);
                if(c.collide){
                     e->onCollide(e2, c.mtv/2.0);
                     e2->onCollide(e, c.mtv/-2.0);
                }
             }
         }
    }
    foreach (Manager* m, _managers)
        m->onTick(seconds);

    //raytrace to find hovered point
    if(mouseMove){
        _ray->update(_camera->modelview, mouseX, mouseY);
    }
    else{
        _ray->source = _camera->getPos();
        _ray->direction = _camera->getLook();
    }
}

void World::onDraw(Graphic *g)
{
    _camera->transform();
    foreach (Manager* m, _managers){
        m->onDraw(g);
    }
    foreach (Entity* e, _entities){
        e->onDraw(g);
    }
}

void World::onUI(Graphic *g)
{
    foreach (Manager* m, _managers){
        m->onUI(g);
    }
    foreach (Entity* e, _entities){
        e->onUI(g);
    }
}

void World::rotateCamera(const Vector2& delta){
    _camera->rotate(delta.x / 100.f, -delta.y / 100.f);
}

void World::onResize(const Vector2& size){
    _camera->resize(size);
}

void World::addEntity(Entity* const &e){
    _entities.append(e);
}
void World::removeEntity(Entity* const &e){
    _entities.removeOne(e);
}
void World::addManager(Manager* const &m){
    _managers.append(m);
}

void World::mousePressEvent(QMouseEvent *event){
    // Remember the mouse position so we can calculate deltas
    mouseX = event->x();
    mouseY = event->y();
    if(event->button() == Qt::MidButton)  //enable
        draggingCamera=true;
    foreach (Manager* m, _managers)
        m->mousePressEvent(event);
}

void World::mouseMoveEvent(QMouseEvent *event){
    if(!mouseMove){
        Vector2 delta = _screen->centerMouse(event);
        rotateCamera(delta);
    }
    else{
        // Get the change in mouse position from last time
        int deltaX = event->x() - mouseX;
        int deltaY = event->y() - mouseY;
        if(draggingCamera)
            rotateCamera(Vector2(deltaX,deltaY));
    }
    mouseX = event->x();
    mouseY = event->y();

    foreach (Manager* m, _managers)
        m->mouseMoveEvent(event);
}

void World::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::MidButton)
        draggingCamera=false;
    foreach (Manager* m, _managers)
        m->mouseReleaseEvent(event);
}

void World::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_M)
        mouseMove=!mouseMove;
    foreach (Manager* m, _managers)
        m->keyPressEvent(event);
}

void World::keyReleaseEvent(QKeyEvent *event){
    foreach (Manager* m, _managers)
        m->keyReleaseEvent(event);
}

void World::wheelEvent(QWheelEvent *event){
    foreach (Manager* m, _managers)
        m->wheelEvent(event);
}
