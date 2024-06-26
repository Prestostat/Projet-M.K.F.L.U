#ifndef PARTICULE_H
#define PARTICULE_H
#include "///usr/include/GL/glew.h"
#include <GLFW/glfw3.h>
class Particule {
    private:
    public:
    float x;
    float y;
    float vx;
    float vy;
    float x_predit;
    float y_predit;
    float rayon;

    void initialise_particules(float xini, float yini,float vxini,float vyini,float rayonini);
    void random_initialise_particules(float rayonini);
    void inirapide();
    void position_particule(unsigned int nb_points,float rayon_influence,float* position);
    void collision(float ammorti);
    float influence(float ex,float ey,float rayon_influence);
    float grad_influence(float ex, float ey, float rayon_influence);
    float grad_influence_proche(float ex, float ey, float rayon_influence);
    void coordonnee(int* coord, float rayon_influence);
    int cle(float rayon_influence, int nombre_de_particules);
    void couleur(float* couleur,float vitesse_caracteristique,float opacite,bool affiche_vitesses_colorees);
    void position_particule_carre(float* position);


};
float fonction_influence(float x,float rayon_influence);
float derive_fonction_influence(float x, float rayon_influence);
float derive_fonction_influence_proche(float x, float rayon_influence);
void coordonnee(int* coord,float ex, float ey,float rayon_influence);
int cle(int* coord, int nombre_de_particules);
#endif