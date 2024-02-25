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
    float rayon = 0.005f;
    unsigned int nombre_de_points =3; // doit être supérieur a 3 (pour au moins avoir 1 triangle)
    unsigned int nombre_de_triangles = nombre_de_points-2;
    unsigned int nombre_de_particules =2000;
    Ensemble fluide = Ensemble(nombre_de_particules,rayon);
    static float logg = 6.0f;
    float masse = 1;
    float logmultiplicateur_pression = 7;
    float multiplicateur_pression_proche = 10000000000;
    float densite_visee=800;
    float moinslogdt = 5;
    float rayon_influence = 0.15;
    float coeff_amorti = 0.9;
    float viscstrength = 10;
    int affiche_densité =0;
    int resolution_densite = 200;
    bool clique_gauche = false;
    bool clique_droit = false;
    float sourisx=0;
    float sourisy=0;
    float rayon_action_clique_gauche = 0.5; // de la souris gauche
    float puissance_action = -10000000; // de la souris

    




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
            ImGui::SliderFloat("rayon visuelle de la particule", &rayon, 0.0f, 0.05f);
            ImGui::SliderFloat("-log(dt)", &moinslogdt, 4.0f, 6.0f);
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
            sourisy=io.MousePos.y;
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

                float densité_grille= fluide.densite_ponctuelle(position_carre[0]+pas/2,position_carre[1]+pas/2,rayon_influence);
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

            
        }
        

        for (unsigned int i=0;i<nombre_de_particules;i++){
            fluide.data[i].rayon = rayon;
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
        fluide.evolution(pow(10,-moinslogdt),pow(10,logg), rayon_influence,masse,pow(10,logmultiplicateur_pression),multiplicateur_pression_proche,densite_visee,coeff_amorti,viscstrength,
        clique_gauche,clique_droit,sourisx,sourisy,rayon_action_clique_gauche,puissance_action); //float dt, float g,float rayon_influence, float m,float multipression,float dvisee,float coef
        


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