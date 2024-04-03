#ifndef TEST_TEXTURE2D_H
#define TEST_TEXTURE2D_H
#include "test.h"
#include <GL/glew.h>
#include "../../header/Renderer.h"
#include "../vendor/imgui/imgui.h"
#include "../../header/VertexBuffer.h"
#include "../../header/IndexBuffer.h"
#include "../../header/VertexArray.h"
#include "../../header/Shader.h"
//#include "../vendor/imgui/imgui_impl_glfw_gl3.h"
#include "../../header/VertexBufferLayout.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../../header/texture.h"
#include "../vendor/stb_image/stb_image.h"
#include <memory>
namespace test {
    class TestTexture2D : public Test
    {
        public:
        TestTexture2D();
        ~TestTexture2D();

        void OnUpdate(float deltaTime) override;
        void OnRender(Ensemble* fluide) override;
        void OnImGuiRender() override;
        private:
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Texture> m_Texture;
        glm::mat4 m_Proj,m_View;
        glm::vec3 m_TranslationA,m_TranslationB;

    };
}
 



#endif