#ifndef NET_H
#define NET_H

#include "verlet.h"

class Net: public Verlet
{
public:
    //@param dimension: how many points in the net
    //@param start: where first point of net is created
    //@param interval1 + 2: spacing from one point to the next, 'horizontally' and 'vertically'
    Net(Vector2 dimension, const Vector3& start,
        const Vector3& interval1, const Vector3& interval2,
        VerletManager* vm);
    ~Net();
};

#endif // NET_H
