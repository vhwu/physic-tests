#include "cloth.h"

Cloth::Cloth(const Vector2 &dimension, float w,
             const Vector3 &start, Axis a, VerletManager *vm): Verlet(vm)
{
    int r = dimension.y;
    int c = dimension.x;
    row = r;
    col = c;

    float h = (sqrt(3)/2.0) * w;

    Vector3 width;
    Vector3 height;

    if(a==X){
        width = Vector3(0,0,-w);
        height = Vector3(0,-h,0);
    }
    else if(a==Y){
        width = Vector3(0,0,-w);
        height = Vector3(-h,0,0);
    }
    else{
        width = Vector3(-w,0,0);
        height = Vector3(0,-h,0);
    }

    Vector3 half_width = width*.5;

    //Create points
    for(int i =0; i<r; i++){
        for(int j = 0; j<c; j++){
            if(i%2==0){  //even
                if(j==c-1)
                    createPoint(start+half_width+(j-1)*width+i*height);
                else
                    createPoint(start+j*width+i*height);
            }
            else{  //odd
                if(j==0)
                    createPoint(start+i*height);
                else
                    createPoint(half_width+start+(j-1)*width+i*height);
            }
        }
    }

    //Create structural links
    for(int i =0; i<r; i++){
        for(int j = 0; j<c; j++){
            int index = j+i*c;
            if(j<c-1)  //horizontal
                createLink(index, index+1);
            if(i<r-1)  //vertical
                createLink(index, index+c);
            if(j<c-1&&i<r-1&&i%2==0) //diagonal on even rows
                createLink(index, index+c+1);
            if(j>0&&i<r-1&&i%2==1) //diagonal on odd rows
                createLink(index, index+c-1);
        }
    }

    //Triangulate
    for(int i =0; i<r-1; i++){
        for(int j = 0; j<c; j++){
            if(i%2==0){  //even
                if(j<c-1){
                    //upper triangles
                    int n = c*i+j;
                    triangulate(n,n+c+1,n+c);
                    //lower triangles
                    triangulate(n,n+1,n+c+1);
                }
            }
            else{  //odd
                if(j!=0){
                    int n = c*i+j;
                    //upper triangles
                    triangulate(n,n+c,n+c-1);
                    //lower triangles
                    triangulate(n,n+c-1,n-1);
                }
            }
        }
    }

    //Create shear and bend constraints
    for(int i = 0; i<r; i++){
        for(int j = 0; j<c; j++){
            if(j+2<c)  //horizontal
                createLink(j+(i*c),(i*c)+j+2);
            if(i+2<r && (j==0 || j==c-1)) //vertical, at edges
                createLink(j+(i*c),(i+2)*c+j);
            if(i+2<r && j+1<c){ //diagonal right-down
                if(j+1!=c-1&& (j!=0||i%2==0)){
                    if(i%2==0)
                        createLink(j+c*i,j+1+c*(i+2));
                    else
                        createLink(j+c*i,j+1+c*(i+2));
                }
                else if(j+1==c-1 && i%2==1)
                    createLink(j+c*i,j+1+c*(i+2));
            }
            if(i+2<r && j>0 && j-1<c){ //diagonal left-down
                if((!(j==c-1&&i%2==0)) && (!(j==1&&i%2!=0))){
                    if(i%2==1)
                        createLink(j+c*i,j-1+c*(i+2));
                    else
                        createLink(j+c*i,j-1+c*(i+2));
                }
            }
        }
    }
}

void Cloth::onTick(float ){
    for(unsigned int i = 0; i<_triangles.size(); i++)
        calculate(_triangles.at(i));
}

void Cloth::onDraw(Graphic* g){
    g->setColor(Vector3(1,1,1));
    Verlet::onDraw(g);

    g->cull(false);
    for(Tri* t: _triangles){
        g->setColor(Vector4(t->normal.x*.4,.65+t->normal.y*.3,.77+t->normal.z*.15,1));
        g->drawTriangle(t->vertices,t->normal);
    }
    g->cull(true);
}

void Cloth::triangulate(int a, int b, int c){
    Tri* t = new Tri(a,b,c);
    calculate(t);
    _triangles.push_back(t);
}

int Cloth::getCorner(int c){
    if(c==0)
        return 0;
    else if(c==1)
        return col-1;
    else if(c==2)
        return row*col-col;
    else
        return row*col-1;
}

void Cloth::pinCorners(){
    this->createPin(getCorner(0));
    this->createPin(getCorner(1));
    this->createPin(getCorner(2));
    this->createPin(getCorner(3));
}

Cloth::~Cloth()
{
    for (std::vector<Tri*>::iterator it = _triangles.begin() ; it != _triangles.end(); ++it)
        delete (*it);
    _triangles.clear();
}

