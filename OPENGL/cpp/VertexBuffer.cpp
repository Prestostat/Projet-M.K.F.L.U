#include "../header/Renderer.h"
#include "../header/VertexBuffer.h"



VertexBuffer::VertexBuffer(const void* data, unsigned int size){
    GLCall(glGenBuffers(1,&m_Renderer_ID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,m_Renderer_ID));
    GLCall(glBufferData(GL_ARRAY_BUFFER,size, data, GL_DYNAMIC_DRAW)); //GL_STATIC_DRAW
}

VertexBuffer::~VertexBuffer(){
    GLCall(glDeleteBuffers(1,&m_Renderer_ID));
}


void VertexBuffer::Bind() const {
     GLCall(glBindBuffer(GL_ARRAY_BUFFER,m_Renderer_ID));
}
void VertexBuffer::Unbind() const {
     GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
}