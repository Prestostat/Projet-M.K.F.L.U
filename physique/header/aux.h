#ifndef AUX_H
#define AUX_H

#include "ensemble.h"
#include <math.h>
#include "../../OPENGL/src/vendor/imgui/imgui.h"
#include "../../OPENGL/src/vendor/imgui/imgui_impl_glfw_gl3.h"
void coord_carre(float* position_carre,float x,float y,float tx,float ty);
void couleur_densite(float* couleur,float densite_visee,float densite);
void position_couleur_densite(Ensemble* f1,Ensemble* f2,float* info,float densite_visee,int res);
void rempli_indice(unsigned int* li, unsigned int nb);
void fenetre_interaction(const char* name,float* densite_visee_melange,float* pression_melange, float* logpmel,float* logppmel,float* pression_proche_melange,float* viscosite_melange,float* logviscmel);
void fenetre_liquide(const char* name,float* rayon_collision,float* g,float* masse,float* multiplicateur_pression,float* multiplicateur_pression_proche,float* densite_visee,float* coeff_amorti,float* coeff_viscosite,float* coeff_adherence,float* logg,float* logmp,float* logmpp,float* logvisc,int* texd,int* texf);
void fenetre_principale(const char* name,float* sourisx,float* sourisy,bool* clique_gauche,bool* clique_droit,bool* a_key,bool* z_key,bool* e_key,bool* q_key,bool* s_key,bool* d_key,float* dt,
float* rayon_influence,float* vx_boite,float* vy_boite,float* rayon_action_autour_curseur,float* puissance_action_autour_curseur,int* sens_action_clique_gauche,
bool* affiche_densite,bool* flou,bool* pause_change,bool* pause,float* opacite,bool* affiche_vitesses_colorees,float* vitesse_caracteristique,float* logdt,float* logpacg,float* logvc);
float mini(float a,float b);

#endif