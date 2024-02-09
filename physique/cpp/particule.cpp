#include "../header/particule.h"
#include <math.h>
#include <iostream>
#define PI 3.14159265
using namespace std;
void Particule::initialise_particules(float xini, float yini,float vxini,float vyini,float rayonini){
    x=xini;
    y=yini;
    vx=vxini;
    vy=vyini;
    rayon=rayonini;
}
void Particule::inirapide(){
    initialise_particules(0,0,0,0,0.1);
}

void Particule::position_particule(unsigned int nb_points,float* position){
    for (unsigned int i=0; i<nb_points;i++){
        position[2*i]   =   x + rayon*cos(2*PI*i/nb_points);
        position[2*i+1] =   y + rayon*sin(2*PI*i/nb_points);
        //cout << position [2*i] << " " <<position[2*i+1] << endl;
    }
}
void Particule::collision(float amorti){
    if (x+rayon >1){x=1-rayon-amorti*(x+rayon-1);vx*=-1*amorti;}
    if (x-rayon <-1){x=-1+rayon+amorti*(-x+rayon-1);vx*=-1*amorti;}
    if (y+rayon >1){y=1-rayon-amorti*(y+rayon-1);vy*=-1*amorti;}
    if (y-rayon <-1){y=-1+rayon+amorti*(-y+rayon-1);vy*=-1*amorti;}
}