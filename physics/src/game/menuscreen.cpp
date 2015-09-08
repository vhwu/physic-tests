#include "menuscreen.h"
#include "engine/common/screen.h"
#include "view.h"
#include "engine/common/application.h"

MenuScreen::MenuScreen(View* v, Application* a): Screen(v, a)
{
    _view = v;
    _app = a;
}

MenuScreen::~MenuScreen()
{}

void MenuScreen::create(int mode)
{}

void MenuScreen::tick(float seconds)
{}

void MenuScreen::onDraw(Graphic *g)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1,1,1);
    _view->renderText(_view->width()/2-120, _view->height()/2,
                       "Press any key to start", _view->font());
}

void MenuScreen::onUI(Graphic* g){}

void MenuScreen::onResize(const Vector2& size)
{}

void MenuScreen::mousePressEvent(QMouseEvent *event)
{}

void MenuScreen::mouseMoveEvent(QMouseEvent *event)
{}

void MenuScreen::mouseReleaseEvent(QMouseEvent *event)
{}

void MenuScreen::wheelEvent(QWheelEvent *event)
{}

void MenuScreen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_1){
        _app->changeScreen(1);
    }
    if (event->key() == Qt::Key_2){
        _app->changeScreen(2);
    }
    if (event->key() == Qt::Key_3){
        _app->changeScreen(3);
    }
    if (event->key() == Qt::Key_5){
        _app->changeScreen(5);
    }
}

void MenuScreen::keyReleaseEvent(QKeyEvent *event)
{}
