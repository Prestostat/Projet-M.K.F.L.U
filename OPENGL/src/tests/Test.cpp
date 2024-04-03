#include "test.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw_gl3.h"
namespace test
{
    TestMenu::TestMenu(Test*& currentTestPointer)
        :m_CurrentTest (currentTestPointer)
    {

    }
    void TestMenu::OnImGuiRender()
    {
        for (auto& test : m_Tests){
            if (ImGui::Button(test.first.c_str())) {
                m_CurrentTest = test.second();
            }
        }
    }

    
}
