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

void MenuScreen::create(int )
{}

void MenuScreen::tick(float )
{}

void MenuScreen::onDraw(Graphic* )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1,1,1);
//    _view->renderText(_view->width()/2-120, _view->height()/2,
    _view->renderText(10,20,
                      "1.Translation -> wave propagation (VerletTest1)", _view->font());
    _view->renderText(10,35,
                      "2.Rotation + angle -> kaleidoscope (VerletTest4)", _view->font());
    _view->renderText(10,50,
                      "3.Rotation + angle + translation + align -> staircase (VerletTest5)", _view->font());
    _view->renderText(10,65,
                      "4.AutoRotate + AutoTranslate (VerletTest6)", _view->font());
}

void MenuScreen::onUI(Graphic* ){}

void MenuScreen::onResize(const Vector2& )
{}

void MenuScreen::mousePressEvent(QMouseEvent*)
{}

void MenuScreen::mouseMoveEvent(QMouseEvent*)
{}

void MenuScreen::mouseReleaseEvent(QMouseEvent*)
{}

void MenuScreen::wheelEvent(QWheelEvent*)
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
    if (event->key() == Qt::Key_4){
        _app->changeScreen(4);
    }
}

void MenuScreen::keyReleaseEvent(QKeyEvent*)
{}
