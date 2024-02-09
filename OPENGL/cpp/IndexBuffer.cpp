#include "../header/IndexBuffer.h"
#include "../header/Renderer.h"


IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count){
    m_count=count;
    GLCall(glGenBuffers(1,&m_Renderer_ID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_Renderer_ID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,count*sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer(){
    GLCall(glDeleteBuffers(1,&m_Renderer_ID));
}


void IndexBuffer::Bind() const {
     GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_Renderer_ID));
}
void IndexBuffer::Unbind() const{
     GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));
}