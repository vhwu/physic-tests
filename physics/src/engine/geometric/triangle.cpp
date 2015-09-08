#include "triangle.h"
#include "geometricmanager.h"
#include "engine/common/ellipsoid.h"
#include "engine/common/graphic.h"
#include <iostream>

Triangle::Triangle(){
}

Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3){
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;

    normal = (vertices[1] - vertices[0]).cross(vertices[2] - vertices[0]);
    normal.normalize();
}

Triangle::Triangle(OBJ::Triangle t, OBJ* _obj){
    int v1 = t.a.vertex;
    int v2 = t.b.vertex;
    int v3 = t.c.vertex;
    vertices[0] = _obj->vertices.at(v1);
    vertices[1] = _obj->vertices.at(v2);
    vertices[2] = _obj->vertices.at(v3);

    normal = (vertices[1] - vertices[0]).cross(vertices[2] - vertices[0]);
    normal.normalize();

    QPair<int, int> key1(std::min(v1,v2), std::max(v1,v2));
    QPair<int, int> key2 (std::min(v2,v3), std::max(v2,v3));
    QPair<int, int> key3 (std::min(v3,v1), std::max(v3,v1));

    edges.push_back(key1);
    edges.push_back(key2);
    edges.push_back(key3);
}

//VERLET
void Triangle::adjustHitbox(Vector3& min, Vector3& max){
    for(unsigned int i = 0; i<3; i++){
        float y = vertices[i].y;
        float x = vertices[i].x;
        float z = vertices[i].z;
        max.y = (y>max.y) ? y : max.y;
        min.y = (y<min.y) ? y : min.y;
        max.x = (x>max.x) ? x : max.x;
        min.x = (x<min.x) ? x : min.x;
        max.z = (z>max.z) ? z : max.z;
        min.z = (z<min.z) ? z : min.z;
    }
}


Triangle Triangle::scale(Vector3 basis){
    return Triangle(vertices[0]*basis, vertices[1]*basis, vertices[2]*basis);
}

bool Triangle::within(const Vector3& x){
    Vector3 edge1 = vertices[0]-vertices[1];
    Vector3 edge2 = vertices[1]-vertices[2];
    Vector3 edge3 = vertices[2]-vertices[0];
    Vector3 edge4 = x-vertices[0];
    Vector3 edge5 = x-vertices[1];
    Vector3 edge6 = x-vertices[2];

    //To calculate triangle normal: cross product of two of its edges
    Vector3 normal1 = edge1.cross(edge4);
    Vector3 normal2 = edge2.cross(edge5);
    Vector3 normal3 = edge3.cross(edge6);

    //If normals of all sub-triangles are in the same direction, x is within triangle
    return (normal1.dot(normal2)>0 && normal1.dot(normal3)>0);
}

float Triangle::signedDistTo(const Vector3& point){
    return normal.dot(point-vertices[0]);
}

