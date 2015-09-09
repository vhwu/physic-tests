#include "gamescreen.h"
#include "engine/common/screen.h"
#include "view.h"
#include "verlettest1.h"
#include "engine/common/application.h"

#include <iostream>

GameScreen::GameScreen(View* v, Application* a): Screen(v, a)
{
    _view = v;
    _app = a;
}

void GameScreen::create(int m){
    mode = m;
    //_world = new VerletWorld(this);
    _world = new VerletTest1(this);
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



