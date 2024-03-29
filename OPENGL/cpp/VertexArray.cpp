#include "../header/VertexArray.h"
#include "../header/Renderer.h"
#include "../header/VertexBufferLayout.h"

VertexArray::VertexArray(){
    GLCall(glGenVertexArrays(1,&m_RendererID));
    
}

VertexArray::~VertexArray(){
    GLCall(glDeleteVertexArrays(1,&m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElement();
    unsigned int offset =0;
    for (unsigned int i=0; i<elements.size();i++){
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i,element.count,element.type,element.normalized,layout.GetStride(),(const void*)offset)); // buffer Layout abstraction in OpenGL
        offset+=element.count*VertexBufferElement::GetSizeOfType(element.type);

    }
    
}
void VertexArray::Bind() const{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::UnBind() const{
    GLCall(glBindVertexArray(0));
}