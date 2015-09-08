#include "net.h"

Net::Net(Vector2 dimension, const Vector3& start,
         const Vector3& interval1, const Vector3& interval2,
         VerletManager* vm): Verlet(vm)
{
    int width = dimension.x;
    int height = dimension.y;
    int count = 0;

    for(int i = 0; i<width; i++){
        for(int j = 0; j<height; j++){
            createPoint(start+j*interval1+i*interval2);
            if(j!=0)  //attach to the left
                createLink(count-1,count);
            if(i!=0)  //attach to top
                createLink(count-width, count);
            count++;
        }
    }
}

Net::~Net()
{

}

