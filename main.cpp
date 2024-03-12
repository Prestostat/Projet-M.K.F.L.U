#include <iostream>
#include <fstream>
#include <math.h>
#include "///usr/include/GL/glew.h"
#include <GLFW/glfw3.h>
#include "OPENGL/header/Renderer.h"
#include "OPENGL/header/VertexBuffer.h"
#include "OPENGL/header/IndexBuffer.h"
#include "OPENGL/header/VertexArray.h"
#include "OPENGL/header/Shader.h"
#include "OPENGL/header/VertexBufferLayout.h"
#include "physique/header/particule.h"
#include "physique/header/ensemble.h"
#include "OPENGL/src/vendor/imgui/imgui.h"
#include "OPENGL/src/vendor/imgui/imgui_impl_glfw_gl3.h"
#include <map>
#include <chrono>
#include <thread>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.14159265
#define E 2.71828182846
using namespace std;


float mini(float a,float b){
    if(a>b){return(b);}
    else{return(a);}
}
void fenetre_principale(const char* name,float* sourisx,float* sourisy,bool* clique_gauche,bool* clique_droit,bool* a_key,bool* z_key,bool* e_key,bool* q_key,bool* s_key,bool* d_key,float* dt,
float* rayon_influence,float* vx_boite,float* vy_boite,float* rayon_action_autour_curseur,float* puissance_action_autour_curseur,int* sens_action_clique_gauche,
bool* affiche_densite,bool* flou,bool* pause_change,bool* pause,float* opacite,bool* affiche_vitesses_colorees,float* vitesse_caracteristique,float* logdt,float* logpacg,float* logvc){
    ImGui::Begin("fenêtre principale");
    {   
        ImGui::Text(name);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SliderFloat("rayon action autour du curseur", (rayon_action_autour_curseur), 0.0f, 0.5f);
        ImGui::SliderFloat("log(dt)", logdt, -7.0, 0.0f);            
        *dt=pow(10,*logdt);
        ImGui::SliderFloat("rayon_influence", rayon_influence, 0.0f, 1.0f);
        ImGui::SliderFloat("log(forces autour du curseur)", logpacg, 0.0f, 5.0f);
        *puissance_action_autour_curseur=pow(E,*logpacg)* *sens_action_clique_gauche;
        ImGui::SliderFloat("opacite", opacite, 0.0f, 100.0f);  
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





        ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x/960.0 -1, io.MousePos.y/960.0 -1);
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
float* coeff_amorti,float* coeff_viscosite,float* coeff_adherence,float* logg,float* logmp,float* logmpp,float* logvisc){
    ImGui::Begin(name);
    {   
        ImGui::Text(name);
        ImGui::SliderFloat("rayon_collision",rayon_collision ,0.0 , 0.5);
        ImGui::SliderFloat("log(g)", logg,0.0 ,10.0 );
        *g=pow(10,*logg);
        if (ImGui::Button("g=0")){
            *g = 0;} 
        ImGui::SliderFloat("masse", masse,0 ,10.0 );
        ImGui::SliderFloat("log(multiplicateur_pression)", logmp,0.0 , 10.0);
        *multiplicateur_pression=pow(10,*logmp);
        ImGui::SliderFloat("log(multiplicateur_pression_proche)", logmpp,0.0 , 10.0);
        *multiplicateur_pression_proche=pow(10,*logmpp);
        ImGui::SliderFloat("densite_visee", densite_visee,0 ,5000 );
        ImGui::SliderFloat("coeff_amorti",coeff_amorti ,0 , 2);
        ImGui::SliderFloat("log(coeff_viscosite)",logvisc ,-4.0 , 2.0);
        *coeff_viscosite=pow(10,*logvisc);
        ImGui::SliderFloat("coeff_adherence", coeff_adherence,0 ,1 );
    }
    ImGui::End();
}

void fenetre_interaction(string name){
    cout << " A définir " << name << endl;
}


