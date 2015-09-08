#include "myapplication.h"
#include "engine/common/application.h"

#include "view.h"
#include "gamescreen.h"
#include "menuscreen.h"
#include "engine/common/graphic.h"

MyApplication::MyApplication(View* v): Application(v)
{
    _view = v;

    _screens.push(new MenuScreen(v, this));
    _active_screen = _screens.top();

    _graphic = new Graphic(v);
}

MyApplication::~MyApplication()
{
    delete _graphic;
}

void MyApplication::changeScreen(int mode){
    _screens.pop();
    if(mode == 0)
        _screens.push(new MenuScreen(_view, this));
    else
        _screens.push(new GameScreen(_view, this));
    _active_screen = _screens.top();
    _active_screen->create(mode);
}

void MyApplication::initializeGL(){
    _graphic->loadResource();
}

void MyApplication::tick(float seconds)
{
    _active_screen->tick(seconds);
}

void MyApplication::onDraw()
{
    _active_screen->onDraw(_graphic);
}

void MyApplication::storeMatrix(){
    this->_graphic->storeMatrix();
}

void MyApplication::onUI()
{
    _graphic->depth(false);
    _active_screen->onUI(_graphic);
    _graphic->depth(true);
}

void MyApplication::onResize(const Vector2& size)
{
    _active_screen->onResize(size);
}

void MyApplication::mousePressEvent(QMouseEvent *event)
{
    _active_screen->mousePressEvent(event);
}

void MyApplication::mouseMoveEvent(QMouseEvent *event)
{
    _active_screen->mouseMoveEvent(event);
}

void MyApplication::mouseReleaseEvent(QMouseEvent *event)
{
    _active_screen->mouseReleaseEvent(event);
}

void MyApplication::wheelEvent(QWheelEvent *event)
{
    _active_screen->wheelEvent(event);
}

void MyApplication::keyPressEvent(QKeyEvent *event)
{
    _active_screen->keyPressEvent(event);
}

void MyApplication::keyReleaseEvent(QKeyEvent *event)
{
    _active_screen->keyReleaseEvent(event);
}

