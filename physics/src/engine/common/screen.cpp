#include "screen.h"
#include "view.h"

Screen::Screen(View* v, Application *a)
{

}

Screen::~Screen()
{

}

Vector2 Screen::centerMouse(QMouseEvent *event){
    int deltaX = event->x() - _view->width() / 2;
    int deltaY = event->y() - _view->height() / 2;
    if (!deltaX && !deltaY)
        return Vector2(0,0);
    QCursor::setPos(_view->mapToGlobal(QPoint(_view->width() / 2, _view->height() / 2)));
    return Vector2(deltaX, deltaY);
}
