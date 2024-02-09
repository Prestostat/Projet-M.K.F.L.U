#ifndef PARTICULE_H
#define PARTICULE_H

class Particule {
    private:
    public:
    float x;
    float y;
    float vx;
    float vy;
    float rayon;

    void initialise_particules(float xini, float yini,float vxini,float vyini,float rayonini);
    void inirapide();
    void position_particule(unsigned int nb_points,float* position);
    void collision(float ammorti);

};
#endif