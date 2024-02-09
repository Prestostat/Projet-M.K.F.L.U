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
#define PI 3.14159265
using namespace std;






int main(){
    float x=0.0f;
    float y=0.0f;
    float rayon = 0.2f;
    unsigned int nombre_de_points =20; // doit être supérieur a 3 (pour au moins avoir 1 triangle)
    unsigned int nombre_de_triangles = nombre_de_points-2;
    unsigned int nombre_de_particules =1;

    




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
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        for (unsigned int i=0;i<nombre_de_particules;i++){

            test2.position_particule(nombre_de_points,position );

            shader.Bind();
            shader.SetUniform4f("u_color",1.0f,0.0f,0.0f,1.0f);            
            VertexArray va;                
            VertexBuffer vb(position,nombre_de_points*2*sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(2);
            va.AddBuffer(vb,layout);
            IndexBuffer ib(indices,3*nombre_de_triangles);
            renderer.Draw(va,ib,shader);
        }
        

        

        
                
        
        


        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

}