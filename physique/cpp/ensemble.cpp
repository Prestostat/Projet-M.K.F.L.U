#include "../header/ensemble.h"
#include "../header/particule.h"
#include <math.h>
#include <iostream>
#define retrun return //permet d'écrir retrun a la place de return :sunglases:
using namespace std;

Ensemble::Ensemble(unsigned int nb,float rayon){
    nombre_de_particules=nb;
    masse = 1;
    multiplicateur_pression = 1000000;
    densite_visee=200;
    coeff_amorti = 1;
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
        data[i].collision(coeff_amorti);
    }
}
void Ensemble::evolution(float dt, float g,float rayon_influence, float m,float multipression,float dvisee,float coef){
    masse = m;
    multiplicateur_pression = multipression;
    densite_visee = dvisee;
    coeff_amorti = coef;
    gravite(dt,g);
    force_pression(dt,rayon_influence);
    deplacement(dt);
}
float Ensemble::densite_ponctuelle(float ex,float ey,float rayon_influence, int** liste_indice, int* indice_debut){
    float d=0;
    int* coord = coordonnee(ex,ey,rayon_influence);
    int* coord_temporaire = (int*)malloc(2*sizeof(int));            
    int* liste_cellules = (int*)malloc(9*sizeof(int));

    for(int i=0;i<9;i++){
        coord_temporaire[0]=coord[0]-1+i%3;
        coord_temporaire[1]=coord[1]-1+i/3;
        liste_cellules[i] = cle(coord_temporaire,nombre_de_particules);
    }
    for(unsigned int i=0;i<9;i++){
        int j=indice_debut[liste_cellules[i]];
        if (j>=0) {
            while(liste_indice[j][1]==liste_indice[indice_debut[liste_cellules[i]]][1]){
                d+=masse*data[liste_indice[j][0]].influence(ex,ey,rayon_influence);
                //cout << masse*data[liste_indice[j][0]].influence(ex,ey,rayon_influence) << endl;
                j++;
            }
        }
    }
    return(d);
}
float Ensemble::densite_ponctuelle_naive(float ex,float ey,float rayon_influence){
    float d=0;
    for(unsigned int i=0;i<nombre_de_particules;i++){
    d+=masse*data[i].influence(ex,ey,rayon_influence);
    }
    return(d);
}
float* Ensemble::densite(float rayon_influence){
    float* dens=(float*)malloc(nombre_de_particules*sizeof(float));
    int** indices = liste_indice (rayon_influence);
    tri_liste_indice(indices);
    int* indice_debut = liste_indice_debut(indices);
    //for (unsigned int i=0;i<nombre_de_particules ; i++){cout << indice_debut[i] << endl;}
    for(unsigned int i=0;i<nombre_de_particules;i++){
        //dens[i]=densite_ponctuelle(data[i].x,data[i].y,rayon_influence,indices,indice_debut);
        dens[i]= densite_ponctuelle_naive(data[i].x,data[i].y,rayon_influence);
        
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
            else {
            float dirx = (data[n].x-data[i].x)/distance;
            float diry = (data[n].y-data[i].y)/distance;
            float grad_influence = data[i].grad_influence(data[n].x,data[n].y,rayon_influence);
            float pression_mutuelle;
            pression_mutuelle = ((-densite[i]+densite_visee)+(-densite[n]+densite_visee))/2.0*multiplicateur_pression;
            pression[0]+=(pression_mutuelle*grad_influence*dirx/densite[i]);
            pression[1]+=(pression_mutuelle*grad_influence*diry/densite[i]);
            }
         
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

int** Ensemble::liste_indice( float rayon_influence) {
    int** liste= (int**)malloc(nombre_de_particules*sizeof(int*));
    for (unsigned int i=0; i<nombre_de_particules;i++){
        liste[i] = (int*)malloc(2*sizeof(int));
        liste[i][0] = (int)i;
        liste[i][1]= data[i].cle(rayon_influence,nombre_de_particules);
    }
    return(liste);
}

int compare(const void* a,const void* b)
{
    if( (*(int**)a)[1] < (*(int**)b)[1]  ) {return(-1);}
    if( (*(int**)a)[1] == (*(int**)b)[1]  ) {return(0);}
    if( (*(int**)a)[1] > (*(int**)b)[1]  ) {return(1);}
    return(0);
}

void Ensemble::tri_liste_indice( int** liste ){
    qsort(liste,nombre_de_particules,sizeof(int*),compare);
}
int* Ensemble::liste_indice_debut(int**liste){
    int* debut_indice = (int*)malloc(nombre_de_particules*sizeof(int));
    for (unsigned int i=0;i<nombre_de_particules;i++){debut_indice[i]=-1;}
    int temp =-1;
    for(unsigned int i=0;i<nombre_de_particules;i++){
        if (temp != liste[i][1]) {
            temp = liste[i][1];
            debut_indice[temp]=i;
        }
    }
    return(debut_indice);
}