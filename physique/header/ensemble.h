#ifndef ENSEMBLE_H
#define ENSEMBLE_H
#include "particule.h"
class Ensemble {
    private:
    public:
    unsigned int nombre_de_particules;
    Particule* data;
    int** indices;
    int* indice_debut;
    float masse;
    float densite_visee;
    float multiplicateur_pression;
    float multiplicateur_pression_proche;
    float coeff_amorti;
    Ensemble (unsigned int nb,float rayon);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite(float dt,float g);
    void deplacement(float dt);
    void evolution(float dt, float g,float rayon_influence, float m,float multipression,float multipression_proche,float dvisee,float coef,float viscstrength,bool clique_gauche,bool clique_droit,float sourisx,float sourisy,float rayon_action_clique_gauche, float puissance_action);
    float densite_ponctuelle(float ex, float ey, float rayon_influence);
    float densite_ponctuelle_naive(float ex, float ey, float rayon_influence);
    float* densite(float rayon_influence);
    void pression_ponctuelle(unsigned int n, float* densite,float rayon_influence,float* pression);
    void force_pression(float dt,float rayon_influence,float* d);
    void visc_ponctuelle(unsigned int n,float rayon_influence,float viscstrength, float* visc);
    void visc(float dt, float rayon_influence,float viscstrength,float* d);
    int** liste_indice(float rayon_influence);
    void tri_liste_indice( int** liste );
    int* liste_indice_debut(int**liste);
    void force_souris(float dt,bool clique_gauche,bool clique_droit, float sourisx,float sourisy,float rayon_action_clique_gauche, float puissance_action);
    void pression_ponctuelle_proche(unsigned int n, float* densite,float rayon_influence,float* pression);
    void force_pression_proche(float dt,float rayon_influence,float* d);
};
int compare(const void* a,const void* b);
#endif