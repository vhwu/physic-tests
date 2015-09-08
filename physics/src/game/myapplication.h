#ifndef MY_APPLICATION_H
#define MY_APPLICATION_H

#include "engine/common/application.h"

class GameScreen;
class MenuScreen;
class View;

class MyApplication: public Application
{
public:
    MyApplication(View* v);
    ~MyApplication();

    void changeScreen(int mode);
    void initializeGL();

    void tick(float seconds);
    void onDraw();
    void storeMatrix();
    void onUI();

    void onResize(const Vector2& size);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
};

#endif // MY_APPLICATION_H
