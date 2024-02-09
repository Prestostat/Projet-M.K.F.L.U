#include "../header/ensemble.h"
#include "../header/particule.h"

Ensemble::Ensemble(unsigned int nb,float rayon){
    nombre_de_particules=nb;
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