int main(){
    
    // Paramètre pour l'affichage de rond avec la méthode "Triangle Fan". Non modifiable durant la simulation
    unsigned int nombre_de_points =3; // doit être supérieur a 3 (pour au moins avoir 1 triangle)
    unsigned int nombre_de_triangles = nombre_de_points-2;


    int resolution_densite = 100; // Change le nombre de "pixel" utilisés pour afficher la densité. Non modifiable durant la simulation

    // Options pour ce que l'on souhaite afficher. Modifiable durant la simulation
    bool affiche_densite =false; 
    bool flou = false;
    bool pause=false;
    bool pause_change = false;
    float opacite = 10; // permet d'influer sur la transparence lorsque le flou est actif. 
    bool affiche_vitesses_colorees = false; // permet de creer un gradient de couleur entre les particules lente et rapide. 
    float vitesse_caracteristique =1; // influe sur l'échelle des vitesses pour l'affichage. 
    //Rouge : v_particule >= 2*vitesse_caracteristique, magenta si v_particule=vitesse_caracteristique, bleu si v_particule=0. 
    //Augmentation/diminution linéaire des composantes rouges et bleu en fonction de la vitesse.

    // variables  servant à interagir avec le fluide en imposant des forces extérieurs. Le clique gauche n'est pas consistent, il est recommendé d'utiliser "a" et "e"
    float sourisx = 0; // position du curseur
    float sourisy = 0;
    bool clique_gauche=false; //Variables contenant true si la touche est pressé.
    bool clique_droit=false;
    bool a_key=false;
    bool z_key=false;
    bool e_key=false;
    bool q_key=false;
    bool s_key=false;
    bool d_key=false;

    float rayon_action_autour_curseur =0.5;
    float puissance_action_autour_curseur = 100;
    int sens_action_clique_gauche=1; 
    
    // Paramètres Partagés, modifiable durant la simulation

    float dt =0.0001; // Partagé pour que la simutation reste cohérente, le temps s'écoule pour tout le monde pareil(dans notre modeste modèle)
    float rayon_influence =0.03; // Partagé pour simplifier les interaction entre particules différentes et simplifier lors de l'utilisation des listes d'indices
    float vx_boite = 0; // vitesse des bords haut et bas de la boite
    float vy_boite = 0; // vitesse des bords gauch et droit de la boite



    // Paramètres pour le premier fluide

    unsigned int nombre_de_particules =10; // Non modifiable durant la simulation
    
    // Paramètres modifiables durant la simulation
    float rayon_collision = 0.01; // rayon utilisé pour les collisions avec les parois et les objets. N'intervient pas dans les interactions particulaire. Rayon utilisé pour l'affichage (sans flou)
    float g = 10; // contole la norme de la gravitée. A utiliser pour faire varier les densitées des fluides (A justifier, cf : )
    float masse = 1; // Proportionnel à la masse ajouté par particule, répartie dans le disque de rayon rayon_influence. 
    float multiplicateur_pression = 100; // Force de l'interaction de répulsion/attraction entre 2 particules de même type
    float multiplicateur_pression_proche = 100; // Force de l'interaction de répulsion entre 2 particules de même type a courte distance. Aide à la génération de pseudo-tension de surface
    float densite_visee = 5; // Densitée à laquelle il n'y à plus de force de pression. (peut encore y avoir de la force de pression proche)
    float coeff_amorti = 0.9; // coefficient multiplicatif lors du rebond sur un mur
    float coeff_viscosite = 0.01; // coefficient multiplicatif de la force visqueuse entre 2 particules de même type
    float coeff_adherence = 0.002; // Coefficient d'adhérence à la paroi



    // Paramètres pour le second fluiden il sera définit si nombre_de_particule2>0

    unsigned int nombre_de_particules2 =0;// Non modifiable durant la simulation
    
    // Paramètres modifiables durant la simulation
    float rayon_collision2 = 0.01; // rayon utilisé pour les collisions avec les parois et les objets. N'intervient pas dans les interactions particulaire. Rayon utilisé pour l'affichage (sans flou)
    float g2 = 10; // contole la norme de la gravitée. A utiliser pour faire varier les densitées des fluides (A justifier, cf : )
    float masse2 = 1; // Proportionnel à la masse ajouté par particule, répartie dans le disque de rayon rayon_influence. 
    float multiplicateur_pression2 = 100; // Force de l'interaction de répulsion/attraction entre 2 particules de même type
    float multiplicateur_pression_proche2 = 100; // Force de l'interaction de répulsion entre 2 particules de même type a courte distance. Aide à la génération de pseudo-tension de surface
    float densite_visee2 = 200; // Densitée à laquelle il n'y à plus de force de pression. (peut encore y avoir de la force de pression proche)
    float coeff_amorti2 = 0.9; // coefficient multiplicatif lors du rebond sur un mur
    float coeff_viscosite2 = 0.1; // coefficient multiplicatif de la force visqueuse entre 2 particules de même type
    float coeff_adherence2 = 0.005; // Coefficient d'adhérence à la paroi

    
    //Paramètres d'interaction entre les 2 fluides



    // Variables outils pour changer les paramètres de façons logarithmiques.
    float logdt = log10(dt);
    float logpacg = log10(puissance_action_autour_curseur);
    float logvc = log10(vitesse_caracteristique);

    float logg = log10(g);
    float logmp =log10(multiplicateur_pression);
    float logmpp = log10(multiplicateur_pression_proche);
    float logvisc = log10(coeff_viscosite);

    float logg2 = log10(g2);
    float logmp2 =log10(multiplicateur_pression2);
    float logmpp2 = log10(multiplicateur_pression_proche2);
    float logvisc2 = log10(coeff_viscosite2);
    
    
    // Initialiqation des deux liquides

    Ensemble fluide = Ensemble(nombre_de_particules,rayon_collision);
    Ensemble fluide2 = Ensemble(nombre_de_particules2,rayon_collision);
    

    
    //Initialisation d'OpenGL pour l'affichage. Utilisation de la série tutoriel de The Cherno pour la création des fichiers initiaux relatifs à OpenGl  l
    //ien de la playlist :https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,5);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 960, "Simulation M.K.F.L.U", NULL, NULL); // Ne pas changer la taille en pixel de la fenêtre (calcul de la position de la souris et l'affichage en dépendent)
    
    if (!window)
    {
        GLCall(glfwTerminate());
        return -1;
    } 

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(10);
    
    if (glewInit()!= GLEW_OK){
        std::cout << "erreur"<< std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;

    // Active la possibilité de transparence et défini la fonction de transparence
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));





    // Tableau d'indices pour parcourir les points du tableau position dans le bon ordre pour tracer un disque avec la méthode "triangle Fan"
    unsigned int indices[nombre_de_triangles*3];
    for (unsigned int i=0; i<nombre_de_triangles;i++) {
        indices[3*i]=0;
        indices[3*i+1]=i+1;
        indices[3*i+2]=i+2;
    }
    //Tableau d'indice indiquant l'ordre de parcours de la position_carre pour tracer un rectacle 
    unsigned int indices_densite[6] = {0,1,2,1,2,3};

    //déclaration des tableaux qui contiendront les positions des points pour un cercle avec la méthode "Triangle Fan" ou un Carré.
    float position[nombre_de_points*2];
    float position_carre[4*2];
    
    // Tableau contenant les couleurs utilisées pour dessiner un élément. En RGB entre 0 et 1
    float* clear_color = (float*)malloc(3*sizeof(float));
    clear_color[0]=1;
    clear_color[1]=0;
    clear_color[2]=0;

    // Initialisation des objets nécessaire pour l'affichage
    Shader triangle_shader("OPENGL/res/shaders/triangle.shader");
    Shader disque_shader("OPENGL/res/shaders/disque.shader");
    Renderer renderer;

    //initialisation de ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
    




    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        ImGui_ImplGlfwGL3_NewFrame();
        //fenetre_ImGui(&rayon_collision,&g, &logg, &multiplicateur_pression, &logmp, &multiplicateur_pression_proche, &logmpp, &densite_visee, &dt,  &logdt, &rayon_influence, &coeff_amorti, &coeff_viscosite, &sourisx, &sourisy, &rayon_action_autour_curseur, &puissance_action_autour_curseur,&sens_action_clique_gauche, &logpacg,&coeff_adherence, &logvc , &vitesse_caracteristique,&opacite,&flou,&affiche_densite, &clique_gauche, &clique_droit, &a_key, &z_key, &e_key, &q_key, &s_key, &d_key,&pause);
        fenetre_principale("dodo", &sourisx, &sourisy, &clique_gauche, &clique_droit, &a_key, &z_key, &e_key, &q_key, &s_key, &d_key, &dt,
 &rayon_influence, &vx_boite, &vy_boite, &rayon_action_autour_curseur, &puissance_action_autour_curseur, &sens_action_clique_gauche,
 &affiche_densite, &flou, &pause_change, &pause, &opacite, &affiche_vitesses_colorees, &vitesse_caracteristique, &logdt, &logpacg, &logvc);

        fenetre_liquide("fluide 1", &rayon_collision, &g, &masse, &multiplicateur_pression, &multiplicateur_pression_proche, &densite_visee,
 &coeff_amorti, &coeff_viscosite, &coeff_adherence, &logg, &logmp, &logmpp, &logvisc);
        if (nombre_de_particules2>0){
            fenetre_liquide("fluide 2", &rayon_collision2, &g2, &masse2, &multiplicateur_pression2, &multiplicateur_pression_proche2, &densite_visee2,
 &coeff_amorti2, &coeff_viscosite2, &coeff_adherence2, &logg2, &logmp2, &logmpp2, &logvisc2);
        }
        
        
        

        if (affiche_densite){
            float pas =2.0/resolution_densite;
            for (unsigned int i=0;i<resolution_densite*resolution_densite;i++){
                position_carre[0]=-1.0+i%resolution_densite*pas;
                position_carre[1]=-1.0+i/resolution_densite*pas;
                position_carre[2]=-1.0+(i%resolution_densite+1)*pas;
                position_carre[3]=-1.0+i/resolution_densite*pas;
                position_carre[4]=-1.0+i%resolution_densite*pas;
                position_carre[5]=-1.0+(i/resolution_densite+1)*pas;
                position_carre[6]=-1.0+(i%resolution_densite+1)*pas;
                position_carre[7]=-1.0+(i/resolution_densite+1)*pas;

                float densité_grille= fluide.densite_ponctuelle_visee(position_carre[0]+pas/2,position_carre[1]+pas/2,&fluide2);
                float r=mini(1.0,maxi(0.0,densité_grille/densite_visee -1));
                float g=mini(0.0,pow(mini(abs(densité_grille/densite_visee),abs(2-densité_grille/densite_visee)),4));
                float b=maxi(0.0,1-densité_grille/densite_visee);

                triangle_shader.Bind();
                triangle_shader.SetUniform4f("u_color",r,g,b,1.0f);            
                VertexArray va;                
                VertexBuffer vb(position_carre,4*2*sizeof(float));
                VertexBufferLayout layout;
                layout.Push<float>(2);
                va.AddBuffer(vb,layout);
                IndexBuffer ib(indices_densite,6);
                renderer.Draw(va,ib,triangle_shader);

            }

            
        }
        

        for (unsigned int i=0;i<nombre_de_particules;i++){
            fluide.data[i].rayon = rayon_collision;
            if (flou){
                fluide.data[i].position_particule(nombre_de_points,rayon_influence,position );
            }
            else {
                fluide.data[i].position_particule(nombre_de_points,rayon_collision,position );
            }
            
            clear_color=fluide.data[i].couleur(vitesse_caracteristique);

            disque_shader.Bind();
            if (flou){
                disque_shader.SetUniform4f("information",opacite/(densite_visee*rayon_influence),rayon_influence/2,(fluide.data[i].x+1)/2,(fluide.data[i].y+1)/2);
            }
            else {
                disque_shader.SetUniform4f("information",10000,rayon_collision/2,(fluide.data[i].x+1)/2,(fluide.data[i].y+1)/2);
            }
            
            disque_shader.SetUniform4f("u_color",clear_color[0],clear_color[1],clear_color[2],1.0f);
                        
            VertexArray va;                
            VertexBuffer vb(position,nombre_de_points*2*sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);
            va.AddBuffer(vb,layout);
            IndexBuffer ib(indices,3*nombre_de_triangles);
            renderer.Draw(va,ib,disque_shader);
        }

        
        for (unsigned int i=0;i<nombre_de_particules2;i++){
            fluide2.data[i].rayon = rayon_collision;
            if (flou){
                fluide2.data[i].position_particule(nombre_de_points,rayon_influence,position );
            }
            else {
                fluide2.data[i].position_particule(nombre_de_points,rayon_collision,position );
            }
            
            clear_color=fluide2.data[i].couleur(vitesse_caracteristique);

            disque_shader.Bind();
            if (flou){
                disque_shader.SetUniform4f("information",opacite/(densite_visee*rayon_influence),rayon_influence/2,(fluide2.data[i].x+1)/2,(fluide2.data[i].y+1)/2);
            }
            else {
                disque_shader.SetUniform4f("information",10000,rayon_collision/2,(fluide2.data[i].x+1)/2,(fluide2.data[i].y+1)/2);
            }
            
            disque_shader.SetUniform4f("u_color",0,1,0,1.0f);
                        
            VertexArray va;                
            VertexBuffer vb(position,nombre_de_points*2*sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);
            va.AddBuffer(vb,layout);
            IndexBuffer ib(indices,3*nombre_de_triangles);
            renderer.Draw(va,ib,disque_shader);
        }
        if (! pause){

            if (nombre_de_particules2==0) {
                fluide.actualise_constantes(rayon_collision,g,masse,multiplicateur_pression,multiplicateur_pression_proche,densite_visee,dt,rayon_influence,coeff_amorti,coeff_viscosite,sourisx,sourisy,rayon_action_autour_curseur,puissance_action_autour_curseur,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
                fluide.evolution();
                fluide.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);
        
            }
            else {
                fluide.actualise_constantes(rayon_collision,g,masse,multiplicateur_pression,multiplicateur_pression_proche,densite_visee,dt,rayon_influence,coeff_amorti,coeff_viscosite,sourisx,sourisy,rayon_action_autour_curseur,puissance_action_autour_curseur,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
                fluide2.actualise_constantes(rayon_collision2,g2,masse2,multiplicateur_pression2,multiplicateur_pression_proche2,densite_visee2,dt,rayon_influence,coeff_amorti2,coeff_viscosite2,sourisx,sourisy,rayon_action_autour_curseur,puissance_action_autour_curseur,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
                interaction(&fluide,&fluide2);
                fluide2.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);
                fluide.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);
            }
        }
        
        //Affiche ImGui
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        GLCall(glfwSwapBuffers(window));
        GLCall(glfwPollEvents());
    }
    //ferme ImGUi
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    return(0);

}