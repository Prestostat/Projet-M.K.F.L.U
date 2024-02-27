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
    float rayon_affichage ;
    float g ;
    float masse = 1;
    float multiplicateur_pression ;
    float multiplicateur_pression_proche ;
    float densite_visee ;
    float dt ;
    float rayon_influence ;
    float coeff_amorti;
    float viscstrength ;
    float sourisx;
    float sourisy;
    float rayon_action_clique_gauche ;
    float puissance_action_clique_gauche ;

    bool clique_gauche;
    bool clique_droit;

    Ensemble (unsigned int nb,float rayon_affichage);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite();
    void deplacement();
    void evolution(float rayon_affichage,float g,float masse,float multiplicateur_pression,float multiplicateur_pression_proche,float densite_visee,float dt,float rayon_influence,float coeff_amorti,float viscstrength,float sourisx,float sourisy,float rayon_action_clique_gauche,float puissance_action_clique_gauche,float clique_gauche,float clique_droit);
    float densite_ponctuelle(float ex, float ey);
    float* densite();
    void pression_ponctuelle(unsigned int n, float* densite,float* pression);
    void force_pression(float* d);
    void visc_ponctuelle(unsigned int n, float* visc);
    void visc(float* d);
    int** liste_indice();
    void tri_liste_indice( int** liste );
    int* liste_indice_debut(int**liste);
    void force_souris();
    void pression_ponctuelle_proche(unsigned int n,  float* densite,float* pression);
    void force_pression_proche(float* d);
};
int compare(const void* a,const void* b);
#endif