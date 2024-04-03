#include "../header/aux.h"
#include "../header/ensemble.h"
#include <iostream>
using namespace std;
#define PI 3.14159265
#define E 2.71828182846

void coord_carre(float* position_carre,float x,float y,float tx,float ty){
    position_carre[0]=x-tx;
    position_carre[1]=y-ty;
    position_carre[2]=x-tx;
    position_carre[3]=y+ty;
    position_carre[4]=x+tx;
    position_carre[5]=y-ty;
    position_carre[6]=x+tx;
    position_carre[7]=y+ty;
}

void couleur_densite(float* couleur,float densite_visee,float densite){
    float r,g,b;
    float contraste = 1;
    if (densite/densite_visee<=1-contraste){
        r=0;
        g=0;
        b=1;
    }
    else if (densite/densite_visee>1-contraste && densite/densite_visee<1 ){
        b=1;
        r=densite/(contraste * densite_visee);
        g=densite/(contraste * densite_visee);
    }
    else if(densite>densite_visee && densite/densite_visee<1+contraste){
        r=1;
        b=2-densite/(contraste * densite_visee);
        g=2-densite/(contraste * densite_visee);
    }
    else{
        r=1;
        g=0;
        b=0;
    }
    couleur[0]=r;couleur[1]=g;couleur[2]=b;
}


/**
 * x,y,R,G,B
*/
void position_couleur_densite(Ensemble* f1,Ensemble* f2,float* info,float densite_visee,int res=960){
    float bord_g=-1;float bord_d=1;float bord_b=-1;float bord_h=1;
    float pas_x=(bord_d-bord_g)/(1+res);float tx=(bord_d-bord_g)/res;
    float pas_y=(bord_h-bord_b)/(res+1);float ty=(bord_h-bord_b)/res;
    float x,y,dens;
    float* couleur = (float*)malloc(3*sizeof(float));
    float* carre = (float*)malloc(8*sizeof(float));
    
    for(unsigned int i=0;i<res*res;i++){
        int index =i*4*5;
        x=bord_g+(i/res)*pas_x+pas_x/2.0;
        y=bord_b+(i%res)*pas_y+pas_y/2.0;
        dens=(*f1).densite_ponctuelle_visee(x,y,f2);
        couleur_densite(couleur,densite_visee,dens);
        coord_carre(carre,x,y,tx,ty);
        
        for (unsigned int j=0;j<4;j++){
            info[index+j*5+0]=carre[2*j];
            info[index+j*5+1]=carre[2*j+1];
            info[index+j*5+2]=couleur[0];
            info[index+j*5+3]=couleur[1];
            info[index+j*5+4]=couleur[2];
        }
    }
    free(couleur);free(carre);
}

void rempli_indice(unsigned int* li, unsigned int nb){
    for (unsigned int i=0; i<nb;i++){
        li[6*i]=4*i;
        li[6*i+1]=4*i+1;
        li[6*i+2]=4*i+2;
        li[6*i+3]=4*i+1;
        li[6*i+4]=4*i+2;
        li[6*i+5]=4*i+3;
    }
}
float mini(float a,float b){
    if(a>b){return(b);}
    else{return(a);}
}
void fenetre_principale(const char* name,float* sourisx,float* sourisy,bool* clique_gauche,bool* clique_droit,bool* a_key,bool* z_key,bool* e_key,bool* q_key,bool* s_key,bool* d_key,float* dt,
float* rayon_influence,float* vx_boite,float* vy_boite,float* rayon_action_autour_curseur,float* puissance_action_autour_curseur,int* sens_action_clique_gauche,
bool* affiche_densite,bool* flou,bool* pause_change,bool* pause,float* opacite,bool* affiche_vitesses_colorees,float* vitesse_caracteristique,float* logdt,float* logpacg,float* logvc){
    ImGui::Begin("fenêtre principale");
    {   
        ImGui::Text("%s",name);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SliderFloat("rayon action autour du curseur", (rayon_action_autour_curseur), 0.0f, 0.5f);
        ImGui::SliderFloat("log(dt)", logdt, -7.0, 0.0f);            
        *dt=pow(10,*logdt);
        ImGui::SliderFloat("rayon_influence", rayon_influence, 0.0f, 1.0f);
        ImGui::SliderFloat("log(forces autour du curseur)", logpacg, 0.0f, 5.0f);
        *puissance_action_autour_curseur=pow(E,*logpacg)* *sens_action_clique_gauche;
        ImGui::SliderFloat("opacite", opacite, 0.0f, 1.0f);  
        ImGui::SliderFloat("log(vitesse_caracteristique)", logvc, -2.0f, 2.0f);
        *vitesse_caracteristique=pow(E,*logvc);
        ImGui::SliderFloat("vitesse horizontale de la boite", vx_boite, 0.0f, 1.0f);
        ImGui::SliderFloat("vitesse verticale de la boite", vy_boite, 0.0f, 1.0f);




        if (ImGui::Button("sens de la force du clique gauche")){
            *sens_action_clique_gauche*=-1;}
        ImGui::SameLine();
        if (ImGui::Button("activer le flou")){
            *flou = ! *flou;} 
        ImGui::SameLine();
        if (ImGui::Button("Voir la densité")){
            *affiche_densite = ! *affiche_densite;} 
        ImGui::SameLine();
        if (ImGui::Button("Voir les vitesses en couleurs")){
            *affiche_vitesses_colorees = ! *affiche_vitesses_colorees;} 





        ImGui::Text("Mouse pos: (%g, %g)", 2*io.MousePos.x/960.0 -1,2*io.MousePos.y/960.0 -1);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("espace : pause "); 
        ImGui::Text("clique droit : met toutes les vitesses à 0 "); 
        ImGui::Text("clique gauche : force centrée sur la souris");
        ImGui::Text("a : attraction centrée sur la souris avec les multiplicateurs du clique gauche");  
        ImGui::Text("e : répulsion centrée sur la souris avec les multiplicateurs du clique gauche"); 
        ImGui::Text("z,q,s,d : augmentation de la vitesse du fluide vers le : haut, gauche, bas, droite"); 


        *clique_gauche=ImGui::IsMouseClicked(0,true);
        *clique_droit=ImGui::IsMouseClicked(1,true);
        *a_key=io.KeysDown['Q']; // attention, c'est en Qwerty dans ImGui donc il y a la convertion a faire ici
        *z_key=io.KeysDown['W']; 
        *e_key=io.KeysDown['E'];
        *q_key=io.KeysDown['A'];
        *s_key=io.KeysDown['S'];
        *d_key=io.KeysDown['D'];
        if (*pause_change != io.KeysDown[' ']) {*pause_change=io.KeysDown[' '];if (! *pause_change){*pause = ! *pause;}}
    

        *sourisx=io.MousePos.x;
        *sourisy=io.MousePos.y;
    }
    ImGui::End();
}


