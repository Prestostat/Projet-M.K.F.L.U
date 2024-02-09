#include "../header/ensemble.h"
#include "../header/particule.h"

Ensemble::Ensemble(unsigned int nb,float rayon){
    nombre_de_particules=nb;
    masse = 1;
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
        data[i].collision(0.9);
    }
}
void Ensemble::evolution(float dt, float g){
    gravite(dt,g);
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
    float densite[nombre_de_particules];
    for(unsigned int =0;i<nombre_de_particules;i++){
        densite[i]=densite_ponctuelle(data[i].x,data[i].y,rayon_influence);
    }
    return(densite);
}
float* Ensemble::gradient_densite(float ex, float ey, float* densite,float rayon_influence){
    float grad[2] ={0,0};
    for (unsigned int i=0; i<nombre_de_particules;i++){
        distance = sqrt((ex-data[i].x)*(ex-data[i].x)+(yx-data[i].y)*(ey-data[i].y));
        float dirx = (ex-data[i].x)/distance;
        float diry = (ey-data[i].y)/distance;
        gradinfluence = data[i].gradinfluence(ex,ey,rayon_influence);
        grad[0]

    }

}
