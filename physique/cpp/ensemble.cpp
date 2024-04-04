#include "../header/ensemble.h"
#include "../header/particule.h"
#include "../header/obstacle.h"
#include <math.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#define E 2.71828182846
#define retrun return //permet d'écrir retrun a la place de return :sunglases:
using namespace std;

Ensemble::Ensemble(unsigned int nb,float rayon){
    nombre_de_particules=nb;
    data = new Particule[nb];
    indices = (int**)malloc(nb*sizeof(int*));
    indice_debut = new int[nb];
    for (unsigned int i=0;i<nb;i++){indices[i]= (int*)malloc(2*sizeof(int));}
   
    
    unsigned int N; // nombre dont le carré est le 1er carré supérieur à nb
    unsigned int temp =0;
    while (temp*temp<nb) {
        temp+=1;
        N=temp;
    }
        for (unsigned int i=0;i<nb;i++){ 
        
        //data[i].initialise_particules(-1+1.0/N+(i%N)*2.0/N,-1+1.0/N+(i/N)*2.0/N,0,0,rayon);   //place les particles dans un carré remplissant l'espace de l'écran
        data[i].initialise_particules(-0.5+0.5/N+(i%N)*1.0/N,-0.5+0.5/N+(i/N)*1.0/N,0,0,rayon); // CARRÉ  
        //data[i].random_initialise_particules(rayon, Obs); // RANDOM  
                                                              //(de (-1,-1) à (1,1) dépendant de leur nombre
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
}


Ensemble::Ensemble(const Ensemble& other) {
    data=new Particule[other.nombre_de_particules];
    for(unsigned int i=0;i<other.nombre_de_particules;i++){
        data[i]=other.data[i];
    }

}

void Ensemble::actualise_constantes(float rayon_collision_,float g_,float masse_,float multiplicateur_pression_,float multiplicateur_pression_proche_,float densite_visee_,float dt_,float rayon_influence_,float coeff_amorti_,float coeff_viscosite_,float sourisx_,float sourisy_,float rayon_action_clique_gauche_,float puissance_action_clique_gauche_,bool clique_gauche_,bool clique_droit_,bool a,bool z,bool e,bool q,bool s, bool d, bool pause_){
    //Actualise les constantes de l'ensemble, car elles peuvent changer a cause de ImGui
    rayon_collision = rayon_collision_;
    g = g_;
    masse =masse_;
    multiplicateur_pression = multiplicateur_pression_;
    multiplicateur_pression_proche = multiplicateur_pression_proche_;
    densite_visee = densite_visee_;
    dt =dt_;
    rayon_influence =rayon_influence_;
    coeff_amorti = coeff_amorti_;
    coeff_viscosite = coeff_viscosite_;
    sourisx = sourisx_;
    sourisy = sourisy_;
    rayon_action_clique_gauche =rayon_action_clique_gauche_;
    puissance_action_clique_gauche = puissance_action_clique_gauche_;

    clique_gauche=clique_gauche_;
    clique_droit=clique_droit_;
    a_key=a;
    z_key=z;
    e_key=e;
    q_key=q;
    s_key=s;
    d_key=d;
    pause=pause_;
}

void Ensemble::gravite() {
    //ajoute l'effet de la force de gravité sur les particules
    ////#pragma omp parallel for
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].vy-=g*dt;
    }
}


void Ensemble::deplacement() {
    //Effectue le déplacement des particules pour passer au positions de l'étape suivante
    ////#pragma omp parallel for
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].x+=data[i].vx*dt;
        data[i].y+=data[i].vy*dt;
        data[i].collision(coeff_amorti); // Gère les rebonds des particules sur la boite
    }
}

void Ensemble::actualise_listes(){
    //Actualise les lites indices (contenant la liste des particules et de la clé  de la case dans laquelle elles sont)
    // et indices début, donnant l'indice ou trouver la première particule en fonction d'une clé de case.
    liste_indice (indices);
    tri_liste_indice(indices);
    liste_indice_debut(indice_debut,indices);

}




