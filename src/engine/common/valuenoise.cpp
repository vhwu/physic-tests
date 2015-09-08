#include "valuenoise.h"
#include <math.h>
#include <stdlib.h>

ValueNoise::ValueNoise()
{
}

ValueNoise::~ValueNoise()
{

}

//between -1 and 1
float ValueNoise::noise(int x, int y){

    srand(seed*123343443*(y+34590)*(x+3827429)*15243731);
    return (((float)rand()/RAND_MAX)*2) -1;
}

float ValueNoise::smoothNoise(Vector2 coor){
    float x = coor.x;
    float y = coor.y;
    float corners = (noise(x-1, y-1)+noise(x+1, y-1)+noise(x-1, y+1)+noise(x+1, y+1)) / 16.0;
    float sides = (noise(x-1, y)+noise(x+1,y)+noise(x, y-1)+ noise(x, y+1)) / 8.0;
    float center = noise(x,y)/4;
    return corners+sides+center;
}

float ValueNoise::interpolate(float a, float b, float x){
    float f = (1-cos(x*M_PI))*.5;
    float v = a*(1-f)+(b*f);
    return v;
}

float ValueNoise::interpolatedNoise(Vector2 coor){
    int x = (int)coor.x;
    float frac_x = coor.x-x;
    int y = (int)coor.y;
    float frac_y = coor.y-y;

    float v1=smoothNoise(Vector2(x,y));
    float v2=smoothNoise(Vector2(x+1,y));
    float v3=smoothNoise(Vector2(x,y+1));
    float v4=smoothNoise(Vector2(x+1,y+1));

    float i1= interpolate(v1,v2,frac_x);
    float i2= interpolate(v3,v4,frac_x);

    return interpolate(i1,i2,frac_y);
}

float ValueNoise::valueNoise(Vector2  coor){
    float total = 0;
    float amp = persistence;

    for(int i =0; i<octaves; i++){

        float freq = pow(2,i);
        amp*=persistence;
        total+= interpolatedNoise(Vector2(coor.x*freq, coor.y*freq))*amp;

    }
    return total+.3f;
}
