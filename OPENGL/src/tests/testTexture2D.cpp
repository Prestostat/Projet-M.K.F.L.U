#include "testTexture2D.h"
#include "test.h"

#include <GL/glew.h>

#include "../../header/Renderer.h"
#include "../../header/VertexBuffer.h"
#include "../../header/IndexBuffer.h"
#include "../../header/VertexArray.h"
#include "../../header/Shader.h"
#include "../../header/VertexBufferLayout.h"
#include "../../header/texture.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"

#include <memory>

namespace test {
    TestTexture2D::TestTexture2D()
        : m_Proj(glm::ortho(-1,1,-1,1,-1,1)) , m_View(glm::translate(glm::mat4(1.0),glm::vec3(0,0,0))), m_TranslationA(0,0,0),m_TranslationB(0.5,0,0) 
    {
        float positions[] = {
            -0.5,-0.5,0,0,0.5,0.5,0.5,
            -0.5,0.5,0,1,0.5,0.5,0.5,
            0.5,-0.5,1,0,0.5,0.5,0.5,
            0.5,0.5,1,1,0.5,0.5,0.5};

        unsigned int indices[] = {0,1,2,1,2,3};

        
        m_VAO = std::make_unique<VertexArray>();
        m_VertexBuffer =  std::make_unique<VertexBuffer>(positions,4*7*sizeof(float));
        
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        layout.Push<float>(3);

        m_VAO->AddBuffer(*m_VertexBuffer,layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices,6);

        m_Shader = std::make_unique<Shader>("OPENGL/res/shaders/texrond.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_color",0.2,0.3,0.7,1);

        m_Texture = std::make_unique<Texture>("OPENGL/res/textures/Titouan.png");
        m_Shader->SetUniform1i("u_Texture",0);
 
    }
    TestTexture2D::~TestTexture2D(){

    }
    void TestTexture2D::OnUpdate(float deltaTime){

    }

    void TestTexture2D::OnRender(Ensemble* fluide)
    {
        GLCall(glClearColor(0,0,0,0.5));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        Renderer renderer;
        
        {
            glm::mat4 model = glm::translate(glm::mat4(1),m_TranslationA);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP",mvp);
            renderer.Draw(*m_VAO,*m_IndexBuffer,*m_Shader);
        }
        {
            glm::mat4 model = glm::translate(glm::mat4(1),m_TranslationB);
            glm::mat4 mvp =m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP",mvp);
            renderer.Draw(*m_VAO,*m_IndexBuffer,*m_Shader);
        }
    }

    void TestTexture2D::OnImGuiRender(){
        ImGui::SliderFloat3("Translation A",&m_TranslationA.x,-1,1);
        ImGui::SliderFloat3("Translation B",&m_TranslationB.x,-1,1);

        
    }

}

