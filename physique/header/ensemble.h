#ifndef ENSEMBLE_H
#define ENSEMBLE_H
#include "particule.h"
#include <map>
#include <string>
class Ensemble {
    private:
    public:
    unsigned int nombre_de_particules;
    Particule* data;
    int** indices;
    int* indice_debut;
    std::map<std::string,float> constantes;

    Ensemble (unsigned int nb,std::map<std::string,float> constantes);
    ~Ensemble();
    Ensemble (const Ensemble&);

    void gravite();
    void deplacement();
    void evolution(std::map<std::string,float> constantes,std::map<std::string,bool> controles);
    float densite_ponctuelle(float ex, float ey);
    float* densite();
    void pression_ponctuelle(unsigned int n, float* densite,float* pression);
    void force_pression(float* d);
    void visc_ponctuelle(unsigned int n, float* visc);
    void visc(float* d);
    int** liste_indice();
    void tri_liste_indice( int** liste );
    int* liste_indice_debut(int**liste);
    void force_souris(std::map<std::string,bool> controles);
    void pression_ponctuelle_proche(unsigned int n,  float* densite,float* pression);
    void force_pression_proche(float* d);
};
int compare(const void* a,const void* b);
#endif