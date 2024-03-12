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
}


void Obstacle::collision_obstacle(Particule* data,float masse_particule, unsigned int m, bool deplacement_ON_OFF,float dt){
    // APPROX PARTICULE = "petit carré"
    if(nom_obstacle == "carré"){
        //Côté Gauche
        if ((data[m].x < xini - c/2) && (data[m].vx>0)){
            if ((data[m].y - data[m].rayon < yini + c/2) && (data[m].y + data[m].rayon > yini - c/2) && (data[m].x + data[m].rayon > xini - c/2)){
                data[m].x = xini - c/2 - data[m].rayon;
                data[m].vx*=-1*coeff_amorti_obstacle;

                if (deplacement_ON_OFF){
                    // on pourrai améliorer en calculant l'angle de collision
                    // terme en (1-coeff_amorti_obstacle)*data[m].vx ou (1-coeff_amorti_obstacle)*data[m].vx*masse_particule/masse pour conservation d'énergie ça serait pas mal ? à moins que 1 - coeff_amorti_obstacle = masse_particule/masse 
                    vx_obs += data[m].vx*masse_particule/masse;
                    data[m].vx -= data[m].vx*masse_particule/masse;
                }
            }

            else if (((data[m].y + data[m].vy*dt) - data[m].rayon < yini + c/2) && ((data[m].y + data[m].vy*dt) + data[m].rayon > yini - c/2) && ((data[m].x + data[m].vx*dt) + data[m].rayon > xini - c/2)){
                    data[m].y = data[m].y + data[m].vy*dt + data[m].vy*(xini - c/2 - data[m].x - data[m].vx*dt)/data[m].vx;
                    data[m].x = xini - c/2 - data[m].rayon;
                    data[m].vx*=-1*coeff_amorti_obstacle;

                    if (deplacement_ON_OFF){
                        // on pourrai améliorer en calculant l'angle de collision
                        // terme en (1-coeff_amorti_obstacle)*data[m].vx ou (1-coeff_amorti_obstacle)*data[m].vx*masse_particule/masse pour conservation d'énergie ça serait pas mal ? à moins que 1 - coeff_amorti_obstacle = masse_particule/masse 
                        vx_obs += data[m].vx*masse_particule/masse;
                        data[m].vx -= data[m].vx*masse_particule/masse;
                    }
            }
        }

        //Côté Droit
        if ((data[m].x > xini + c/2) && (data[m].vx<0)){
            if ((data[m].x - data[m].rayon < xini + c/2) && (data[m].y - data[m].rayon < yini + c/2) && (data[m].y + data[m].rayon > yini - c/2)){
                data[m].x = xini + c/2 + data[m].rayon;
                data[m].vx *= -1*coeff_amorti_obstacle;

                if (deplacement_ON_OFF){
                    vx_obs+=data[m].vx*masse_particule/masse;
                    data[m].vx-=data[m].vx*masse_particule/masse;
                }
            }
            else if (((data[m].x + data[m].vx*dt) - data[m].rayon < xini + c/2) && ((data[m].y + data[m].vy*dt) - data[m].rayon < yini + c/2) && ((data[m].y + data[m].vy*dt) + data[m].rayon > yini - c/2)){
                    data[m].y = data[m].y + data[m].vy*dt + data[m].vy*(xini + c/2 - data[m].x - data[m].vx*dt)/data[m].vx;
                    data[m].x = xini + c/2 + data[m].rayon;
                    data[m].vx*=-1*coeff_amorti_obstacle;

                    if (deplacement_ON_OFF){
                        vx_obs+=data[m].vx*masse_particule/masse;
                        data[m].vx-=data[m].vx*masse_particule/masse;
                    }
            }
        }
            
        //Dessous
        if((data[m].y < yini - c/2) && (data[m].vy>0)){
            if ((data[m].y + data[m].rayon > yini - c/2) && (data[m].x - data[m].rayon < xini + c/2) && (data[m].x + data[m].rayon > xini - c/2)){
                data[m].y = yini - c/2 - data[m].rayon;
                data[m].vy *= -1*coeff_amorti_obstacle;

                if (deplacement_ON_OFF){
                    vy_obs+=data[m].vy*masse_particule/masse;
                    data[m].vy-=data[m].vy*masse_particule/masse;
                }
            }
            else if (((data[m].y + data[m].vy*dt) + data[m].rayon > yini - c/2) && ((data[m].x + data[m].vx*dt) - data[m].rayon < xini + c/2) && ((data[m].x + data[m].vx*dt) + data[m].rayon > xini - c/2)){
                data[m].x = data[m].x + data[m].vx*dt + data[m].vx*(yini - c/2 - data[m].y - data[m].vy*dt)/data[m].vy;
                data[m].y = yini - c/2 - data[m].rayon;
                data[m].vy *= -1*coeff_amorti_obstacle;

                if (deplacement_ON_OFF){
                    vy_obs+=data[m].vy*masse_particule/masse;
                    data[m].vy-=data[m].vy*masse_particule/masse;
                }
            }

        }

        //Dessus
        if((data[m].y > yini + c/2) && (data[m].vy<0)){
            if ((data[m].y - data[m].rayon < yini + c/2) && (data[m].x - data[m].rayon < xini + c/2) && (data[m].x + data[m].rayon > xini - c/2)){
                data[m].y = yini + c/2 + data[m].rayon;
                data[m].vy *= -1*coeff_amorti_obstacle;

                if (deplacement_ON_OFF){
                    vx_obs+=data[m].vy*masse_particule/masse;
                    data[m].vy-=data[m].vy*masse_particule/masse;
                }
            }
            else if (((data[m].y + data[m].vy*dt) - data[m].rayon < yini + c/2) && ((data[m].x + data[m].vx*dt) - data[m].rayon < xini + c/2) && ((data[m].x + data[m].vx*dt) + data[m].rayon > xini - c/2)){
                data[m].x = data[m].x + data[m].vx*dt + data[m].vx*(yini + c/2 - data[m].y - data[m].vy*dt)/data[m].vy;
                data[m].y = yini + c/2 + data[m].rayon;
                data[m].vy *= -1*coeff_amorti_obstacle;

                if (deplacement_ON_OFF){
                    vx_obs+=data[m].vy*masse_particule/masse;
                    data[m].vy-=data[m].vy*masse_particule/masse;
                }
            }
        }
        if (deplacement_ON_OFF){
            yini += dt*vy_obs;
            xini += dt*vx_obs;
        }
    }
}

void Obstacle::position_obstacle(float* position){
    if ((string)"carré"==nom_obstacle){
        position[0]=xini-c/2;
        position[1]=yini-c/2;
        position[2]=xini+c/2;
        position[3]=yini-c/2;
        position[4]=xini-c/2;
        position[5]=yini+c/2;
        position[6]=xini+c/2;
        position[7]=yini+c/2;
    }
}