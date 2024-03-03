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


 void fenetre_ImGui(float* rayon_affichage,float* g,float* logg,float* multiplicateur_pression,float* logmp,float* multiplicateur_pression_proche,float* logmpp,float* densite_visee,float* dt, float* logdt,float* rayon_influence,float*coeff_amorti,float*viscstrength,float* sourisx,float* sourisy,float* rayon_action_clique_gauche,float* puissance_action_clique_gauche,int* sens_action_clique_gauche,float* logpacg,float* coeff_adherence,float* logvc ,float* vitesse_caracteristique,float* transparence,bool* flou,bool* clique_gauche,bool* clique_droit,bool* a,bool*z,bool* e,bool* q,bool* s, bool* d, bool* pause){
    ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("méca flu :sunglassess: ");     
            ImGui::SliderFloat("rayon visuelle de la particule", (rayon_affichage), 0.0f, 0.05f);
            ImGui::SliderFloat("rayon action clique gauche", (rayon_action_clique_gauche), 0.0f, 0.5f);
            ImGui::SliderFloat("ln(dt)", logdt, -7.0, 0.0f);
            *dt=pow(E,*logdt);
            ImGui::SliderFloat("rayon_influence", rayon_influence, 0.0f, 1.0f);
            ImGui::SliderFloat("ln(gravité)", logg, 0.0f, 10.0f);
            *g=pow(E,*logg);
            ImGui::SliderFloat("ln(mulplicateur de pression)", logmp, 0.0f, 10.0f);
            *multiplicateur_pression=pow(E,*logmp);
            ImGui::SliderFloat("ln(mulplicateur de pression_proche)", logmpp, 0.0f, 10.0f);
            *multiplicateur_pression_proche=pow(E,*logmpp);
            ImGui::SliderFloat("densité visée", densite_visee, 0.0f, 5000.0f);
            ImGui::SliderFloat("Coef viscosité", viscstrength, 0.0f, 10.0f);
            ImGui::SliderFloat("ln(force_clique_gauche)", logpacg, 0.0f, 5.0f);
            *puissance_action_clique_gauche=pow(E,*logpacg)* *sens_action_clique_gauche;
            ImGui::SliderFloat("coeff d'amortissement", coeff_amorti, 0.0f, 2.0f);  
            ImGui::SliderFloat("coeff d'adhérence aux parois", coeff_adherence, 0.0f, 1000.0f);
            ImGui::SliderFloat("transparence", transparence, 0.0f, 100.0f);  
            ImGui::SliderFloat("ln(vitesse_caracteristique)", logvc, -2.0f, 2.0f);
            *vitesse_caracteristique=pow(E,*logvc);
            if (ImGui::Button("sens de la force du clique gauche"))
                *sens_action_clique_gauche*=-1;
            if (ImGui::Button("activer le flou")){
                *flou = ! *flou;} 
            ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("espace : pause "); 
            ImGui::Text("clique droit : met toutes les vitesses à 0 "); 
            ImGui::Text("clique gauche : force centrée sur la souris");
            ImGui::Text("a : attraction centrée sur la souris avec les multiplicateurs du clique gauche");  
            ImGui::Text("e : répulsion centrée sur la souris avec les multiplicateurs du clique gauche"); 
            *clique_gauche=ImGui::IsMouseClicked(0,true);
            *clique_droit=ImGui::IsMouseClicked(1,true);
            *a=io.KeysDown['Q']; // attention, c'est en Qwerty 
            *z=io.KeysDown['W']; 
            *e=io.KeysDown['E'];
            *q=io.KeysDown['A'];
            *s=io.KeysDown['S'];
            *d=io.KeysDown['D'];
            if (io.KeysDown[' ']) {*pause = !*pause; std::this_thread::sleep_for(std::chrono::milliseconds(100));}

            *sourisx=io.MousePos.x;
            *sourisy=io.MousePos.y;
        }

 }


