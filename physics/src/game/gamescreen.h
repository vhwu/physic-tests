#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include "engine/common/screen.h"

class Graphic;
class PlatformWorld;
class VerletWorld;
class World;

class GameScreen: public Screen
{
public:
    GameScreen(View* v, Application* _a);
    virtual ~GameScreen();

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
private:
    World* _world;
    int mode;
};

#endif // GAME_SCREEN_H
