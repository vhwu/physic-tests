#ifndef CLOTH_H
#define CLOTH_H

#include "verlet.h"
#include <vector>

class Cloth: public Verlet
{
public:
    //@param axis: 1 = x, 2 = y, 3 = z
    //Cloth's points will be unchanged along specified axis,
    //and be spaced (by -width) along other two axis
    Cloth(const Vector2& dimension, float width,
          const Vector3& start, int axis, VerletManager* vm);
    ~Cloth();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void pinCorner(int c);
private:
    //Called in constructor to add triangles
    void triangulate(int a, int b, int c);
    std::vector<Tri*> _triangles;
};

#endif // CLOTH_H
