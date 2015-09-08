#include "world.h"
#include "graphic.h"
#include "entity.h"
#include "manager.h"
#include "camera.h"
#include "engine/common/collision.h"
#include "engine/common/screen.h"

World::World(Screen* s)
{
    _screen = s;
    endGame = false;
}

World::~World()
{

}

void World::onTick(float seconds)
{
    foreach (Entity* e, _entities){
        e->applyAcc(_gravity);
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
}
void World::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::MidButton)
        draggingCamera=false;
}
void World::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_M)
        mouseMove=!mouseMove;
}


