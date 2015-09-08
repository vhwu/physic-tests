#ifndef APPLICATION_H
#define APPLICATION_H

#include <stack>
#include <QKeyEvent>
#include "vector.h"

class Screen;
class Graphic;
class View;

//Contained within view, passes calls to screens
class Application
{
public:
    Application(View* v);
    virtual ~Application();

    //Call in view's initilizeGL
    //Custom openGL initialization: load resources here
    virtual void initializeGL()=0;
    //Call within screen
    virtual void changeScreen(int mode)=0;

    //Pass to active screen
    virtual void tick(float seconds)=0;
    virtual void onDraw()=0;
    virtual void storeMatrix()=0;
    virtual void onUI()=0;
    virtual void onResize(const Vector2& size)=0;
    virtual void mousePressEvent(QMouseEvent *event)=0;
    virtual void mouseMoveEvent(QMouseEvent *event)=0;
    virtual void mouseReleaseEvent(QMouseEvent *event)=0;
    virtual void wheelEvent(QWheelEvent *event)=0;
    virtual void keyPressEvent(QKeyEvent *event)=0;
    virtual void keyReleaseEvent(QKeyEvent *event)=0;
protected:
    std::stack <Screen*> _screens;
    Screen* _active_screen;
    Graphic* _graphic;
    View* _view;
};

#endif // APPLICATION_H