float maxi(float a,float b){
    if(a<b){return(b);}
    else{return(a);}
}


float aire(float rayon, float distance){
    float res = pow(rayon, 2)*(acos(distance/rayon)-(distance/rayon)*sqrt(1-pow(distance/rayon, 2)));
    return res;
}


float aire_triangle(float base, float hauteur){
    float res = base*hauteur/2;
    return res;
}


void Ensemble::frottement_paroi(float vx_paroi, float vy_paroi, float xlim_d, float xlim_g, float ylim_h, float ylim_b,float coeff_adherence){
    //d, g, h, b pour droite, gauche, haut, bas; droite signifie par exemple qu'on est à droite de la paroi

    //ne marchera pas pour un cercle, mais flemme pour le moment de rajouter une variable qui correspond 
    //à la normale donc tout sera droit pour le moment

    //(v-v_paroi).n = 0
    for(unsigned int i = 0; i<nombre_de_particules; i++){


    if ((data[i].x-rayon_influence)<xlim_d){
        data[i].vy += (vy_paroi-data[i].vy)*influence_paroi(data[i].x-xlim_d,rayon_influence,coeff_adherence);
    }
    if ((data[i].x+rayon_influence)>xlim_g){
        data[i].vy += (vy_paroi-data[i].vy)*influence_paroi(data[i].x-xlim_g,rayon_influence,coeff_adherence);
    }
    if ((data[i].y-rayon_influence)<ylim_h){
        data[i].vx += (vx_paroi-data[i].vx)*influence_paroi(data[i].y-ylim_h,rayon_influence,coeff_adherence);
    }
    if ((data[i].y+rayon_influence)>ylim_b){
        data[i].vx += (vx_paroi-data[i].vx)*influence_paroi(data[i].y-ylim_b,rayon_influence,coeff_adherence);
    }
    }
}


float influence_paroi(float dst,float rayon_influence, float coeff_adherence){
    float f=coeff_adherence*(1-pow(dst/rayon_influence,2));// A normaliser pour que la force totale ne dépende que du coeff(comme la densite)
    return(f);
}


/**
*actualise la liste d'indice contenant les clé et indice de chaque particules
*/
void Ensemble::liste_indice(int** liste) {
    for (unsigned int i=0; i<nombre_de_particules;i++){
        liste[i] = (int*)malloc(2*sizeof(int));
        liste[i][0] = (int)i;
        liste[i][1]= data[i].cle(rayon_influence,nombre_de_particules);
    }
}

/**
*fonction prenant deux tableau a 2 element, et renoyant -1,0 ou 1 si le second élement de a est plus petit, egal ou grand que celui de b
*/
int compare(const void* a,const void* b){
    if( (*(int**)a)[1] < (*(int**)b)[1]  ) {return(-1);}
    if( (*(int**)a)[1] == (*(int**)b)[1]  ) {return(0);}
    if( (*(int**)a)[1] > (*(int**)b)[1]  ) {return(1);}
    return(0);
}

/**
*Trie la liste en fonction de la clé. Utile pour ne regarder que les particules proches.
*/
void Ensemble::tri_liste_indice( int** liste ){
    qsort(liste,nombre_de_particules,sizeof(int*),compare);
}

/**
*donne le premier indice auquel trouver les particules de clé i 
*/
void Ensemble::liste_indice_debut(int* debut_indice,int** liste){
    for (unsigned int i=0;i<nombre_de_particules;i++){debut_indice[i]=-1;}
    int temp =-1;
    for(unsigned int i=0;i<nombre_de_particules;i++){
        if (temp != liste[i][1]) {
            temp = liste[i][1];
            debut_indice[temp]=i;      
        }
    }
}


