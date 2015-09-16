#ifndef CLOTH_H
#define CLOTH_H

#include "verlet.h"
#include <vector>

class Cloth: public Verlet
{
public:
    //Cloth's points will be unchanged along specified axis,
    //and be spaced (by -width) along other two axis
    Cloth(const Vector2& dimension, float width,
          const Vector3& start, Axis a, VerletManager* vm, int angle = 0);
    ~Cloth();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    //For setting up
    void printCorners(); //Prints index + location of each corner
    bool visualize = false;  //Set to true to visualize color-coded corners: RGB White
    int getCorner(int c); //Returns index of corners (0-3)
    void pinCorners(); //Pins all corners

private:
    //Called in constructor to add triangles
    void triangulate(int a, int b, int c);

    //Helper
    void drawCorners(Graphic* g);

    //For calculating rotated triangles
    QPair<Vector2,Vector2> rotateTriangle(float _w,int angle); //returns <width,height>
    Vector2 rotatePoint(const Vector2& p, int angle); //helper

    std::vector<Tri*> _triangles;
};

#endif // CLOTH_H
