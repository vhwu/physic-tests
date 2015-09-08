#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include <QWheelEvent>
#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

//Held within world. Pointed to by player.
//Include in tick/draw (transform) and wheelEvent (wheelEvent)
class Camera
{
public:
    Camera(const Vector2& size, bool person=true); //ratio, the width/height of screen
    virtual ~Camera();

    void resize(const Vector2& s);
    //Repositions camera: call in onDraw of world
    void transform();

    //Camera movements
    //change position of camera by vec
    void translate(const Vector3& vec);
    //change position of camera to vec
    void moveTo(const Vector3& vec);
    //change direction camera is looking: call in world's rotateCamera
    void rotate(float _yaw, float _pitch);

    //Call by player to get movement vector
    Vector3 getPos() {return _eye;}
    Vector3 getLook();
    Vector3 getForward();
    Vector3 getBackward();
    Vector3 getRight();
    Vector3 getLeft();

    //Toggles between first + third person: call in world's xxxEvent
    void changePerson();
    //Zooms: call in world's wheelEvent, pass in event->delta
    void zoom(int scroll);

    //**************************************for camera dragging
    double modelview[16];
private:
    //true for first person, false for third person
    bool _person;
    //in third person, how far you are from eye
    float _zoom;
    //controls how fast you zoom
    float _zoomFactor;
    float _zoomMin;
    float _zoomMax;

    //position of camera
    Vector3 _eye;
    //rotation around y (vertical) axis: horizontal direction
    float _yaw;
    //angle with horizontal plane: up and down. Keep between PI/2 and PI/-2
    float _pitch;
    float _maxPitch;
    float _minPitch;
    //field of view: width of lens. Default of 60, keep between 0 and 180
    float _fov;
    //aspect ratio of screen
    Vector2 _ratio;
    float _near;
    float _far;
};

#endif // CAMERA_H
