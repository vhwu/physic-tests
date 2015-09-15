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
          const Vector3& start, Axis a, VerletManager* vm);
    ~Cloth();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    //For setting up
    void drawCorners(Graphic* g); //Color-codes corner order: RGB White
    void printCorners(); //Prints index + location of each corner
    bool visualize = false;

    //Returns index of corners (0-3)
    int getCorner(int c);
    //Pins all corners
    void pinCorners();
private:
    //Called in constructor to add triangles
    void triangulate(int a, int b, int c);
    std::vector<Tri*> _triangles;
};

#endif // CLOTH_H
