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






int main(){
    float rayon = 0.01f;
    unsigned int nombre_de_points =10; // doit être supérieur a 3 (pour au moins avoir 1 triangle)
    unsigned int nombre_de_triangles = nombre_de_points-2;
    unsigned int nombre_de_particules =500;
    Ensemble fluide = Ensemble(nombre_de_particules,rayon);
    static float g = 0.0f;


    




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
    Particule test1;
    test1.inirapide();
    Particule test2;
    test2.initialise_particules(0.5,0.5,0,0,0.2);

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
            
            static int counter = 0;
            ImGui::Text("méca flu :sunglassess: ");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("gravité", &g, -1000000.0f, 1000000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("multiplicateur de manque d'intelligence = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        for (unsigned int i=0;i<nombre_de_particules;i++){
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


        fluide.evolution(0.00001,g, 0.07); // dt,g,rayon_influence
        


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