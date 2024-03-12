#ifndef OBSTACLE_H
#define OBSTACLE_H
#include "particule.h"
#include <string>
using namespace std;

class Obstacle{
    public:
    float* x_obs;
    float* y_obs;
    float vx_obs;
    float vy_obs;
    float c = 0.1;
    float xini;
    float yini;
    unsigned int N = 1000;
    string nom_obstacle;
    float masse=1;
    float coeff_amorti_obstacle;
    bool deplacement_ON_OFF = false;

    void init_Obstacle(string nom_obstacle_, float masse_, float coeff_amorti_obstacle_, float xini_, float yini_, float vx_obs_, float vy_obs_);
    void collision_obstacle(Particule* data, float masse_particule, unsigned int nombre_de_particules, bool deplacement_ON_OFF);

};
#endif