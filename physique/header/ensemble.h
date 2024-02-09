#ifndef ENSEMBLE_H
#define ENSEMBLE_H
#include "particule.h"
class Ensemble {
    private:
    public:
    unsigned int nombre_de_particules;
    Particule* data;
    Ensemble (unsigned int nb,float rayon);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite(float dt,float g);
    void deplacement(float dt);
    void evolution(float dt,float g);

    




};
#endif