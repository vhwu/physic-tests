#include "ellipsoid.h"
#include "graphic.h"
#include "collision.h"

Ellipsoid::Ellipsoid(const Vector3& pos, const Vector3& dimension):
        Shape(pos, dimension)
{
    _pos = pos;
    _dim = dimension;
}

Ellipsoid::~Ellipsoid()
{}

void Ellipsoid::onDraw(Graphic* g){
    g->transform(&Graphic::drawUnitSphere, _pos,0,_dim*.95);
}


Collision Ellipsoid::collide(Shape* shape){
    return shape->collideEllipsoid(this);
}

Collision Ellipsoid::collideEllipsoid(Ellipsoid* c2){
    Ellipsoid* c1 = this;
    Vector3 mtvCircle = Vector3(0,0,0);
    bool circleCollide = false;
    Vector3 mtvY = Vector3(0,0,0);
    bool yCollide = false;

    //Check if circles are colliding (difference in pos vs. sum of radii)
    Vector3 difference3D = c1->getPos()-c2->getPos();
    Vector2 difference2D = Vector2(difference3D.x,difference3D.z);
    float radiusSum = (c1->getDimension().x+c2->getDimension().x);
    if (difference2D.lengthSquared()<radiusSum*radiusSum){
        float length = fabs(difference2D.length());
        Vector2 m = (difference2D/length)*(radiusSum-length);
        mtvCircle = Vector3(m.x,0,m.y);
        circleCollide = true;
    }

    //Check if y-axis is colliding
    Vector2 c1y = Vector2(c1->getPos().y - c1->getDimension().y, c1->getPos().y + c1->getDimension().y);
    Vector2 c2y = Vector2(c2->getPos().y - c2->getDimension().y, c2->getPos().y + c2->getDimension().y);
    if(c1y.x<c2y.y&&c2y.x<c1y.y){  //overlap vertically
        yCollide = true;
        float aRight = c2y.y-c1y.x;
        float aLeft = c1y.y-c2y.x;
        if(aRight<aLeft)
            mtvY = Vector3(0,aRight,0);
        else
            mtvY = Vector3(0,-1*aLeft,0);
    }

    //If both circles and y-axis are overlapping, there is a collision
    if(circleCollide && yCollide){
        Vector3 mtv;
        if(mtvCircle.lengthSquared()<mtvY.lengthSquared())
            mtv = mtvCircle;
        else
            mtv = mtvY;
        Collision c = Collision(mtv*-1);
        return c;
    }
    else
        return Collision();
}

