#include "link.h"

Link::Link(int a, int b, float length)
{
//    if(b>a)
//        std::cout<<"constructing link: b greater than 1"<<std::endl;
//    pointA = a;
//    pointB = b;
    if(a<b){
        pointA = a;
        pointB = b;
    }
    else{
        pointA = b;
        pointB = a;
    }

    restLength = length;
    restLengthSq = restLength*restLength;
}

Link::~Link()
{

}
