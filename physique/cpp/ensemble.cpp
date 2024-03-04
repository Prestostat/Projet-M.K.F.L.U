#include "../header/ensemble.h"
#include "../header/particule.h"
#include <math.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define E 2.71828182846
#define retrun return //permet d'écrir retrun a la place de return :sunglases:
using namespace std;

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
        //data[i].initialise_particules(-1+1.0/N+(i%N)*2.0/N,-1+1.0/N+(i/N)*2.0/N,0,0,rayon);   //place les particles dans un carré remplissant l'espace de l'écran
        data[i].initialise_particules(-0.5+0.5/N+(i%N)*1.0/N,-0.5+0.5/N+(i/N)*1.0/N,0,0,rayon); // CARRÉ
        //data[i].random_initialise_particules(rayon); // RANDOM                                                                                     //(de (-1,-1) à (1,1) dépendant de leur nombre
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

void Ensemble::actualise_constantes(float rayon_affichage_,float g_,float masse_,float multiplicateur_pression_,float multiplicateur_pression_proche_,float densite_visee_,float dt_,float rayon_influence_,float coeff_amorti_,float viscstrength_,float sourisx_,float sourisy_,float rayon_action_clique_gauche_,float puissance_action_clique_gauche_,bool clique_gauche_,bool clique_droit_,bool a,bool z,bool e,bool q,bool s, bool d, bool pause_){
    rayon_affichage = rayon_affichage_;
    g = g_;
    masse =masse_;
    multiplicateur_pression = multiplicateur_pression_;
    multiplicateur_pression_proche = multiplicateur_pression_proche_;
    densite_visee = densite_visee_;
    dt =dt_;
    rayon_influence =rayon_influence_;
    coeff_amorti = coeff_amorti_;
    viscstrength = viscstrength_;
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
void Ensemble::addforce(float* de){
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
                force_pression(de);
            }
            #pragma omp section
            {
                force_pression_proche(de);
            }
            #pragma omp section
            {
                visc(de);
            }
            #pragma omp section
            {
                force_souris();
            }
        }
    }

}
void Ensemble::gravite() {
    #pragma omp parallel for
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].vy-=g*dt;
    }
}


void Ensemble::deplacement() {
    #pragma omp parallel for
    for (unsigned int i=0; i<nombre_de_particules;i++) {
        data[i].x+=data[i].vx*dt;
        data[i].y+=data[i].vy*dt;
        data[i].collision(coeff_amorti);
    }
}

void Ensemble::actualise_listes(){
    indices = liste_indice ();
    tri_liste_indice(indices);
    indice_debut = liste_indice_debut(indices);

}

void Ensemble::evolution(){
    
    
    if (! pause){
        #pragma omp parallel for
        for(unsigned int i=0; i<nombre_de_particules;i++){
            data[i].x_predit = data[i].x+data[i].vx*pow(10,-5);
            data[i].y_predit = data[i].y+data[i].vy*pow(10,-5);
        }
        
        float* de = densite();
        addforce(de);
        deplacement();
        free(de);
        
    }
    
}


  float Ensemble::densite_ponctuelle_visee(float ex, float ey){
    float d=0;
    int* coord = coordonnee(ex,ey,rayon_influence);
    int* coord_temporaire = (int*)malloc(2*sizeof(int));            
    int* liste_cellules = (int*)malloc(9*sizeof(int));
    for(int i=0;i<9;i++){
        coord_temporaire[0]=coord[0]-1+i%3;
        coord_temporaire[1]=coord[1]-1+i/3;
        liste_cellules[i] = cle(coord_temporaire,nombre_de_particules);
    }

    float a = 0; //valeur de l'aire qui est en dehors de la boîte

    if ((ex-rayon_influence)<-1){
        float diff = 1+ex;
        a = aire(rayon_influence, diff);
        }
    if ((ex+rayon_influence)>1){
        float diff = 1-ex;
        a = aire(rayon_influence, diff);
        }
    if ((ey-rayon_influence)<-1){
        float diff = 1+ey;
        a = aire(rayon_influence, diff);
        }
    if ((ey+rayon_influence)>1){
        float diff = 1-ey;
        a = aire(rayon_influence, diff);
        }
    if(((ex-rayon_influence)<-1) && ((ey-rayon_influence)<-1)){
        float diff_x = 1+ex;
        float diff_y = 1+ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
    }
    if(((ex-rayon_influence)<-1) && ((ey+rayon_influence)>1)){
        float diff_x = 1+ex;
        float diff_y = 1-ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
    }
    if(((ex+rayon_influence)>1) && ((ey-rayon_influence)<-1)){
        float diff_x = 1-ex;
        float diff_y = 1+ey;
        a= aire(rayon_influence, diff_x);
        a+= aire(rayon_influence, diff_y);
        float y_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_x*diff_x) - diff_y, 0);
        float x_1 = maxi(sqrt(rayon_influence*rayon_influence - diff_y*diff_y) - diff_x, 0);
        a -= (aire_triangle(y_1, x_1) + pow((x_1+y_1)/2, 2)*(M_PI/4 - 0.5));
    }
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

    d+= a*densite_visee/(M_PI*rayon_influence*rayon_influence); //metttre cette ligne en commentaire pour avoir la version précédente de la fonction

    free(coord);free(coord_temporaire);free(liste_cellules);
    return(d);
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