void Ensemble::force_souris(){
    // Ajoute les différentes interaction entre l'utilisateur et le fluide, 960 est la taille de la fenètre d'affichage(carré).
    // La position du curseur peut être faussé si l'écran d'affichage est trop petit (<960*960)
    if (clique_gauche && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        float x = 2*sourisx/960.0 -1;
        float y = -sourisy*2/960.0 +1;
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            float dst = sqrt(pow((data[i].x-x),2)+pow((data[i].y)-y,2));
            if (dst==0){
                dst+=1;
            }
            if (dst<rayon_action_clique_gauche){  
                float infl = fonction_influence(dst,rayon_action_clique_gauche);
                data[i].vx+=dt*infl*puissance_action_clique_gauche*((data[i].x-x)/dst);
                data[i].vy+=dt*infl*puissance_action_clique_gauche*((data[i].y-y)/dst);
            }
        }
    }
    if (a_key && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        float x = 2*sourisx/960.0 -1;
        float y = -sourisy*2/960.0 +1;
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            float dst = sqrt(pow((data[i].x-x),2)+pow((data[i].y)-y,2));
            if (dst==0){
                dst+=1;
            }
            if (dst<rayon_action_clique_gauche){  
                float infl = fonction_influence(dst,rayon_action_clique_gauche);
                data[i].vx+=-dt*infl*abs(puissance_action_clique_gauche)*((data[i].x-x)/dst);
                data[i].vy+=-dt*infl*abs(puissance_action_clique_gauche)*((data[i].y-y)/dst);
            }
        }
    }
    if (e_key && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        float x = 2*sourisx/960.0 -1;
        float y = -sourisy*2/960.0 +1;
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            float dst = sqrt(pow((data[i].x-x),2)+pow((data[i].y)-y,2));
            if (dst==0){
                dst+=1;
            }
            if (dst<rayon_action_clique_gauche){  
                float infl = fonction_influence(dst,rayon_action_clique_gauche);
                data[i].vx+=dt*infl*abs(puissance_action_clique_gauche)*((data[i].x-x)/dst);
                data[i].vy+=dt*infl*abs(puissance_action_clique_gauche)*((data[i].y-y)/dst);
            }
        }
    }
    if (clique_droit && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx=0;
            data[i].vy=0;
        }    
    }
    if(z_key){
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vy+=dt*10;
        }
    }
    if(d_key){
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx+=dt*10;
        }
    }
    if(q_key){
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx-=dt*10;
        }
    }
    if(s_key){
        ////#pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vy-=dt*10;
        }
    }
}


