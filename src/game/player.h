#ifndef PLAYER_H
#define PLAYER_H

#include <QKeyEvent>
#include "vector.h"
#include "engine/common/entity.h"
#include "engine/common/raytracer.h"

class Camera;
class Graphic;
class Ellipsoid;

//Controls player movements with WASD
//Include in tick, keyPressEvent and keyReleaseEvent
class Player: public Entity
{
public:
    Player(Camera* c, float height);
    ~Player();

    Ellipsoid* getEllipsoid();

    void onTick(float seconds);
    void onDraw(Graphic* g);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void move(const Vector3& translate);
    void onCollide(Entity *e, const Vector3 &mtv);

private:
    Camera* _camera;
    float _playerHeight;

    float _goal, _dampen, _jumpVel;

    bool _w;
    bool _a;
    bool _s;
    bool _d;
    bool _r;
    bool _f;

    bool onGround;
};

#endif // PLAYER_H
