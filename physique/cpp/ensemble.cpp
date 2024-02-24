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
        //data[i].initialise_particules(-1+1.0/N+(i%N)*2.0/N,-1+1.0/N+(i/N)*2.0/N,0,0,rayon);   //place les particles dans un carré remplissant l'espace de l'écran
        //data[i].initialise_particules(-0.5+0.5/N+(i%N)*1.0/N,-0.5+0.5/N+(i/N)*1.0/N,0,0,rayon); // CARRÉ
        data[i].random_initialise_particules(rayon); // RANDOM                                                                                     //(de (-1,-1) à (1,1) dépendant de leur nombre
    }
}
Ensemble::~Ensemble(){
    if (data!= nullptr) {delete[] data;}
    if (indice_debut != nullptr) {delete[] indice_debut;}
    for(unsigned int i;i<nombre_de_particules;i++){
        if(indices[i]!= nullptr) {delete[] indices[i];}
    }
    if (indices!=nullptr){
        delete[] indices;
    }
} // a modifier pour delete tous les tableaux
Ensemble::Ensemble(const Ensemble& other) {
    data=new Particule[other.nombre_de_particules];
    for(unsigned int i=0;i<other.nombre_de_particules;i++){
        data[i]=other.data[i];
    }

}

void Ensemble::gravite(float dt,float g) {
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].vy-=g*dt;
        data[i].x_predit = data[i].x + data[i].vx*1/10000.0;
        data[i].y_predit = data[i].y + data[i].vy*1/10000.0;
    }
}

void Ensemble::deplacement(float dt) {
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].x+=data[i].vx*dt;
        data[i].y+=data[i].vy*dt;
        data[i].collision(coeff_amorti);
    }
}
void Ensemble::evolution(float dt, float g,float rayon_influence, float m,float multipression,float dvisee,float coef,float viscstrength,bool clique_gauche,bool clique_droit,float sourisx,float sourisy,float rayon_action_clique_gauche, float puissance_action){
    masse = m;
    multiplicateur_pression = multipression;
    densite_visee = dvisee;
    coeff_amorti = coef;
    gravite(dt,g);
    indices = liste_indice (rayon_influence);
    tri_liste_indice(indices);
    indice_debut = liste_indice_debut(indices);
    float* d = densite(rayon_influence);
    force_pression(dt,rayon_influence,d);
    visc(dt, rayon_influence, viscstrength);
    force_souris(dt,clique_gauche,clique_droit,sourisx,sourisy,rayon_action_clique_gauche,puissance_action);
    deplacement(dt);
    free(d);
}
float Ensemble::densite_ponctuelle(float ex,float ey,float rayon_influence){
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
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){
                d+=masse*data[indices[j][0]].influence(ex,ey,rayon_influence);
                j++;
                if ((unsigned int)j==nombre_de_particules){
                    break;
                }
            }
        }
    }

    free(coord);free(coord_temporaire);free(liste_cellules);
    return(d);
}
float Ensemble::densite_ponctuelle_naive(float ex,float ey,float rayon_influence) // plus mauvaise complexité que celle au dessus
{
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
        //dens[i]= densite_ponctuelle_naive(data[i].x,data[i].y,rayon_influence);
        
    }
    return(dens);
}
void Ensemble::pression_ponctuelle(unsigned int n,  float* densite,float rayon_influence,float* pression){
    pression[0]=0;
    pression[1]=0;
    int* coord = coordonnee(data[n].x,data[n].y,rayon_influence);
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
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){
                unsigned int m=indices[j][0];
                if (m!=n) {
                    float distance = sqrt((data[n].x-data[m].x)*(data[n].x-data[m].x)+(data[n].y-data[m].y)*(data[n].y-data[m].y));
                    if (distance == 0) {
                        float d=0.001/RAND_MAX;
                        float x =(-0.5+std::rand())*d+0.0000001;
                        float y =(-0.5+std::rand())*d+0.0000001;
                        distance = sqrt(x*x+y*y); 
                        float dirx = x/distance;
                        float diry = y/distance;
                        float grad_influence = data[m].grad_influence(data[m].x+x,data[m].y+y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = ((-densite[m]+densite_visee)+(-densite[n]+densite_visee))/2.0*multiplicateur_pression;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/densite[m]);
                        pression[1]+=(pression_mutuelle*grad_influence*diry/densite[m]);
                        }
                    else {
                        float dirx = (data[n].x-data[m].x)/distance;
                        float diry = (data[n].y-data[m].y)/distance;
                        float grad_influence = data[m].grad_influence(data[n].x,data[n].y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = ((-densite[m]+densite_visee)+(-densite[n]+densite_visee))/2.0*multiplicateur_pression;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/densite[m]);
                        pression[1]+=(pression_mutuelle*grad_influence*diry/densite[m]);
                    }
         
                }
                j++;
                if ((unsigned int)j==nombre_de_particules){
                    break;
                }
            }
        }
    }
    free(coord);free(coord_temporaire);free(liste_cellules);
}

void Ensemble::force_pression(float dt,float rayon_influence,float* d){
    float* pression = (float*)malloc(2*sizeof(float));
    for(unsigned int i=0; i<nombre_de_particules;i++){
        pression_ponctuelle(i,d,rayon_influence,pression);
        data[i].vx+=dt*pression[0]/d[i];
        data[i].vy+=dt*pression[1]/d[i];
    }
    free(pression);
    

}

void Ensemble::visc_ponctuelle(unsigned int n,float rayon_influence, float viscstrength, float* visc){
    visc[0]=0;
    visc[1]=0;
    int* coord = coordonnee(data[n].x,data[n].y,rayon_influence);
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
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){
                unsigned int m=indices[j][0];
                if (m!=n) {
                    float infl = data[i].influence(data[n].x,data[n].y,rayon_influence);
                    visc[0] += (data[i].vx-data[n].vx)*infl*viscstrength;
                    visc[1] += (data[i].vy-data[n].vy)*infl*viscstrength;
                    }
                j++;
                if ((unsigned int)j==nombre_de_particules){
                    break;
                }
            }
        }
    }
    free(coord);free(coord_temporaire);free(liste_cellules);
    
}
void Ensemble::visc(float dt, float rayon_influence,float viscstrength){
    float* viscosite = (float*)malloc(2*sizeof(float));
    for(unsigned int i=0; i<nombre_de_particules;i++){
        visc_ponctuelle(i,rayon_influence,viscstrength,viscosite);
        data[i].vx += dt*viscosite[0];
        data[i].vy += dt*viscosite[1];
    }
    free(viscosite);
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

void Ensemble::force_souris(float dt,bool clique_gauche,bool clique_droit, float sourisx, float sourisy,float rayon_action_clique_gauche,float puissance_action){
    if (clique_gauche && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        float x = 2*sourisx/960.0 -1;
        float y = -sourisy*2/960.0 +1;
        for(unsigned int i=0;i<nombre_de_particules;i++){
            float dst = sqrt(pow((data[i].x-x),2)+pow((data[i].y)-y,2));
            if (dst==0){
                dst+=1;
            }
            if (dst<rayon_action_clique_gauche){  
                float infl = fonction_influence(dst,rayon_action_clique_gauche);
                data[i].vx*=0.9;
                data[i].vy*=0.9;
                data[i].vx+=dt*infl*puissance_action*((data[i].x-x)/dst);
                data[i].vy+=dt*infl*puissance_action*((data[i].y-y)/dst);
            }
            
        }
    }
    if (clique_droit && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        float x = 2*sourisx/960.0 -1;
        float y = -sourisy*2/960.0 +1;
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx=0;
            data[i].vy=0;
        }
            
        
    }
}