void Ensemble::rempli_listes(Ensemble* f,int* coord, int* liste_cellules,int* liste_cellules2){
    int* coord_temporaire = (int*)malloc(2*sizeof(int));
    for(int i=0;i<9;i++){
        coord_temporaire[0]=coord[0]-1+i%3;
        coord_temporaire[1]=coord[1]-1+i/3;
        liste_cellules[i] = cle(coord_temporaire,nombre_de_particules);
        if ((*f).nombre_de_particules>0){
            liste_cellules2[i] = cle(coord_temporaire,(*f).nombre_de_particules);
        } 
    }
    free(coord_temporaire);
}

 float Ensemble::densite_ponctuelle_visee(float ex, float ey,Ensemble* f){
    float d=0; // densité
    int* coord = (int*)malloc(2*sizeof(int));
    coordonnee(coord,ex,ey,rayon_influence);            
    int* liste_cellules = (int*)malloc(9*sizeof(int));
    int* liste_cellules2 = (int*)malloc(9*sizeof(int));
    rempli_listes(f,coord,liste_cellules,liste_cellules2);
    float a = 0; //valeur de l'aire qui est en dehors de la boîte
    // côté gauche
    if ((ex-rayon_influence)<-1){
        float diff = 1+ex;
        a = aire(rayon_influence, diff);
        }
     // côté droit
    if ((ex+rayon_influence)>1){
        float diff = 1-ex;
        a = aire(rayon_influence, diff);
        }
     // bas
    if ((ey-rayon_influence)<-1){
        float diff = 1+ey;
        a = aire(rayon_influence, diff);
        }
     // haut
    if ((ey+rayon_influence)>1){
        float diff = 1-ey;
        a = aire(rayon_influence, diff);
        }
     // coin bas gauche
    if(((ex-rayon_influence)<-1) && ((ey-rayon_influence)<-1)){
        float diff_x = 1+ex;
        float diff_y = 1+ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
    }
     // coin haut gauche
    if(((ex-rayon_influence)<-1) && ((ey+rayon_influence)>1)){
        float diff_x = 1+ex;
        float diff_y = 1-ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
    }
     // coin bas droit
    if(((ex+rayon_influence)>1) && ((ey-rayon_influence)<-1)){
        float diff_x = 1-ex;
        float diff_y = 1+ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
    }
     // coin haut droit
    if(((ex+rayon_influence)>1) && ((ey+rayon_influence)>1)){
        float diff_x = 1-ex;
        float diff_y = 1-ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
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
    if ((*f).nombre_de_particules>0){

    
    for(unsigned int i=0;i<9;i++){

            int k=(*f).indice_debut[liste_cellules2[i]];
            if (k>=0) {

                while((*f).indices[k][1]==(*f).indices[(*f).indice_debut[liste_cellules2[i]]][1]){

                    d+=(*f).masse*(*f).data[(*f).indices[k][0]].influence(ex,ey,(*f).rayon_influence);
                    k++;

                    if ((unsigned int)k==(*f).nombre_de_particules){
                        break;
                    }
                }
            }
        }
    }
    d+= a*densite_visee/(M_PI*rayon_influence*rayon_influence); //metttre cette ligne en commentaire pour avoir la version sans prise en compte de la densité des paroies de la fonction

    free(coord);free(liste_cellules);free(liste_cellules2);
    return(d);
}

void Ensemble::densite(Ensemble* f,float* dens){
    if (nombre_de_particules>2000){
        
        #pragma omp parallel for // à décommenter pour les simulations avec N >~2000
        for(unsigned int i=0;i<nombre_de_particules;i++){
            
            dens[i]=densite_ponctuelle_visee(data[i].x_predit,data[i].y_predit,f);

        }
    }
    else {
        for(unsigned int i=0;i<nombre_de_particules;i++){
            dens[i]=densite_ponctuelle_visee(data[i].x_predit,data[i].y_predit,f);
        
        }
    }
}

void interaction(Ensemble* l1,Ensemble* l2,float pression_melange,float pression_proche_melange,float densite_visee_melange,float viscosite_melange){

    for(unsigned int i=0; i<(*l1).nombre_de_particules;i++){
        (*l1).data[i].x_predit = (*l1).data[i].x;//+(*l1).data[i].vx*pow(10,-5);
        (*l1).data[i].y_predit = (*l1).data[i].y;//+(*l1).data[i].vy*pow(10,-5);
    }
    for(unsigned int i=0; i<(*l2).nombre_de_particules;i++){
        (*l2).data[i].x_predit = (*l2).data[i].x;//+(*l2).data[i].vx*pow(10,-5);
        (*l2).data[i].y_predit = (*l2).data[i].y;//+(*l2).data[i].vy*pow(10,-5);
    }
    

    (*l1).actualise_listes();
    (*l2).actualise_listes();
    
    float* d1 = (float*)malloc((*l1).nombre_de_particules*sizeof(float));
    float* d2 = (float*)malloc((*l2).nombre_de_particules*sizeof(float));
    
    (*l1).densite(l2,d1);
    if ((*l2).nombre_de_particules>0){
        (*l2).densite(l1,d2);
    }
    
    (*l1).addforce2(d1,l2,d2,pression_melange,pression_proche_melange,densite_visee_melange,viscosite_melange);
    if ((*l2).nombre_de_particules>0){
        (*l2).addforce2(d2,l1,d1,pression_melange,pression_proche_melange,densite_visee_melange,viscosite_melange);
    }

    (*l1).deplacement();
    if ((*l2).nombre_de_particules>0){
        (*l2).deplacement();
    }
    
    free(d1);
    if ((*l2).nombre_de_particules>0){
        free(d2);
    }
}

void Ensemble::addforce2(float* d1,Ensemble* l2,float* d2,float pression_melange,float pression_proche_melange,float densite_visee_melange,float viscosite_melange){
    // Application des forces en parallèle
     #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                gravite();
            }
            #pragma omp section
            {
                force_pression(l2,d1,d2,pression_melange,densite_visee_melange);
            }
            #pragma omp section
            {
                force_pression_proche(l2,d1,d2,pression_proche_melange);
            }
            #pragma omp section
            {
                visc(l2,d1,viscosite_melange);
            }
            #pragma omp section
            {
                force_souris();
            }
        }
    }

}