CollisionInfo Triangle::collide(Ellipsoid* el, const Vector3& v){
    //To work in ellipsoid space, scale triangle, velocity, + ellipsoid pos (source)
    Vector3 radius = el->getDimension();
    Vector3 basis =1.0/radius;

    Vector3 velocity = v*basis;
    Vector3 source = el->getPos()*basis;
    Triangle scaledTri = scale(basis);

    //dist to triangle
    float signedDist = scaledTri.signedDistTo(source);
    float normalDotVel = scaledTri.normal.dot(velocity);  //cached

    //Represents times during which sphere collides w/ triangle
    //sphere on front face of triangle
    float t0= (-1.0-signedDist)/normalDotVel;
    //sphere on back face of triangle
    float t1= (1.0-signedDist)/normalDotVel;

    //Swap so t0<t1
    if(t0>t1){
        float temp = t1;
        t1=t0;
        t0=temp;
    }

    //See if within triangle is within range of sphere's movement- if not, no collision
    if(t0>1.0 || t1<0.0){  //both are outside range
        return CollisionInfo();
    }

    //Clamp
    if(t0<0.0) t0=0;
    if(t1<0.0) t1=0;
    if(t0>1.0) t0=1;
    if(t1>1.0) t1=1;

    //Plane intersection: uses point closest to plane (source-normal)
    Vector3 _point = source-normal;
    Vector3 _direction = velocity;
    float t = -(scaledTri.normal.dot(_point-scaledTri.vertices[0]))/(scaledTri.normal.dot(_direction));
    Vector3 x = _direction*t+_point;

    //See if X is in triangle
    if(t>0&&scaledTri.within(x)&&t<1){
        float d = scaledTri.normal.dot(velocity);
        if(!(d>0)){ //don't test plane collision if triangle's normal faces same direction as displacement
            Vector3 planeIntersection = source-normal + (t*velocity);
            if(scaledTri.within(planeIntersection))
                return CollisionInfo(t,planeIntersection,normal,planeIntersection*radius,normal);
        }
    }

    CollisionInfo collisions[6];
    //Edge intersection
    collisions[0]=  scaledTri.collideEdge(source, velocity, 0, 1);
    collisions[1]=  scaledTri.collideEdge(source, velocity, 1, 2);
    collisions[2]=  scaledTri.collideEdge(source, velocity, 2, 0);
    //Vertex intersection
    collisions[3]= scaledTri.collideVertex(0, source, velocity);
    collisions[4]= scaledTri.collideVertex(1, source, velocity);
    collisions[5]= scaledTri.collideVertex(2, source, velocity);

    CollisionInfo toReturn = CollisionInfo();

    for(int i = 0; i<6; i++){
        CollisionInfo c = collisions[i];
        if(c.t>0&&c.t<1&&(toReturn.t==0||c.t<toReturn.t))
                toReturn = c;

    }

    //Scale back to ellipsoid space
    if(toReturn.t!=0){
        Vector3 ePos = source+velocity*toReturn.t;
        toReturn.sphere_normal = (ePos-toReturn.sphere_contactPt);
        toReturn.ellipsoid_normal = toReturn.sphere_normal/radius;
        toReturn.ellipsoid_contactPt = toReturn.sphere_contactPt*radius;
    }

    return toReturn;
}

//In which the edge of the triangle is represented as a cylinder
CollisionInfo Triangle::collideEdge(const Vector3& source,
                 const Vector3& velocity, int indexA, int indexB){
    Vector3 p1 = vertices[indexA];
    Vector3 p2 = vertices[indexB];

    Vector3 edge = p2-p1;
    Vector3 cross1 = velocity.cross(edge);
    Vector3 cross2 = (source-p1).cross(edge);
    float edgeSquared = edge.lengthSquared();

    float newT;
    float a=cross1.lengthSquared();
    float b=2*cross1.dot(cross2);
    float c=cross2.lengthSquared()-edgeSquared;

    if(quadratic(a,b,c, &newT)){
        //Check if intersection is within line segment
        float f = ((edge.dot(velocity))*newT-(edge.dot(p1-source)))/edgeSquared;
        if(f>=0.0&&f<=1.0){
            //Vector3 cylPt = source+velocity*newT;

            return CollisionInfo(newT,p1+f*edge,normal);
        }
    }
    return CollisionInfo();
}


CollisionInfo Triangle::collideVertex(int index, const Vector3& source,
                                      const Vector3& velocity){
    Vector3 p = vertices[index];
    float newT;
    float a=velocity.lengthSquared();
    float b=2*(velocity.dot(source-p));
    float c=(p-source).lengthSquared()-1;
    if(quadratic(a,b,c, &newT))
        return CollisionInfo(newT,p,normal);
    return CollisionInfo();
}

bool Triangle::quadratic(float a, float b, float c, float* t){
    float d = b*b-(4.0*a*c);

    if(d>0){  //2 intersections
        float square = sqrt(d);
        float r1 = (-b-square)/(2*a);
        float r2 = (-b+square)/(2*a);

        if(r1>r2){
            float temp = r2;
            r2=r1;
            r1=temp;
        }

        if(r1>0){
            *t=r1;
            return true;
        }
        else if(r2>0){
            *t=r2;
            return true;
        }
    }
    else if(d==0){   //1 intersection
        float r = -b/a;
        if(r>0){
            *t=r;
            return true;
        }
    }
    return false; //0 (valid) intersections
}

void Triangle::onDraw(Graphic* g){
    g->drawTriangle(vertices,normal);
}

void Triangle::drawWithOutline(Graphic* g){
    g->drawTriangle(vertices,normal);
    g->setColor(Vector3(1,1,1));
    g->drawLine(vertices[0],vertices[1]);
    g->drawLine(vertices[1],vertices[2]);
    g->drawLine(vertices[2],vertices[0]);
}

void Triangle::drawNormal(Graphic* g){
    Vector3 center = (vertices[0]+vertices[1]+vertices[2])/3.0;
    g->drawLine(center, center+normal);
}