float* Ensemble::densite(){
    
    float* dens=(float*)malloc(nombre_de_particules*sizeof(float));
    
    #pragma omp parallel for
    for(unsigned int i=0;i<nombre_de_particules;i++){
        dens[i]=densite_ponctuelle_visee(data[i].x_predit,data[i].y_predit);
        
    }
    return(dens);
}

void Ensemble::frottement_paroi(float vx_paroi, float vy_paroi, float xlim_d, float xlim_g, float ylim_h, float ylim_b,float coeff_adherence){
    //d, g, h, b pour droite, gauche, haut, bas; droite signifie par exemple qu'on est à droite de la paroi

    //ne marchera pas pour un cercle, mais flemme pour le moment de rajouter une variable qui correspond 
    //à la normale donc tout sera droit pour le moment

    //(v-v_paroi).n = 0
    for(unsigned int i = 0; i<nombre_de_particules; i++){

    if ((data[i].x-rayon_influence)<xlim_d){
        data[i].vy = vy_paroi+(data[i].vy-vy_paroi)*pow((data[i].x-xlim_d)/(rayon_influence),2);
    }
    if ((data[i].x+rayon_influence)>xlim_g){
        data[i].vy = vy_paroi+(data[i].vx-vy_paroi)*pow((data[i].x-xlim_g)/(rayon_influence),2);
    }
    if ((data[i].y-rayon_influence)<ylim_h){
        data[i].vx = vx_paroi+(data[i].vx-vx_paroi)*pow((data[i].y-ylim_h)/(rayon_influence),2);
    }
    if ((data[i].y+rayon_influence)>ylim_b){
        data[i].vx = vx_paroi+(data[i].vx-vx_paroi)*pow((data[i].y-ylim_b)/(rayon_influence),2);
    }
    //je ne vois pas pourquoi il y a besoin de refaire les angles (Luc)
    /*
    if(((data[i].x-rayon_influence)<xlim_d) && ((data[i].y-rayon_influence)<ylim_h)){
        data[i].vx = vx_paroi;
        data[i].vy = vy_paroi;
    }
    if(((data[i].x-rayon_influence)<xlim_d) && ((data[i].y+rayon_influence)>ylim_b)){
        data[i].vx = vx_paroi;
        data[i].vy = vy_paroi;
    }
    if(((data[i].x+rayon_influence)>xlim_g) && ((data[i].y-rayon_influence)<ylim_h)){
        data[i].vx = vx_paroi;
        data[i].vy = vy_paroi;
    }
    if(((data[i].x+rayon_influence)>xlim_g) && ((data[i].y+rayon_influence)>ylim_b)){
        data[i].vx = vx_paroi;
        data[i].vy = vy_paroi;
    }*/
    }
}