int main(){
    

    unsigned int nombre_de_points =3; // doit être supérieur a 3 (pour au moins avoir 1 triangle)
    unsigned int nombre_de_triangles = nombre_de_points-2;
    unsigned int nombre_de_particules =1000;
    //bool affiche_densité =false;
    //int resolution_densite = 100;
    int sens_action_clique_gauche=1;

    float rayon_affichage = 0.005;
    float g = 10;
    float masse = 1;
    float multiplicateur_pression = 100;
    float multiplicateur_pression_proche = 10000;
    float densite_visee = 1200;
    float dt =0.005;
    float rayon_influence =0.15;
    float coeff_amorti = 0.9;
    float viscstrength = 0.1;
    float sourisx = 0;
    float sourisy = 0;
    float rayon_action_clique_gauche =0.5;
    float puissance_action_clique_gauche = 100;
    float transparence = 10;
    float vitesse_caracteristique =1;
    float vx_boite = 0;
    float vy_boite = 0;
    float coeff_adherence = 0.05;

    bool clique_gauche=false;
    bool clique_droit=false;
    bool a_key=false;
    bool z_key=false;
    bool e_key=false;
    bool q_key=false;
    bool s_key=false;
    bool d_key=false;
    bool pause=false;
    bool flou = false;


    

    float logg = log(g);
    float logmp =log(multiplicateur_pression);
    float logmpp = log(multiplicateur_pression_proche);
    float logdt = log(dt);
    float logpacg = log(puissance_action_clique_gauche);
    float logvc = log(vitesse_caracteristique);
    Ensemble fluide = Ensemble(nombre_de_particules,rayon_affichage);

    unsigned int nombre_de_particules2 =0;
    Ensemble fluide2 = Ensemble(nombre_de_particules2,rayon_affichage);
    

    


    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,5);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 960, "Méca flu :sunglasses:", NULL, NULL);
    
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








    unsigned int indices[nombre_de_triangles*3];
    for (unsigned int i=0; i<nombre_de_triangles;i++) {
        indices[3*i]=0;
        indices[3*i+1]=i+1;
        indices[3*i+2]=i+2;
    }
    float position[nombre_de_points*2];
    //unsigned int indices_densite[6] = {0,1,2,1,2,3};
    //float position_carre[4*2];
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

    Shader shader("OPENGL/res/shaders/Basic.shader");
    Renderer renderer;
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
    float* clear_color = (float*)malloc(3*sizeof(float));
    clear_color[0]=1;
    clear_color[1]=0;
    clear_color[2]=0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        fenetre_ImGui(&rayon_affichage,&g, &logg, &multiplicateur_pression, &logmp, &multiplicateur_pression_proche, &logmpp, &densite_visee, &dt,  &logdt, &rayon_influence, &coeff_amorti, &viscstrength, &sourisx, &sourisy, &rayon_action_clique_gauche, &puissance_action_clique_gauche,&sens_action_clique_gauche, &logpacg,&coeff_adherence, &logvc , &vitesse_caracteristique,&transparence,&flou, &clique_gauche, &clique_droit, &a_key, &z_key, &e_key, &q_key, &s_key, &d_key,&pause);
        
        /*
        affiche_densité = affiche_densité%2;

        if (affiche_densité){
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

                float densité_grille= fluide.densite_ponctuelle_visee(position_carre[0]+pas/2,position_carre[1]+pas/2);
                float r=mini(1.0,maxi(0.0,densité_grille/densite_visee -1));
                float g=mini(0.0,pow(mini(abs(densité_grille/densite_visee),abs(2-densité_grille/densite_visee)),4));
                float b=maxi(0.0,1-densité_grille/densite_visee);

                shader.Bind();
                shader.SetUniform4f("u_color",r,g,b,1.0f);            
                VertexArray va;                
                VertexBuffer vb(position_carre,4*2*sizeof(float));
                VertexBufferLayout layout;
                layout.Push<float>(2);
                va.AddBuffer(vb,layout);
                IndexBuffer ib(indices_densite,6);
                renderer.Draw(va,ib,shader);

            }

            
        }*/
        

        for (unsigned int i=0;i<nombre_de_particules;i++){
            fluide.data[i].rayon = rayon_affichage;
            if (flou){
                fluide.data[i].position_particule(nombre_de_points,rayon_influence,position );
            }
            else {
                fluide.data[i].position_particule(nombre_de_points,rayon_affichage,position );
            }
            
            clear_color=fluide.data[i].couleur(vitesse_caracteristique);

            shader.Bind();
            if (flou){
                shader.SetUniform4f("information",transparence/(densite_visee*rayon_influence),rayon_influence/2,(fluide.data[i].x+1)/2,(fluide.data[i].y+1)/2);
            }
            else {
                shader.SetUniform4f("information",10000,rayon_affichage/2,(fluide.data[i].x+1)/2,(fluide.data[i].y+1)/2);
            }
            
            shader.SetUniform4f("u_color",clear_color[0],clear_color[1],clear_color[2],1.0f);
                        
            VertexArray va;                
            VertexBuffer vb(position,nombre_de_points*2*sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);
            va.AddBuffer(vb,layout);
            IndexBuffer ib(indices,3*nombre_de_triangles);
            renderer.Draw(va,ib,shader);
        }


        for (unsigned int i=0;i<nombre_de_particules2;i++){
            fluide2.data[i].rayon = rayon_affichage;
            if (flou){
                fluide2.data[i].position_particule(nombre_de_points,rayon_influence,position );
            }
            else {
                fluide2.data[i].position_particule(nombre_de_points,rayon_affichage,position );
            }
            
            clear_color=fluide2.data[i].couleur(vitesse_caracteristique);

            shader.Bind();
            if (flou){
                shader.SetUniform4f("information",transparence/(densite_visee*rayon_influence),rayon_influence/2,(fluide2.data[i].x+1)/2,(fluide2.data[i].y+1)/2);
            }
            else {
                shader.SetUniform4f("information",10000,rayon_affichage/2,(fluide2.data[i].x+1)/2,(fluide2.data[i].y+1)/2);
            }
            
            shader.SetUniform4f("u_color",0,1,0,1.0f);
                        
            VertexArray va;                
            VertexBuffer vb(position,nombre_de_points*2*sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);
            va.AddBuffer(vb,layout);
            IndexBuffer ib(indices,3*nombre_de_triangles);
            renderer.Draw(va,ib,shader);
        }
        
        fluide2.actualise_constantes(rayon_affichage,g,masse,multiplicateur_pression,multiplicateur_pression_proche,densite_visee,dt,rayon_influence,coeff_amorti,viscstrength,sourisx,sourisy,rayon_action_clique_gauche,puissance_action_clique_gauche,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
        fluide.actualise_constantes(rayon_affichage,g,masse,multiplicateur_pression,multiplicateur_pression_proche,densite_visee,dt,rayon_influence,coeff_amorti,viscstrength,sourisx,sourisy,rayon_action_clique_gauche,puissance_action_clique_gauche,clique_gauche,clique_droit,a_key,z_key,e_key,q_key,s_key,d_key,pause);
        fluide2.actualise_listes();
        fluide.actualise_listes();
        
        fluide2.evolution();
        
        fluide2.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);

        
        fluide.evolution();
        fluide.frottement_paroi(vx_boite, vy_boite, -1, 1, -1, 1,coeff_adherence);

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));
//oui
        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    return(0);

}