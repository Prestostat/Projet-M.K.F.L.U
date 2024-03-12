#include "../header/obstacle.h"
#include "../header/particule.h"
#include <iostream>

void Obstacle::init_Obstacle(string nom_obstacle_, float masse_, float coeff_amorti_obstacle_, float xini_, float yini_, float vx_obs_, float vy_obs_){
    nom_obstacle = nom_obstacle_;
    masse = masse_;
    xini = xini_;
    yini = yini_;
    vx_obs = vx_obs_;
    vy_obs = vy_obs_;
    coeff_amorti_obstacle = coeff_amorti_obstacle_;
    if (nom_obstacle == "carre"){ // Centré en (xini,yini)

        float* x_obs = (float*)malloc(4*N*sizeof(float));
        float* y_obs = (float*)malloc(4*N*sizeof(float));

        float c = 0.2; // Taille du carré
        float eps = c/N;

        // Côté gauche
        for (unsigned int i; i<N; i++){
            y_obs[i] = -c/2 + eps*i + yini;
            x_obs[i] = -c/2 + xini;
        }

        // Bas
        for (unsigned int i; i<N; i++){
            x_obs[i+N] = -c/2 + eps*i + xini;
            y_obs[i+N] = -c/2 + yini;
        }

        // Côté droit
        for (unsigned int i; i<N; i++){
            y_obs[i+2*N] = -c/2 + eps*i + yini;
            x_obs[i+2*N] = c/2 + xini;
        }

        // Haut
        for (unsigned int i; i<N; i++){
            x_obs[i+3*N] = -c/2 + eps*i + xini;
            y_obs[i+3*N] = c/2 + yini;
        }
    }

}

void Obstacle::collision_obstacle(Particule* data,float masse_particule, unsigned int nombre_de_particules, bool deplacement_ON_OFF){
    if(nom_obstacle == "carré"){
        for (unsigned int i;i<nombre_de_particules; i++){
            if ((data[i].x-data[i].rayon<c/2+xini) && (data[i].x+data[i].rayon>-c/2+xini) && (data[i].y-data[i].rayon<c/2+yini) && (data[i].y+data[i].rayon>-c/2+yini)){
                
                if (data[i].x<-c/2 + xini){ //Côté Gauche
                    data[i].x=1-data[i].rayon-coeff_amorti_obstacle*(data[i].x+data[i].rayon-1);
                    data[i].vx*=-1*coeff_amorti_obstacle;

                    if (deplacement_ON_OFF){
                        vx_obs+=data[i].vx*masse_particule/masse;
                        data[i].vx-=data[i].vx*masse_particule/masse;
                    }
                }

                if (data[i].x>c/2 + xini){ //Côté Droit
                    data[i].x=-1+data[i].rayon+coeff_amorti_obstacle*(-data[i].x+data[i].rayon-1);
                    data[i].vx*=-1*coeff_amorti_obstacle;

                    if (deplacement_ON_OFF){
                        vx_obs+=data[i].vx*masse_particule/masse;
                        data[i].vx-=data[i].vx*masse_particule/masse;
                    }
                }

                if (data[i].y<-c/2 + yini){ //Dessous
                    data[i].y=-1+data[i].rayon+coeff_amorti_obstacle*(-data[i].y+data[i].rayon-1);
                    data[i].vy*=-1*coeff_amorti_obstacle;

                    if (deplacement_ON_OFF){
                        vy_obs+=data[i].vy*masse_particule/masse;
                        data[i].vy-=data[i].vy*masse_particule/masse;
                    }
                }

                if (data[i].y>c/2 + yini){ //Dessus
                    data[i].y=1-data[i].rayon+coeff_amorti_obstacle*(-data[i].y+data[i].rayon-1);
                    data[i].vy*=-1*coeff_amorti_obstacle;

                    if (deplacement_ON_OFF){
                        vx_obs+=data[i].vy*masse_particule/masse;
                        data[i].vy-=data[i].vy*masse_particule/masse;
                    }
                }
            }
            /*if(){ // à voir les conditions aux coins si ça marche ou pas

            }*/
        }
    }
}