void Ensemble::pression_ponctuelle(unsigned int n,  float* densite,float* pression){
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
    #pragma omp parallel for
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
                        pression_mutuelle = ((-densite[m]+densite_visee)+(-densite[n]+densite_visee))/2.0*multiplicateur_pression;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/densite[m]);
                        pression[1]+=(pression_mutuelle*grad_influence*diry/densite[m]);
                        }
                    else {
                        float dirx = (data[n].x-data[m].x_predit)/distance;
                        float diry = (data[n].y-data[m].y_predit)/distance;
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


void Ensemble::force_pression(float* d){
    float* pression = (float*)malloc(2*sizeof(float));
    #pragma omp parallel for
    for(unsigned int i=0; i<nombre_de_particules;i++){
        pression_ponctuelle(i,d,pression);
        data[i].vx+=dt*pression[0]/d[i];
        data[i].vy+=dt*pression[1]/d[i];
    }
    free(pression);
    

}


void Ensemble::visc_ponctuelle(unsigned int n,float* visc){
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
    #pragma omp parallel for
    for(unsigned int i=0;i<9;i++){
        int j=indice_debut[liste_cellules[i]];
        if (j>=0) {
            while(indices[j][1]==indices[indice_debut[liste_cellules[i]]][1]){
                unsigned int m=indices[j][0];
                if (m!=n) {
                    float infl = data[m].influence(data[n].x,data[n].y,rayon_influence);
                    visc[0] += (data[m].vx-data[n].vx)*infl*viscstrength/(rayon_influence*rayon_influence);
                    visc[1] += (data[m].vy-data[n].vy)*infl*viscstrength/(rayon_influence*rayon_influence);
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

void Ensemble::visc(float* d){
    float* viscosite = (float*)malloc(2*sizeof(float));
    #pragma omp parallel for
    for(unsigned int i=0; i<nombre_de_particules;i++){
        visc_ponctuelle(i,viscosite);
        data[i].vx += dt*viscosite[0]/d[i];
        data[i].vy += dt*viscosite[1]/d[i];
    }
    free(viscosite);
}

void Ensemble::bord(){
    #pragma omp parallel for
    for(unsigned int m=0;m<nombre_de_particules;m++){
        if (data[m].x<-0.95){
            float dist1 = data[m].x + 1;
            data[m].vy = data[m].vy *(1-0.05*exp(-dist1*dist1));
            }
        if (data[m].x>0.95){
            float dist2 = 1 - data[m].x;
            data[m].vy = data[m].vy * (1-0.05*exp(-dist2*dist2));
            }
        if (data[m].y<-0.95){
            float dist3 = data[m].y + 1;
            data[m].vx = data[m].vx * (1-0.05*exp(-dist3*dist3));
            }
    }
}

int** Ensemble::liste_indice() {
    int** liste= (int**)malloc(nombre_de_particules*sizeof(int*));
    #pragma omp parallel for
    for (unsigned int i=0; i<nombre_de_particules;i++){
        liste[i] = (int*)malloc(2*sizeof(int));
        liste[i][0] = (int)i;
        liste[i][1]= data[i].cle(rayon_influence,nombre_de_particules);
    }
    return(liste);
}


int compare(const void* a,const void* b){
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


void Ensemble::force_souris(){
    if (clique_gauche && sourisx>0 && sourisy>0 && sourisx<960 && sourisy<960){
        float x = 2*sourisx/960.0 -1;
        float y = -sourisy*2/960.0 +1;
        #pragma omp parallel for
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
        #pragma omp parallel for
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
        #pragma omp parallel for
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
        #pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx=0;
            data[i].vy=0;
        }    
    }
    if(z_key){
        #pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vy+=dt*10;
        }
    }
    if(d_key){
        #pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx+=dt*10;
        }
    }
    if(q_key){
        #pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vx-=dt*10;
        }
    }
    if(s_key){
        #pragma omp parallel for
        for(unsigned int i=0;i<nombre_de_particules;i++){
            data[i].vy-=dt*10;
        }
    }
}


void Ensemble::pression_ponctuelle_proche(unsigned int n,  float* densite,float* pression){
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
    #pragma omp parallel for
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
                        pression_mutuelle = ((densite[m])+(densite[n]))/2.0*multiplicateur_pression_proche;
                        pression[0]+=(pression_mutuelle*grad_influence*dirx/densite[m]);
                        pression[1]+=(pression_mutuelle*grad_influence*diry/densite[m]);
                        }
                    else {
                        float dirx = (data[n].x-data[m].x_predit)/distance;
                        float diry = (data[n].y-data[m].y_predit)/distance;
                        float grad_influence = data[m].grad_influence_proche(data[n].x,data[n].y,rayon_influence);
                        float pression_mutuelle;
                        pression_mutuelle = ((densite[m])+(densite[n]))/2.0*multiplicateur_pression_proche;
                        pression[0]+=-(pression_mutuelle*grad_influence*dirx/densite[m]);
                        pression[1]+=-(pression_mutuelle*grad_influence*diry/densite[m]);
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


void Ensemble::force_pression_proche(float* d){
    float* pression = (float*)malloc(2*sizeof(float));
    #pragma omp parallel for
    for(unsigned int i=0; i<nombre_de_particules;i++){
        pression_ponctuelle_proche(i,d,pression);
        data[i].vx+=dt*pression[0]/d[i];
        data[i].vy+=dt*pression[1]/d[i];
    }
    free(pression);
}