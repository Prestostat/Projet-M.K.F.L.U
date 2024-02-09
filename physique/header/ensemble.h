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
    




};
#endif