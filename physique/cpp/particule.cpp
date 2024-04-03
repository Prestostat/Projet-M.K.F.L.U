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
void Particule::position_particule_carre(float* position){
    position[0]=x-rayon;
    position[1]=y-rayon;
    position[2]=0;
    position[3]=0;
    position[4]=x-rayon;
    position[5]=y+rayon;
    position[6]=0;
    position[7]=1;
    position[8]=x+rayon;
    position[9]=y-rayon;
    position[10]=1;
    position[11]=0;
    position[12]=x+rayon;
    position[13]=y+rayon;
    position[14]=1;
    position[15]=1;




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


void Particule::coordonnee(int* coord,float rayon_influence){
    coord[0]=(int)(x/rayon_influence);
    coord[1]=(int)(y/rayon_influence);
}


void coordonnee(int* coord, float ex,float ey,float rayon_influence){
    coord[0]=(int)(ex/rayon_influence);
    coord[1]=(int)(ey/rayon_influence);
}


int Particule::cle(float rayon_influence, int nombre_de_particules){
    int a = 15823;
    int b =9737333;
    int* coord = (int*)malloc(2*sizeof(int));
    coordonnee(coord,rayon_influence);
    int temp = (coord[0]*a+coord[1]*b)%nombre_de_particules;
    temp+= nombre_de_particules;
    temp = temp%nombre_de_particules;
    free(coord);
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
void Particule::couleur(float* couleur, float vc,float opacite,bool affiche_vitesses_colorees){
    float v=sqrt(vx*vx+vy*vy);
    
    couleur[1]=0;
    couleur[3]=opacite;
    if (!affiche_vitesses_colorees){couleur[0]=0;couleur[2]=1;}
    else if (v<vc) {
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
    
}