void Ensemble::pression_ponctuelle(unsigned int n,  float* pression,Ensemble* l2, float* d1, float* d2,float pression_melange,float densite_visee_melange){
    pression[0]=0;//diréction x
    pression[1]=0;//diréction y
    int* coord = (int*)malloc(2*sizeof(int));
    coordonnee(coord,data[n].x,data[n].y,rayon_influence);           
    int* liste_cellules = (int*)malloc(9*sizeof(int));
    int* liste_cellules2 = (int*)malloc(9*sizeof(int));
    rempli_listes(l2,coord,liste_cellules,liste_cellules2);


    //pression de 1 avec 1
    for(unsigned int i=0;i<9;i++){
        int j=indice_debut[liste_cellules[i]];
        if (j>=0) {
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){
                unsigned int m=indices[j][0];
                if (m!=n) {
                    float distance = sqrt((data[n].x-data[m].x_predit)*(data[n].x-data[m].x_predit)+(data[n].y-data[m].y_predit)*(data[n].y-data[m].y_predit));
                    if (distance == 0) {
                        float d=0.001/RAND_MAX;
                        float x =(-0.5+std::rand())*d+0.0000001;
                        float y =(-0.5+std::rand())*d+0.0000001;
                        distance = sqrt(x*x+y*y); 
                        float dirx = x/distance;
                        float diry = y/distance;
                        float grad_influence = data[m].grad_influence(data[m].x+x,data[m].y+y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = ((-d1[m]+densite_visee)+(-d1[n]+densite_visee))/2.0*multiplicateur_pression;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/d1[m]);
                        pression[1]+=(pression_mutuelle*grad_influence*diry/d1[m]);
                        }
                    else {
                        float dirx = (data[n].x-data[m].x_predit)/distance;
                        float diry = (data[n].y-data[m].y_predit)/distance;
                        float grad_influence = data[m].grad_influence(data[n].x,data[n].y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = ((-d1[m]+densite_visee)+(-d1[n]+densite_visee))/2.0*multiplicateur_pression;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/d1[m]);
                        pression[1]+=(pression_mutuelle*grad_influence*diry/d1[m]);
                    }
         
                }
                j++;
                if ((unsigned int)j==nombre_de_particules){
                    break;
                }
            }
        }
    } 
    //Pression de 1 avec 2 (*l2).
    
    if (((*l2).nombre_de_particules>0)){
        for(unsigned int i=0;i<9;i++){
            int j=(*l2).indice_debut[liste_cellules2[i]];
            if (j>=0) {
                while((*l2).indices[j][1]==(*l2).indices[(*l2).indice_debut[liste_cellules2[i]]][1]){
                    unsigned int m=(*l2).indices[j][0];
                    if (m!=n) {
                        float distance = sqrt((data[n].x-(*l2).data[m].x_predit)*(data[n].x-(*l2).data[m].x_predit)+(data[n].y-(*l2).data[m].y_predit)*(data[n].y-(*l2).data[m].y_predit));
                        if (distance == 0) {
                            float d=0.001/RAND_MAX;
                            float x =(-0.5+std::rand())*d+0.0000001;
                            float y =(-0.5+std::rand())*d+0.0000001;
                            distance = sqrt(x*x+y*y); 
                            float dirx = x/distance;
                            float diry = y/distance;
                            float grad_influence = (*l2).data[m].grad_influence(data[n].x+x,data[n].y+y,rayon_influence);
                            float pression_mutuelle;
                            pression_mutuelle = ((densite_visee_melange-d2[m])+(densite_visee_melange-d1[n]))/2.0*pression_melange; 
                            pression[0]+=(pression_mutuelle*grad_influence*dirx/d2[m]);
                            pression[1]+=(pression_mutuelle*grad_influence*diry/d2[m]);
                            }
                        else {
                            float dirx = (data[n].x-(*l2).data[m].x_predit)/distance;
                            float diry = (data[n].y-(*l2).data[m].y_predit)/distance;
                            float grad_influence = (*l2).data[m].grad_influence(data[n].x,data[n].y,rayon_influence);
                            float pression_mutuelle;
                            pression_mutuelle = ((densite_visee_melange-d2[m])+(densite_visee_melange-d1[n]))/2.0*pression_melange;
                            pression[0]+=(pression_mutuelle*grad_influence*dirx/d2[m]);
                            pression[1]+=(pression_mutuelle*grad_influence*diry/d2[m]);
                        }

                    }
                    j++;
                    if ((unsigned int)j==(*l2).nombre_de_particules){
                        break;
                    }
                }
            }
        }
    }
    
    free(coord);free(liste_cellules);free(liste_cellules2);
}

