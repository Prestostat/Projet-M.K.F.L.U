#include "../header/ensemble.h"
#include "../header/particule.h"
#include <math.h>
#include <iostream>
using namespace std;

Ensemble::Ensemble(unsigned int nb,float rayon){
    nombre_de_particules=nb;
    masse = 1;
    multiplicateur_pression = 500000;
    densite_visee=500;
    data = new Particule[nb];
    unsigned int N; // nombre dont le carré est le 1er carré supérieur à nb
    unsigned int temp =0;
    while (temp*temp<nb) {
        temp+=1;
        N=temp;
    }
    for (unsigned int i=0;i<nb;i++){
        data[i].initialise_particules(-1+1.0/N+(i%N)*2.0/N,-1+1.0/N+(i/N)*2.0/N,0,0,rayon);   //place les particles dans un carré remplissant l'espace de l'écran
                                                                                              //(de (-1,-1) à (1,1) dépendant de leur nombre
    }
}
Ensemble::~Ensemble(){
    if (data!= nullptr) {delete[] data;}
}
Ensemble::Ensemble(const Ensemble& other) {
    data=new Particule[other.nombre_de_particules];
    for(unsigned int i=0;i<other.nombre_de_particules;i++){
        data[i]=other.data[i];
    }

}

void Ensemble::gravite(float dt,float g) {
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].vy-=g*dt;
    }
}

void Ensemble::deplacement(float dt) {
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].x+=data[i].vx*dt;
        data[i].y+=data[i].vy*dt;
        data[i].collision(0.5);
    }
}
void Ensemble::evolution(float dt, float g,float rayon_influence){
    gravite(dt,g);
    force_pression(dt,rayon_influence);
    deplacement(dt);
}
float Ensemble::densite_ponctuelle(float ex,float ey,float rayon_influence){
    float d=0;
    for(unsigned int i=0;i<nombre_de_particules;i++){
    d+=masse*data[i].influence(ex,ey,rayon_influence);
    }
    return(d);
}
float* Ensemble::densite(float rayon_influence){
    float* dens=(float*)malloc(nombre_de_particules*sizeof(float));
    for(unsigned int i=0;i<nombre_de_particules;i++){
        dens[i]=densite_ponctuelle(data[i].x,data[i].y,rayon_influence);
    }
    return(dens);
}
float* Ensemble::pression_ponctuelle(unsigned int n,  float* densite,float rayon_influence){
    float* pression = (float*)malloc(2*sizeof(float));
    pression[0]=0;
    pression[1]=0;
    for (unsigned int i=0; i<nombre_de_particules;i++){
        if (i!=n) {
            float distance = sqrt((data[n].x-data[i].x)*(data[n].x-data[i].x)+(data[n].y-data[i].y)*(data[n].y-data[i].y));
            if (distance == 0) {cout << "distance entre 2 particule égale a 0, force nan" << endl;}
            float dirx = (data[n].x-data[i].x)/distance;
            float diry = (data[n].y-data[i].y)/distance;
            float grad_influence = data[i].grad_influence(data[n].x,data[n].y,rayon_influence);
            float pression_mutuelle;
            pression_mutuelle = ((-densite[i]+densite_visee)+(-densite[n]+densite_visee))/2.0*multiplicateur_pression;
            pression[0]+=(pression_mutuelle*grad_influence*dirx/densite[i]);
            pression[1]+=(pression_mutuelle*grad_influence*diry/densite[i]);
        }
    }
    
    return(pression);
}

void Ensemble::force_pression(float dt,float rayon_influence){
    float* pression = (float*)malloc(2*sizeof(float));
    float* d = densite(rayon_influence);
    for(unsigned int i=0; i<nombre_de_particules;i++){
        pression = pression_ponctuelle(i,d,rayon_influence);
        data[i].vx+=dt*pression[0]/d[i];
        data[i].vy+=dt*pression[1]/d[i];
    }
    free(d);free(pression);

}