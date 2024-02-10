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
float fonction_influence(float x,float rayon_influence){
    float volume = PI*pow(rayon_influence,4)/6; //normalisation pour que l'intégrale du volume de l'influence soit 1, a essayer en exponentielle décroissante ?
    return(pow((rayon_influence-x),2)/volume);

}
float Particule::influence(float ex,float ey,float rayon_influence){
    float distance = sqrt((x-ex)*(x-ex)+(y-ey)*(y-ey));
    if (distance>rayon_influence){
        return(0);
    }
    else {
        return (fonction_influence(distance, rayon_influence));

    }


}
 float derive_fonction_influence(float x, float rayon_influence){
    float volume = PI*pow(rayon_influence,4)/6;
    return(2*(x-rayon_influence)/volume);
 }
 float Particule::grad_influence(float ex,float ey,float rayon_influence){
    float distance = sqrt((x-ex)*(x-ex)+(y-ey)*(y-ey));
    if (distance>rayon_influence){
        return(0);
    }
    else {
        return (derive_fonction_influence(distance, rayon_influence));
    }
 }