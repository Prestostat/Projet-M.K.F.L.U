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
    float densite_ponctuelle(float ex, float ey, float rayon_influence);
    float* densite(float rayon_influence);
    float* pression_ponctuelle(unsigned int n, float* densite,float rayon_influence);
    void force_pression(float dt,float rayon_influence);

    




};
#endif