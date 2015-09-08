#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include "engine/common/screen.h"

class MenuScreen: public Screen
{
public:
    MenuScreen(View* v, Application* a);
    virtual ~MenuScreen();

    void create(int mode);
    void tick(float seconds);
    void onDraw(Graphic *g);
    void onUI(Graphic* g);

    void onResize(const Vector2& size);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // MENU_SCREEN_H
