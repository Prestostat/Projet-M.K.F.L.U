#include "../header/particule.h"
#include <math.h>
#include <iostream>
#include "///usr/include/GL/glew.h"
#include <GLFW/glfw3.h>
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


void Particule::random_initialise_particules(float rayonini){
    float d=2.0/RAND_MAX;
    float a =std::rand()*d-1;
    float b =std::rand()*d-1;
    x=a;
    y=b;
    x_predit=a;
    y_predit=b;
    vx=std::rand()*d*10-10;
    vy=std::rand()*d*10-10;
    rayon=rayonini;
}


void Particule::position_particule(unsigned int nb_points,float rayon_influence,float* position){
    for (unsigned int i=0; i<nb_points;i++){
        position[2*i]   =   x + 2*rayon_influence*cos(2*PI*i/nb_points);
        position[2*i+1] =   y + 2*rayon_influence*sin(2*PI*i/nb_points);
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


float Particule::grad_influence_proche(float ex,float ey,float rayon_influence){
    float distance = sqrt((x-ex)*(x-ex)+(y-ey)*(y-ey));
    if (distance>rayon_influence){
        return(0);
    }
    else {
        return (derive_fonction_influence_proche(distance, rayon_influence));
    }
}


float derive_fonction_influence_proche(float x, float rayon_influence){
    float volume = 2*PI*pow(rayon_influence,5)/5;
    return(4*pow((x-rayon_influence),5)/volume);
}


int* Particule::coordonnee(float rayon_influence){
    int* coord = (int*)malloc(2*sizeof(int));
    coord[0]=(int)(x/rayon_influence);
    coord[1]=(int)(y/rayon_influence);
    return(coord);
}


int* coordonnee(float ex,float ey,float rayon_influence){
    int* coord = (int*)malloc(2*sizeof(int));
    coord[0]=(int)(ex/rayon_influence);
    coord[1]=(int)(ey/rayon_influence);
    return(coord);
}


int Particule::cle(float rayon_influence, int nombre_de_particules){
    int a = 15823;
    int b =9737333;
    int* coord = coordonnee(rayon_influence);
    int temp = (coord[0]*a+coord[1]*b)%nombre_de_particules;
    temp+= nombre_de_particules;
    temp = temp%nombre_de_particules;
    return(temp);
}


int cle(int* coord, int nombre_de_particules){
    int a = 15823;
    int b =9737333;
    int temp = (coord[0]*a+coord[1]*b)%nombre_de_particules;
    temp+= nombre_de_particules;
    temp = temp%nombre_de_particules;
    return(temp);
}  
float* Particule::couleur(float vc){
    float v=sqrt(vx*vx+vy*vy);
    float* couleur=(float*)malloc(3*sizeof(float));
    couleur[1]=0;
    if (v<vc) {
        couleur[0]=(v/vc)*(v/vc);
        couleur[2]=1;
    }
    else if (v/vc<2.0){
        couleur[0]=1;
        couleur[2]=1-(v/vc-1)*(v/vc-1);
    }
    else {
        couleur[0]=1;
        couleur[2]=0;
    }
    return(couleur);
}