void Ensemble::force_pression(Ensemble* l2, float* d1, float* d2,float pression_melange,float densite_visee_melange){
    float* pression = (float*)malloc(2*sizeof(float));
    for(unsigned int i=0; i<nombre_de_particules;i++){
        pression_ponctuelle(i,pression,l2,d1,d2, pression_melange, densite_visee_melange);
        data[i].vx+=dt*pression[0]/d1[i];
        data[i].vy+=dt*pression[1]/d1[i];
    }
    free(pression);
    

}






void Ensemble::pression_ponctuelle_proche(unsigned int n,  float* pression,Ensemble* l2, float* d1, float* d2,float pression_proche_melange){
    pression[0]=0;
    pression[1]=0;
    int* coord = (int*)malloc(2*sizeof(int));
    coordonnee(coord,data[n].x,data[n].y,rayon_influence);            
    int* liste_cellules = (int*)malloc(9*sizeof(int));
    int* liste_cellules2 = (int*)malloc(9*sizeof(int));
    rempli_listes(l2,coord,liste_cellules,liste_cellules2);
    

 //pression de 1 avec 1
    for(unsigned int i=0;i<9;i++){
        int j=indice_debut[liste_cellules[i]];
        if (j>=0) {
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){
                unsigned int m=indices[j][0];
                if (m!=n) {
                    float distance = sqrt((data[n].x-data[m].x_predit)*(data[n].x-data[m].x_predit)+(data[n].y-data[m].y_predit)*(data[n].y-data[m].y_predit));
                    if (distance == 0) {
                        float d=0.001/RAND_MAX;
                        float x =(-0.5+std::rand())*d+0.0000001;
                        float y =(-0.5+std::rand())*d+0.0000001;
                        distance = sqrt(x*x+y*y); 
                        float dirx = x/distance;
                        float diry = y/distance;
                        float grad_influence = data[m].grad_influence_proche(data[m].x+x,data[m].y+y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = -((d1[m])+(d1[n]))/2.0*multiplicateur_pression_proche;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/d1[m]);// densit[m] ou densite[n] ??
                        pression[1]+=(pression_mutuelle*grad_influence*diry/d1[m]);
                        }
                    else {
                        float dirx = (data[n].x-data[m].x_predit)/distance;
                        float diry = (data[n].y-data[m].y_predit)/distance;
                        float grad_influence = data[m].grad_influence_proche(data[n].x,data[n].y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = -((d1[m])+(d1[n]))/2.0*multiplicateur_pression_proche;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/d1[m]);// densit[m] ou densite[n] ??
                        pression[1]+=(pression_mutuelle*grad_influence*diry/d1[m]);
                    }
         
                }
                j++;
                if ((unsigned int)j==nombre_de_particules){
                    break;
                }
            }
        }
    }
    if ((*l2).nombre_de_particules>0){
        for(unsigned int i=0;i<9;i++){
            int j=(*l2).indice_debut[liste_cellules2[i]];
            if (j>=0) {
                while((*l2).indices[j][1]==(*l2).indices[(*l2).indice_debut[liste_cellules2[i]]][1]){
                    unsigned int m=(*l2).indices[j][0];
                    if (m!=n) {
                        float distance = sqrt((data[n].x-(*l2).data[m].x_predit)*(data[n].x-(*l2).data[m].x_predit)+(data[n].y-(*l2).data[m].y_predit)*(data[n].y-(*l2).data[m].y_predit));
                        if (distance == 0) {
                            float d=0.001/RAND_MAX;
                            float x =(-0.5+std::rand())*d+0.0000001;
                            float y =(-0.5+std::rand())*d+0.0000001;
                            distance = sqrt(x*x+y*y); 
                            float dirx = x/distance;
                            float diry = y/distance;
                            float grad_influence = (*l2).data[m].grad_influence_proche(data[n].x+x,data[n].y+y,rayon_influence);
                            float pression_mutuelle;
                            pression_mutuelle = ((-d2[m])+(-d1[n]))/2.0*pression_proche_melange; // changer la constante par un multiplicateur de pression d'interaction ?
                            pression[0]+=(pression_mutuelle*grad_influence*dirx/d2[m]);
                            pression[1]+=(pression_mutuelle*grad_influence*diry/d2[m]);
                            }
                        else {
                            float dirx = (data[n].x-(*l2).data[m].x_predit)/distance;
                            float diry = (data[n].y-(*l2).data[m].y_predit)/distance;
                            float grad_influence = (*l2).data[m].grad_influence_proche(data[n].x,data[n].y,rayon_influence);
                            float pression_mutuelle;
                            pression_mutuelle = ((-d2[m])+(-d1[n]))/2.0*pression_proche_melange;
                            pression[0]+=(pression_mutuelle*grad_influence*dirx/d2[m]);
                            pression[1]+=(pression_mutuelle*grad_influence*diry/d2[m]);
                        }

                    }
                    j++;
                    if ((unsigned int)j==(*l2).nombre_de_particules){
                        break;
                    }
                }
            }
        }
    }
     //Pression de 1 avec 2 (*l2).
    
    free(coord);free(liste_cellules);free(liste_cellules2);
}

