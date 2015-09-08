#include "camera.h"
#include "math.h"
#include <iostream>
#include <limits>

Camera::Camera(const Vector2& size, bool person)
{
    _eye = Vector3(0,0,0);
    _yaw = 0;
    _pitch = 0;
    _fov = 60;
    _ratio = size;
    _near = .1f;
    _far = 200;

    //Epsilon prevents camera from flipping
    _maxPitch=M_PI/2.0-std::numeric_limits<float>::epsilon();
    _minPitch=-M_PI/2.0+std::numeric_limits<float>::epsilon();

    _person = person;
    _zoom = 4;
    _zoomMin = 2;
    _zoomMax = 100; //6
    _zoomFactor = .1f;  //.05f
}

Camera::~Camera()
{
}

void Camera::resize(const Vector2& s){
    _ratio = s;
}


void Camera::transform(){
    //set up projection (fov, aspect ratio, etc)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); //reset the matrix, since gluPerspective multiplies it
    gluPerspective(_fov, (float)_ratio.x/(float)_ratio.y, _near, _far);

    //set directin of camera (pos, look, and up vectors)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //reset the matrix, since gluPerspective multiplies it
    //direction you're looking in
    Vector3 look = Vector3(cos(_yaw) * cos(_pitch), sin(_pitch), sin(_yaw) * cos(_pitch));
    Vector3 center = _eye+look;

    Vector3 eye = _eye;
    //third person: translate eye back along look vector
    if(!_person)
        eye-=look*_zoom;

    Vector3 up = Vector3(0,1,0);
    if(_pitch<0)  //looking down
        up = Vector3(look.x, 1, look.z);
    up.normalize();
    //camera is at m_eye, looking at enter, with up direction up
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
}

void Camera::translate(const Vector3& vec){
    _eye+=vec;
}

void Camera::moveTo(const Vector3& vec){
    _eye=vec;
}

void Camera::rotate(float yaw, float pitch){
    _yaw+=yaw;
    _pitch+=pitch;

    if(_pitch>_maxPitch)
        _pitch = _maxPitch;
    else if(_pitch<_minPitch)
        _pitch = _minPitch;

    if(_yaw>=(2*M_PI)||yaw <=(-2*M_PI))
        yaw=0;
}

Vector3 Camera::getLook(){
    return Vector3(cos(_yaw) * cos(_pitch),
                   sin(_pitch), sin(_yaw) * cos(_pitch));
}

Vector3 Camera::getForward(){
    return Vector3(cos(_yaw), 0, sin(_yaw));
}

Vector3 Camera::getBackward(){
    return Vector3(-cos(_yaw), 0, -sin(_yaw));
}

//currently reversed with getLeft in terms of calculations?
Vector3 Camera::getRight(){
    return Vector3(-sin(_yaw), 0, cos(_yaw));
}

Vector3 Camera::getLeft(){
    return Vector3(sin(_yaw), 0, -cos(_yaw));
}

void Camera::changePerson(){
    _person=!_person;
}

void Camera::zoom(int scroll){
    if((scroll > 0) && (_zoom>_zoomMin))
        _zoom*=(1-_zoomFactor);
    else if((scroll < 0) && (_zoom<_zoomMax))
       _zoom*=(1+_zoomFactor);
}