void fenetre_liquide(const char* name,float* rayon_collision,float* g,float* masse,float* multiplicateur_pression,float* multiplicateur_pression_proche,float* densite_visee,
float* coeff_amorti,float* coeff_viscosite,float* coeff_adherence,float* logg,float* logmp,float* logmpp,float* logvisc,int* texd,int* texf){
    ImGui::Begin(name);
    {   
        ImGui::Text("%s",name);
        //ImGui::SliderFloat("rayon_collision",rayon_collision ,0.0 , 0.5);
        //ImGui::SameLine();
        ImGui::InputFloat("rayon_collision",rayon_collision);
        ImGui::SliderFloat("log(g)", logg,0.0 ,10.0 );
        *g=pow(10,*logg);
        if (ImGui::Button("g=0")){
            *logg = -100000000;
            *g=pow(10,*logg);} 
        ImGui::SliderFloat("masse", masse,0 ,10.0 );
        ImGui::SliderFloat("log(multiplicateur_pression)", logmp,0.0 , 10.0);
        *multiplicateur_pression=pow(10,*logmp);
        ImGui::SliderFloat("log(multiplicateur_pression_proche)", logmpp,0.0 , 10.0);
        *multiplicateur_pression_proche=pow(10,*logmpp);
        //ImGui::SliderFloat("densite_visee", densite_visee,0 ,20000 );
        //ImGui::SameLine();
        ImGui::InputFloat("densite_visee", densite_visee);
        ImGui::SliderFloat("coeff_amorti",coeff_amorti ,0 , 2);
        ImGui::SliderFloat("log(coeff_viscosite)",logvisc ,-4.0 , 2.0);
        *coeff_viscosite=pow(10,*logvisc);
        ImGui::SliderFloat("coeff_adherence", coeff_adherence,0 ,1 );
        ImGui::InputInt("début texture", texd);
        ImGui::InputInt("fin texture, >= début texture", texf);
        if (*texf<*texd){*texf=*texd;}
    }
    ImGui::End();
}

void fenetre_interaction(const char* name,float* densite_visee_melange,float* pression_melange, float* logpmel,float* logppmel,float* pression_proche_melange,float* viscosite_melange,float* logviscmel){
    ImGui::Begin(name);
    {
        ImGui::Text("%s",name);
        //ImGui::SliderFloat("densite visée mélange",densite_visee_melange ,0.0 , 5000);
        //ImGui::SameLine();
        ImGui::InputFloat("densite visée mélange",densite_visee_melange);
        ImGui::SliderFloat("log(pression_melange)", logpmel,0.0 ,5.0 );
        *pression_melange=pow(10,*logpmel);
        ImGui::SliderFloat("log(pression_proche_melange)", logppmel,0.0 ,5.0 );
        *pression_proche_melange=pow(10,*logppmel);
        ImGui::SliderFloat("log(viscosité mélange)", logviscmel,-5.0 ,0 );
        *viscosite_melange=pow(10,*logviscmel);
        

    }
    ImGui::End();
}