void Ensemble::force_pression_proche(Ensemble* l2, float* d1, float* d2,float pression_proche_melange){
    float* pression = (float*)malloc(2*sizeof(float));
    for(unsigned int i=0; i<nombre_de_particules;i++){
        pression_ponctuelle_proche(i,pression,l2,d1,d2,pression_proche_melange);
        data[i].vx+=dt*pression[0]/d1[i];
        data[i].vy+=dt*pression[1]/d1[i];
    }
    free(pression);
    

}

void Ensemble::visc_ponctuelle(unsigned int n,float* visc,Ensemble* l2,float viscosite_melange){
    visc[0]=0;// diréction x
    visc[1]=0;// direction y
    int* coord = (int*)malloc(2*sizeof(int));
    coordonnee(coord,data[n].x,data[n].y,rayon_influence);           
    int* liste_cellules = (int*)malloc(9*sizeof(int));
    int* liste_cellules2 = (int*)malloc(9*sizeof(int));
    rempli_listes(l2,coord,liste_cellules,liste_cellules2);
    
    for(unsigned int i=0;i<9;i++){
        int j=indice_debut[liste_cellules[i]];
        if (j>=0) {
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){   
                unsigned int m=indices[j][0];
                if (m!=n) {
                    float infl = data[m].influence(data[n].x,data[n].y,rayon_influence);
                    visc[0] += (data[m].vx-data[n].vx)*infl*coeff_viscosite/(rayon_influence*rayon_influence);
                    visc[1] += (data[m].vy-data[n].vy)*infl*coeff_viscosite/(rayon_influence*rayon_influence);
                    }
                j++;
                if ((unsigned int)j==nombre_de_particules){
                    break;
                }
            }
        }
    }
    // int avec fluide 2
    if((*l2).nombre_de_particules>0){
        for(unsigned int i=0;i<9;i++){
            int j=(*l2).indice_debut[liste_cellules2[i]];
            if (j>=0) {
                while((*l2).indices[j][1]==(*l2).indices[(*l2).indice_debut[liste_cellules2[i]]][1]){
                    unsigned int m=(*l2).indices[j][0];
                    if (m!=n) {
                        float infl = (*l2).data[m].influence(data[n].x,data[n].y,rayon_influence);
                        visc[0] += ((*l2).data[m].vx-data[n].vx)*infl*viscosite_melange/(rayon_influence*rayon_influence);
                        visc[1] += ((*l2).data[m].vy-data[n].vy)*infl*viscosite_melange/(rayon_influence*rayon_influence);
                    }
                    j++;
                    if ((unsigned int)j==(*l2).nombre_de_particules){
                        break;
                    }
                }
            }
        }
    }

    

    free(coord);free(liste_cellules);free(liste_cellules2);
}


