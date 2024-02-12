#ifndef ENSEMBLE_H
#define ENSEMBLE_H
#include "particule.h"
class Ensemble {
    private:
    public:
    unsigned int nombre_de_particules;
    Particule* data;
    float masse;
    float densite_visee;
    float multiplicateur_pression;
    Ensemble (unsigned int nb,float rayon);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite(float dt,float g);
    void deplacement(float dt);
    void evolution(float dt,float g,float rayon_influence);
    float densite_ponctuelle(float ex, float ey, float rayon_influence,int** indice_debut, int* indices);
    float densite_ponctuelle_naive(float ex, float ey, float rayon_influence);
    float* densite(float rayon_influence);
    float* pression_ponctuelle(unsigned int n, float* densite,float rayon_influence);
    void force_pression(float dt,float rayon_influence);
    int** liste_indice(float rayon_influence);
    void tri_liste_indice( int** liste );
    int* liste_indice_debut(int**liste);

};
int compare(const void* a,const void* b);
#endif