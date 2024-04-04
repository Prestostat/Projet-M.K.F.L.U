#include <iostream>
#include <fstream>
#include <math.h>
#include <thread>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


#include "///usr/include/GL/glew.h"
#include <GLFW/glfw3.h>

#include "OPENGL/src/vendor/stb_image/stb_image.h" // bibliothèque permettant d'utiliser facilement des images pour les textures
#include "OPENGL/src/vendor/glm/glm.hpp" // bibliothèque de maths adapté à OpenGL
#include "OPENGL/src/vendor/glm/gtc/matrix_transform.hpp"// bibliothèque de maths adapté à OpenGL

#include "OPENGL/src/vendor/imgui/imgui.h" // ImGui
#include "OPENGL/src/vendor/imgui/imgui_impl_glfw_gl3.h"// ImGui

// Les fichiers ci dessous sont ceux que nous avons fait, avec l'aide du tutoriel OpenGl de The cherno pour les fichier du dossier OPENGL, et inspiration  du travail de Sebastian Lague pour débuter la simulation du fluide
#include "physique/header/particule.h" 
#include "physique/header/ensemble.h"
#include "physique/header/aux.h"

#include "OPENGL/header/Renderer.h"
#include "OPENGL/header/VertexBuffer.h"
#include "OPENGL/header/IndexBuffer.h"
#include "OPENGL/header/VertexArray.h"
#include "OPENGL/header/Shader.h"
#include "OPENGL/header/VertexBufferLayout.h"
#include "OPENGL/header/texture.h"


#define PI 3.14159265
#define E 2.71828182846
using namespace std;




