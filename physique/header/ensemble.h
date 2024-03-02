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
    bool a_key;
    bool z_key;
    bool e_key;
    bool q_key;
    bool s_key;
    bool d_key;
    bool pause;


    Ensemble (unsigned int nb,float rayon_affichage);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite();
    void deplacement();
    void evolution(float rayon_affichage,float g,float masse,float multiplicateur_pression,float multiplicateur_pression_proche,float densite_visee,float dt,float rayon_influence,float coeff_amorti,float viscstrength,float sourisx,float sourisy,float rayon_action_clique_gauche,float puissance_action_clique_gauche,bool clique_gauche_,bool clique_droit_,bool a,bool z,bool e,bool q,bool s, bool d, bool espace);
    float* densite();
    void pression_ponctuelle(unsigned int n, float* densite,float* pression);
    void force_pression(float* d);
    void visc_ponctuelle(unsigned int n, float* visc);
    void visc(float* d);
    void bord();
    int** liste_indice();
    void tri_liste_indice( int** liste );
    int* liste_indice_debut(int**liste);
    void force_souris();
    void pression_ponctuelle_proche(unsigned int n,  float* densite,float* pression);
    void force_pression_proche(float* d);
    float densite_ponctuelle_visee(float ex, float ey);
   
};


float aire(float rayon, float distance);
float aire_triangle(float base, float hauteur);
float maxi(float a,float b);
int compare(const void* a,const void* b);
#endif