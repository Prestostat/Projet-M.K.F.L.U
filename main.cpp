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

#define PI 3.14159265
using namespace std;

float maxi(float a,float b){
    if(a<b){return(b);}
    else{return(a);}
}
float mini(float a,float b){
    if(a>b){return(b);}
    else{return(a);}
}



int main(){
    unsigned int nombre_de_points =3; // doit être supérieur a 3 (pour au moins avoir 1 triangle)
    unsigned int nombre_de_triangles = nombre_de_points-2;
    unsigned int nombre_de_particules =5000;
    int affiche_densité =0;
    int resolution_densite = 100;

    map<string,float> constantes;
    constantes.insert({"rayon_affichage",0.005});
    constantes.insert({"g",6});
    constantes.insert({"masse",1});
    constantes.insert({"multiplicateur_pression",7});
    constantes.insert({"multiplicateur_pression_proche",7});
    constantes.insert({"densite_visee",1000});
    constantes.insert({"dt",5});
    constantes.insert({"rayon_influence",0.1});
    constantes.insert({"coeff_amorti",1});
    constantes.insert({"viscstrength",10});
    constantes.insert({"sourisx",0});
    constantes.insert({"sourisy",0});
    constantes.insert({"rayon_action-clique_gauche",0.5});
    constantes.insert({"puissance-action_clique_gauche",7});

    map<string,bool> controles;
    controles.insert({"clique_gauche",false});
    controles.insert({"clique_droit",false});

    Ensemble fluide = Ensemble(nombre_de_particules,constantes);


    




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
    unsigned int indices_densite[6] = {0,1,2,1,2,3};
    float position_carre[4*2];
    

    Shader shader("OPENGL/res/shaders/Basic.shader");
    Renderer renderer;
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
    bool show_demo_window = true;
    float clear_color[3] = {1.0f,0.0f,0.0f};

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("méca flu :sunglassess: ");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("rayon visuelle de la particule", &(constantes["rayon_affichage"]), 0.0f, 0.05f);
            /*ImGui::SliderFloat("-log(dt)", &, 4.0f, 6.0f);
            ImGui::SliderFloat("rayon_influence", &rayon_influence, 0.0f, 1.0f);
            ImGui::SliderFloat("log(gravité)", &logg, 0.0f, 10.0f);
            ImGui::SliderFloat("log(mulplicateur de pression)", &logmultiplicateur_pression, 0.0f, 10.0f);
            ImGui::SliderFloat("densité visée", &densite_visee, 100.0f, 2000.0f);
            ImGui::SliderFloat("Coef viscosité", &viscstrength, 0.0f, 100.0f);
            ImGui::SliderFloat("coeff d'amortissement", &coeff_amorti, 0.0f, 2.0f);   
            ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state

            if (ImGui::Button("affiche densité"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                affiche_densité++;
            ImGui::SameLine();
            ImGui::Text("multiplicateur de manque d'intelligence = %d", affiche_densité);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            clique_gauche=ImGui::IsMouseClicked(0,true);
            clique_droit=ImGui::IsMouseClicked(1,true);
            sourisx=io.MousePos.x;
            sourisy=io.MousePos.y;*/
        }
        affiche_densité = affiche_densité%2;

        if (affiche_densité==1){
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

                float densité_grille= fluide.densite_ponctuelle(position_carre[0]+pas/2,position_carre[1]+pas/2);
                float r=mini(1.0,maxi(0.0,densité_grille/constantes["densite_visee"] -1));
                float g=mini(0.0,pow(mini(abs(densité_grille/constantes["densite_visee"]),abs(2-densité_grille/constantes["densite_visee"])),4));
                float b=maxi(0.0,1-densité_grille/constantes["densite_visee"]);

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

            
        }
        

        for (unsigned int i=0;i<nombre_de_particules;i++){
            fluide.data[i].rayon = constantes["rayon_affichage"];
            fluide.data[i].position_particule(nombre_de_points,position );
            

            shader.Bind();
            shader.SetUniform4f("u_color",clear_color[0],clear_color[1],clear_color[2],1.0f);            
            VertexArray va;                
            VertexBuffer vb(position,nombre_de_points*2*sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);
            va.AddBuffer(vb,layout);
            IndexBuffer ib(indices,3*nombre_de_triangles);
            renderer.Draw(va,ib,shader);
        }

        






        //cout << fluide.data[0].x << endl;
        //fluide.evolution(pow(10,-const),pow(10,logg), rayon_influence,masse,pow(10,logmultiplicateur_pression),multiplicateur_pression_proche,densite_visee,coeff_amorti,viscstrength,clique_gauche,clique_droit,sourisx,sourisy,rayon_action_clique_gauche,puissance_action); //float dt, float g,float rayon_influence, float m,float multipression,float dvisee,float coef
        fluide.evolution(constantes,controles);


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