int main(){

    bool initial = true;// True tant que la définition des paramètres n'est pas fini

    int resolution_densite = 960/4; // Change le nombre de "pixel" utilisés pour afficher la densité. Non modifiable durant la simulation

    // Options pour ce que l'on souhaite afficher. Modifiable durant la simulation
    bool affiche_densite =false; 
    bool flou = false;
    bool pause=false;
    bool pause_change = false;
    float opacite = 1; // permet d'influer sur la transparence lorsque le flou est actif. 
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

    float dt =0.003; // Partagé pour que la simutation reste cohérente, le temps s'écoule pour tout le monde pareil(dans notre modeste modèle)
    float rayon_influence =0.1; // Partagé pour simplifier les interaction entre particules différentes et simplifier lors de l'utilisation des listes d'indices
    float vx_boite = 0; // vitesse des bords haut et bas de la boite
    float vy_boite = 0; // vitesse des bords gauch et droit de la boite



    // Paramètres pour le premier fluide

    unsigned int nombre_de_particules =1000; // Non modifiable durant la simulation
    int nb =nombre_de_particules; // Pour l'initialisation car je n'ai pas trouvé d'équivalent a ImGui::InputInt pour un unsigned int.

    // Paramètres modifiables durant la simulation
    float rayon_collision = 0.01; // rayon utilisé pour les collisions avec les parois et les objets. N'intervient pas dans les interactions particulaire. Rayon utilisé pour l'affichage (sans flou)
    float g = 10; // contole la norme de la gravitée. A utiliser pour faire varier les densitées des fluides (A justifier, cf : )
    float masse = 1; // Proportionnel à la masse ajouté par particule, répartie dans le disque de rayon rayon_influence. 
    float multiplicateur_pression = 100; // Force de l'interaction de répulsion/attraction entre 2 particules de même type
    float multiplicateur_pression_proche = 1000; // Force de l'interaction de répulsion entre 2 particules de même type a courte distance. Aide à la génération de pseudo-tension de surface
    float densite_visee = 1000; // Densitée à laquelle il n'y à plus de force de pression. (peut encore y avoir de la force de pression proche)
    float coeff_amorti = 0.9; // coefficient multiplicatif lors du rebond sur un mur
    float coeff_viscosite = 0.1; // coefficient multiplicatif de la force visqueuse entre 2 particules de même type
    float coeff_adherence = 0.005; // Coefficient d'adhérence à la paroi
    int texture_debut = 0; // utilisation de texture pour le fluide, celles qui sont bind entre texturedebut et texturefin. 0 correspond a aucune texture.
    int texture_fin = 0;

    // Paramètres pour le second fluiden il sera définit si nombre_de_particule2>0

    unsigned int nombre_de_particules2 =0;// Non modifiable durant la simulation
    int nb2 =nombre_de_particules2; // Pour l'initialisation car je n'ai pas trouvé d'équivalent a ImGui::InputInt pour un unsigned int.
    
    
    // Paramètres modifiables durant la simulation
    float rayon_collision2 = 0.01; // rayon utilisé pour les collisions avec les parois et les objets. N'intervient pas dans les interactions particulaire. Rayon utilisé pour l'affichage (sans flou)
    float g2 = 10; // contole la norme de la gravitée. A utiliser pour faire varier les densitées des fluides (A justifier, cf : )
    float masse2 = 1; // Proportionnel à la masse ajouté par particule, répartie dans le disque de rayon rayon_influence. 
    float multiplicateur_pression2 = 100; // Force de l'interaction de répulsion/attraction entre 2 particules de même type
    float multiplicateur_pression_proche2 = 1000; // Force de l'interaction de répulsion entre 2 particules de même type a courte distance. Aide à la génération de pseudo-tension de surface
    float densite_visee2 = 1000; // Densitée à laquelle il n'y à plus de force de pression. (peut encore y avoir de la force de pression proche)
    float coeff_amorti2 = 0.9; // coefficient multiplicatif lors du rebond sur un mur
    float coeff_viscosite2 = 0.1; // coefficient multiplicatif de la force visqueuse entre 2 particules de même type
    float coeff_adherence2 = 0.005; // Coefficient d'adhérence à la paroi
    int texture_debut2 = 0; // utilisation de texture pour le fluide, celles qui sont bind entre texturedebut et texturefin. 0 correspond a aucune texture.
    int texture_fin2 = 0;
    
    //Paramètres d'interaction entre les 2 fluides
    float pression_melange =100;
    float pression_proche_melange =1000;
    float viscosite_melange=0.1;
    float densite_visee_melange=900;


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

    float logpmel = log10(pression_melange);
    float logppmel=log10(pression_proche_melange);
    float logviscmel=log10(viscosite_melange);

    
    

    

    
    //Initialisation d'OpenGL pour l'affichage. Utilisation de la série tutoriel de The Cherno pour la création des fichiers initiaux relatifs à OpenGl  
    //lien de la playlist :https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
    
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

    
    int TexID[32]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};//utile pour avoir plusieurs texture par fluide
    // matrice de transformation, mais peu utilisé dans notre cas
    glm::mat4 proj = glm::ortho(-1.0,1.0,-1.0,1.0,-1.0,1.0);
    glm::mat4 view=glm::translate(glm::mat4(1.0),glm::vec3(0,0,0));
    glm::mat4 model=glm::translate(glm::mat4(1.0),glm::vec3(0,0,0));
    glm::mat4 mvp = proj * view * model;

    // Initialisation des objets nécessaire pour l'affichage
    Shader densite_shader("OPENGL/res/shaders/densite.shader");
    Shader fluide_shader("OPENGL/res/shaders/fluide.shader");
    Renderer renderer;
    fluide_shader.Bind();
    // importation des textures et bind de celles ci.
    Texture t1("OPENGL/res/textures/eau.png");Texture t2("OPENGL/res/textures/eau1.png");
    Texture t3("OPENGL/res/textures/eau2.png");Texture t4("OPENGL/res/textures/eau3.png");
    Texture t5("OPENGL/res/textures/huile.png");Texture t6("OPENGL/res/textures/huile2.png");
    Texture t7("OPENGL/res/textures/huile3.png");Texture t8("OPENGL/res/textures/huile4.png");
    
    Texture texture=t1;texture.Bind(0);texture = t2;texture.Bind(1);
    texture = t3;texture.Bind(2);texture = t4;texture.Bind(3);
    texture = t5;texture.Bind(4);texture = t6;texture.Bind(5);
    texture = t7;texture.Bind(6);texture = t8;texture.Bind(7);
    
    
    
    //initialisation de ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
    

    //Initialisation des particules et des paramètres avant le début de la simulation
    while (initial && (!glfwWindowShouldClose(window)))
    {



        ImGui_ImplGlfwGL3_NewFrame();        
        // actualise les constante à l'aide d'ImGui
        
        fenetre_principale("principale", &sourisx, &sourisy, &clique_gauche, &clique_droit, &a_key, &z_key, &e_key, &q_key, &s_key, &d_key, &dt,
 &rayon_influence, &vx_boite, &vy_boite, &rayon_action_autour_curseur, &puissance_action_autour_curseur, &sens_action_clique_gauche,
 &affiche_densite, &flou, &pause_change, &pause, &opacite, &affiche_vitesses_colorees, &vitesse_caracteristique, &logdt, &logpacg, &logvc);

        fenetre_liquide("fluide 1", &rayon_collision, &g, &masse, &multiplicateur_pression, &multiplicateur_pression_proche, &densite_visee,
 &coeff_amorti, &coeff_viscosite, &coeff_adherence, &logg, &logmp, &logmpp, &logvisc,&texture_debut,&texture_fin);
        if (nombre_de_particules2>0){
            fenetre_liquide("fluide 2", &rayon_collision2, &g2, &masse2, &multiplicateur_pression2, &multiplicateur_pression_proche2, &densite_visee2,
 &coeff_amorti2, &coeff_viscosite2, &coeff_adherence2, &logg2, &logmp2, &logmpp2, &logvisc2,&texture_debut2,&texture_fin2);
            fenetre_interaction( "mélange", &densite_visee_melange, &pression_melange,  &logpmel, &logppmel, &pression_proche_melange, &viscosite_melange, &logviscmel);
        }

        
        //Fenetre ImGui permettant d'initialiser les nombres de particules et de valider
        ImGui::Begin("Validation des conditions initiales");
        {
            ImGui::InputInt("Nombre de particules 1",&nb);
            nombre_de_particules = (unsigned int)nb;
            ImGui::InputInt("Nombre de particules 2",&nb2);
            nombre_de_particules2 = (unsigned int)nb2;
        if (ImGui::Button("validation des conditions initiales")){
            initial=false;}
        }
        ImGui::End();

        //Affiche ImGui
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        GLCall(glfwSwapBuffers(window));
        GLCall(glfwPollEvents());
    }

     // Initialiqation des deux liquides

    Ensemble fluide = Ensemble(nombre_de_particules,rayon_collision);
    Ensemble fluide2 = Ensemble(nombre_de_particules2,rayon_collision);
    
    //Tableau pour tracer tous les point en un seul "draw"

    unsigned int* indices_fluide = (unsigned int*)malloc(nombre_de_particules*6*sizeof(unsigned int));
    unsigned int* indices_fluide2 = (unsigned int*)malloc(nombre_de_particules2*6*sizeof(unsigned int));

    rempli_indice(indices_fluide,nombre_de_particules);
    rempli_indice(indices_fluide2,nombre_de_particules2);

    //Par point : xpoint,ypoint,xparticule,yparticule,R,G,B,A,TexCoordx,TexCoordy,TexID,rayon_collision²,rayon_influence²
    //4 points par particule
    float* info_points = (float*)malloc(nombre_de_particules*4*13*sizeof(float));
    float* info_points2 = (float*)malloc(nombre_de_particules2*4*13*sizeof(float));

    // idem pour la densite
    unsigned int* indices_densite = (unsigned int*)malloc(resolution_densite*resolution_densite*6*sizeof(unsigned int));
    rempli_indice(indices_densite,resolution_densite*resolution_densite);
    float* info_densite = (float*)malloc(resolution_densite*resolution_densite*4*5*sizeof(float));


    


   


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        ImGui_ImplGlfwGL3_NewFrame();
        // actualise les constante à l'aide d'ImGui
        fenetre_principale("dodo", &sourisx, &sourisy, &clique_gauche, &clique_droit, &a_key, &z_key, &e_key, &q_key, &s_key, &d_key, &dt,
 &rayon_influence, &vx_boite, &vy_boite, &rayon_action_autour_curseur, &puissance_action_autour_curseur, &sens_action_clique_gauche,
 &affiche_densite, &flou, &pause_change, &pause, &opacite, &affiche_vitesses_colorees, &vitesse_caracteristique, &logdt, &logpacg, &logvc);

        fenetre_liquide("fluide 1", &rayon_collision, &g, &masse, &multiplicateur_pression, &multiplicateur_pression_proche, &densite_visee,
 &coeff_amorti, &coeff_viscosite, &coeff_adherence, &logg, &logmp, &logmpp, &logvisc,&texture_debut,&texture_fin);
        if (nombre_de_particules2>0){
            fenetre_liquide("fluide 2", &rayon_collision2, &g2, &masse2, &multiplicateur_pression2, &multiplicateur_pression_proche2, &densite_visee2,
 &coeff_amorti2, &coeff_viscosite2, &coeff_adherence2, &logg2, &logmp2, &logmpp2, &logvisc2,&texture_debut2,&texture_fin2);
            fenetre_interaction( "mélange", &densite_visee_melange, &pression_melange,  &logpmel, &logppmel, &pression_proche_melange, &viscosite_melange, &logviscmel);
        }

        // calcul et affichage de la densitée si l'option est activée
        if (affiche_densite){

            position_couleur_densite(&fluide,&fluide2,info_densite,densite_visee,resolution_densite);

            densite_shader.Bind();            
            VertexArray va_d;                
            VertexBuffer vb_d(info_densite,4*5*resolution_densite*resolution_densite*sizeof(float));
            VertexBufferLayout layout_d;
            layout_d.Push<float>(2);
            layout_d.Push<float>(3);
            va_d.AddBuffer(vb_d,layout_d);
            IndexBuffer ib_d(indices_densite,6*resolution_densite*resolution_densite);
            renderer.Draw(va_d,ib_d,densite_shader);
            
   
        }
        
        // rempli les informations pour l'affichages des particules, Par point : xpoint,ypoint,xparticule,yparticule,R,G,B,A,TexCoordx,TexCoordy,TexID,rayon_collision²,rayon_influence²
        fluide.rempli_info_point(info_points,vitesse_caracteristique,opacite,affiche_vitesses_colorees, texture_debut,texture_fin);
        fluide2.rempli_info_point(info_points2,vitesse_caracteristique,opacite,affiche_vitesses_colorees,texture_debut2,texture_fin2);

        fluide_shader.Bind();
        fluide_shader.SetUniform32i("u_Texture",TexID);  
        fluide_shader.SetUniformMat4f("u_MVP",mvp);

        //Par point : xpoint,ypoint,xparticule,yparticule,R,G,B,A,TexCoordx,TexCoordy,TexID,rayon_collision²,rayon_influence²
        VertexArray va;                
        VertexBuffer vb(info_points,13*4*nombre_de_particules*sizeof(float));
        VertexBufferLayout layout;

        layout.Push<float>(2);
        layout.Push<float>(2);
        layout.Push<float>(4);
        layout.Push<float>(2);
        layout.Push<float>(1);
        layout.Push<float>(2);

        va.AddBuffer(vb,layout);
        IndexBuffer ib(indices_fluide,6*fluide.nombre_de_particules);
        renderer.Draw(va,ib,fluide_shader);



        //Fluide 2
        fluide_shader.Bind();
        fluide_shader.SetUniform32i("u_Texture",TexID);  
        fluide_shader.SetUniformMat4f("u_MVP",mvp);

        //Par point : xpoint,ypoint,xparticule,yparticule,R,G,B,A,TexCoordx,TexCoordy,TexID,rayon_collision²,rayon_influence²
        VertexArray va2;                
        VertexBuffer vb2(info_points2,13*4*nombre_de_particules2*sizeof(float));
        VertexBufferLayout layout2;

        layout2.Push<float>(2);
        layout2.Push<float>(2);
        layout2.Push<float>(4);
        layout2.Push<float>(2);
        layout2.Push<float>(1);
        layout2.Push<float>(2);

        va2.AddBuffer(vb2,layout2);
        IndexBuffer ib2(indices_fluide2,6*fluide2.nombre_de_particules);
        renderer.Draw(va2,ib2,fluide_shader);



        if (! pause){
            fluide.actualise_constantes(rayon_collision,g,masse,multiplicateur_pression,multiplicateur_pression_proche,densite_visee,dt,rayon_influence,coeff_amorti,coeff_viscosite,sourisx,sourisy,rayon_action_autour_curseur,puissance_action_autour_curseur,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
            fluide2.actualise_constantes(rayon_collision2,g2,masse2,multiplicateur_pression2,multiplicateur_pression_proche2,densite_visee2,dt,rayon_influence,coeff_amorti2,coeff_viscosite2,sourisx,sourisy,rayon_action_autour_curseur,puissance_action_autour_curseur,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
            interaction(&fluide,&fluide2,pression_melange,pression_proche_melange,densite_visee_melange,viscosite_melange);
            fluide2.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);
            fluide.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);
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

    free(indices_fluide);free(indices_fluide2);free(info_points);free(info_points2);free(indices_densite);free(info_densite);
    return(0);

}
