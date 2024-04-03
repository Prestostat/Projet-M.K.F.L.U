#include "testClearColor.h"
#include "test.h"
#include <GL/glew.h>
#include "../../header/Renderer.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw_gl3.h"
namespace test {
    TestClearColor::TestClearColor()
    : m_ClearColor{0.2,0.3,0.8,1} 
    {

    }
    TestClearColor::~TestClearColor(){

    }
    void TestClearColor::OnUpdate(float deltaTime){

    }

    void TestClearColor::OnRender(Ensemble* f)
    {
        GLCall(glClearColor(m_ClearColor[0],m_ClearColor[1],m_ClearColor[2],m_ClearColor[3]));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }

    void TestClearColor::OnImGuiRender(){
        ImGui::ColorEdit4("Clear color",m_ClearColor);
    }

}

