#ifndef WORLD_H
#define WORLD_H

#include <QKeyEvent>
class Graphic;
class Entity;
class Camera;
class Manager;
class Screen;

#include "vector.h"
#include "engine/common/raytracer.h"

//Held within screen, holds entities and camera
class World
{
public:
    World(Screen* s);
    virtual ~World();

    //Loops through entities, passes calls to them
    virtual void onTick(float seconds);
    virtual void onDraw(Graphic *g)=0;
    virtual void onUI(Graphic*g);

    //Pass to specific entities, if necessary
    virtual void wheelEvent(QWheelEvent *event)=0;
    virtual void keyPressEvent(QKeyEvent *event)=0;
    //Call these from game-side world for centered-camera + moving-camera functionality
    virtual void mousePressEvent(QMouseEvent *event)=0;
    virtual void mouseMoveEvent(QMouseEvent *event)=0;
    virtual void mouseReleaseEvent(QMouseEvent *event)=0;
    virtual void keyReleaseEvent(QKeyEvent *event)=0;
    virtual void onResize(const Vector2& size);

    bool endGame;

    //For selection
    HitTest hit;
    RayTracer* _ray;

    Vector3 getGravity(){return _gravity;}
protected:
    Camera* _camera;
    Screen* _screen;

    //For camera controls
    //Toggle on/off with m: controls if mouse is centered on screen
    bool mouseMove = true;
    //If !mouseMove, dragging middle button moves camera
    bool draggingCamera = false;
    int mouseX, mouseY;

    QList<Entity *> _entities;
    QList<Manager *> _managers;

    //Helper in mouseMoveEvent to turn camera based on mouse movements
    void rotateCamera(const Vector2& delta);

    void addEntity(Entity* const &e);
    void removeEntity(Entity* const &e);
    void addManager(Manager* const &m);

    Vector3 _gravity;
};

#endif // WORLD_H
