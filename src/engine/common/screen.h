#ifndef SCREEN_H
#define SCREEN_H

#include <QKeyEvent>
#include "vector.h"

class Graphic;
class View;
class Application;

//Contained within application, passes calls to world
class Screen
{
public:
    Screen(View*, Application*);
    virtual ~Screen();

    //Pass to world
    virtual void create(int mode)=0;
    virtual void tick(float seconds)=0;
    virtual void onDraw(Graphic *g)=0;
    virtual void onUI(Graphic* g)=0;
    virtual void mousePressEvent(QMouseEvent *event)=0;
    virtual void mouseMoveEvent(QMouseEvent *event)=0;
    virtual void mouseReleaseEvent(QMouseEvent *event)=0;
    virtual void wheelEvent(QWheelEvent *event)=0;
    virtual void keyPressEvent(QKeyEvent *event)=0;
    virtual void keyReleaseEvent(QKeyEvent *event)=0;

    virtual void onResize(const Vector2& size)=0;
    Vector2 centerMouse(QMouseEvent *event);
protected:
    View* _view;
    Application* _app;
};

#endif // SCREEN_H
