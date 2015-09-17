#include "gamescreen.h"
#include "engine/common/screen.h"
#include "view.h"
#include "verlettest1.h"
#include "verlettest2.h"
#include "verlettest3.h"
#include "verlettest4.h"
#include "verlettest5.h"
#include "engine/common/application.h"

#include <iostream>

GameScreen::GameScreen(View* v, Application* a): Screen(v, a)
{
    _view = v;
    _app = a;
}

void GameScreen::create(int m){
    mode = m;
    if(mode==1)
        _world = new VerletTest1(this);
    else if(mode==2)
        _world = new VerletTest2(this);
    else if(mode==3)
        _world = new VerletTest3(this);
    else if(mode==4)
        _world = new VerletTest4(this);
    else if(mode==5)
        _world = new VerletTest5(this);
}

GameScreen::~GameScreen()
{
    delete _world;
}

void GameScreen::tick(float seconds)
{
    _world->onTick(seconds);
    if(_world->endGame)
        _app->changeScreen(0);
}

void GameScreen::onDraw(Graphic *g)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1,1,1);
    _view->renderText(0,10,
                       "Camera: move (MMB drag) and zoom (MMB scroll)", _view->font());
    _view->renderText(0,25,
                       "Avatar controls: WASD, space, R=up, F=down, U=reset position", _view->font());
    _view->renderText(0,40,
                       "Environment interaction: LMB = selectable points in teal, RMB = freeze/unfreeze", _view->font());
    _world->onDraw(g);
}

void GameScreen::onUI(Graphic* g){
    _world->onUI(g);
}


void GameScreen::onResize(const Vector2& size)
{
    _world->onResize(size);
}

void GameScreen::mousePressEvent(QMouseEvent *event)
{
    _world->mousePressEvent(event);
}

void GameScreen::mouseReleaseEvent(QMouseEvent *event)
{
    _world->mouseReleaseEvent(event);
}

//Keeps mouse at center of screen and sets camera orientation based on movement
void GameScreen::mouseMoveEvent(QMouseEvent *event)
{
    _world->mouseMoveEvent(event);
}

void GameScreen::wheelEvent(QWheelEvent *event)
{
    _world->wheelEvent(event);
}

void GameScreen::keyPressEvent(QKeyEvent *event)
{ 
    if(event->key() == Qt::Key_Backspace)
        _app->changeScreen(0);
    _world->keyPressEvent(event);
}

void GameScreen::keyReleaseEvent(QKeyEvent *event)
{
    _world->keyReleaseEvent(event);
}