void Ensemble::visc(Ensemble* l2, float* d1,float viscosite_melange){
    float* viscosite = (float*)malloc(2*sizeof(float));
    for(unsigned int i=0; i<nombre_de_particules;i++){
        visc_ponctuelle(i,viscosite,l2,viscosite_melange);
        data[i].vx += dt*viscosite[0]/d1[i];
        data[i].vy += dt*viscosite[1]/d1[i];
    }
    free(viscosite);
}



//Par point : xpoint,ypoint,xparticule,yparticule,R,G,B,A,TexCoordx,TexCoordy,TexID,rayon_collision²,rayon_influence²

/**
 * ip,vitesse_caracteristique,opacite,TextureID_debut,TextureID_fin
*/
void Ensemble::rempli_info_point(float* ip,float vc,float opacite,bool affiche_vitesses_colorees, float TexID_d,float TexID_f){
    float* couleur=(float*)malloc(4*sizeof(float));
    for(unsigned int i=0;i<nombre_de_particules;i++){
        Particule p = data[i];
        p.couleur(couleur,vc,opacite,affiche_vitesses_colorees);
        float r = rayon_collision;
        if (TexID_d>0.5){r=rayon_influence;};
        unsigned int index=13*4*i;
        //positions des 4 points
        ip[index]=p.x-r;
        ip[index+1]=p.y-r;
        ip[index+13]=p.x-r;
        ip[index+13+1]=p.y+r;
        ip[index+26]=p.x+r;
        ip[index+26+1]=p.y-r;
        ip[index+39]=p.x+r;
        ip[index+39+1]=p.y+r;
        // TexCoord
        ip[index+8]=0;
        ip[index+9]=0;
        ip[index+13+8]=0;
        ip[index+13+9]=1;
        ip[index+26+8]=1;
        ip[index+26+9]=0;
        ip[index+39+8]=1;
        ip[index+39+9]=1;
        // Reste des info, identiques pour les 4 points
        for (unsigned j=0;j<4;j++){
            //pisition de la particule
            ip[index+j*13+2]=p.x;
            ip[index+j*13+3]=p.y;
            //RGBA
            ip[index+j*13+4]=couleur[0];
            ip[index+j*13+5]=couleur[1];
            ip[index+j*13+6]=couleur[2];
            ip[index+j*13+7]=couleur[3];
            //Texture
            ip[index+j*13+10]=i%(int)(TexID_f-TexID_d+1)+TexID_d;
            //rayon
            ip[index+j*13+11]=rayon_collision*rayon_collision/4;
            ip[index+j*13+12]=rayon_influence*rayon_influence/4;

        }

    }free(couleur);
}
