#ifndef ENSEMBLE_H
#define ENSEMBLE_H

#include "particule.h"
#include "obstacle.h"

class Ensemble {
    private:
    public:
    unsigned int nombre_de_particules;
    Particule* data;
    int** indices;
    int* indice_debut;
    float rayon_collision ;
    float g ;
    float masse = 1;
    float multiplicateur_pression ;
    float multiplicateur_pression_proche ;
    float densite_visee ;
    float dt ;
    float rayon_influence ;
    float coeff_amorti;
    float coeff_viscosite ;
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

    Obstacle Obs;

    Ensemble (unsigned int nb,float rayon_collision);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite();
    void deplacement();
    void liste_indice(int** liste);
    void tri_liste_indice( int** liste );
    void liste_indice_debut(int* liste_i ,int** liste);
    void force_souris();
    
    void frottement_paroi(float vx_paroi, float vy_paroi, float xlim_d, float xlim_g, float ylim_h, float ylim_b,float coeff_adherence);
    void actualise_constantes(float rayon_collision,float g,float masse,float multiplicateur_pression,float multiplicateur_pression_proche,float densite_visee,float dt,float rayon_influence,float coeff_amorti,float coeff_viscosite,float sourisx,float sourisy,float rayon_action_clique_gauche,float puissance_action_clique_gauche,bool clique_gauche_,bool clique_droit_,bool a,bool z,bool e,bool q,bool s, bool d, bool espace);
    void actualise_listes();

    void rempli_listes(Ensemble* f,int* coord, int* liste_cellules,int* liste_cellules2);
    void densite(Ensemble* f,float* d);
    float densite_ponctuelle_visee(float ex, float ey,Ensemble* f);
    void pression_ponctuelle(unsigned int n, float* pression,Ensemble* l2, float* d1, float* d2,float pression_mutuelle,float densite_visee_melange);
    void force_pression(Ensemble* l2, float* d1, float* d2,float pression_mutuelle,float densite_visee_melange);
    void pression_ponctuelle_proche(unsigned int n,  float* pression,Ensemble* l2, float* d1, float* d2,float pression_proche_melange);
    void force_pression_proche(Ensemble* l2, float* d1, float* d2,float pression_proche_melange);
    void visc_ponctuelle(unsigned int n,float* visc,Ensemble* l2,float viscosite_melange);
    void visc(Ensemble* l2, float* d1,float viscosite_melange);
    void addforce2(float* d1,Ensemble* l2,float* d2,float pression_melange,float pression_proche_melange,float densite_visee_melange,float viscosite_melange);
    void rempli_info_point(float* ip,float vc,float opacite,bool affiche_vitesses_colorees,float TexID_d,float TexID_f);
   
};
void interaction(Ensemble* l1, Ensemble* l2,float pression_melange,float pression_proche_melange,float densite_visee_melange,float viscosite_melange);
float aire(float rayon, float distance);
float aire_triangle(float base, float hauteur);
float maxi(float a,float b);
int compare(const void* a,const void* b);
float influence_paroi(float dst,float rayon_influence,float coeff_adherence);


#endif