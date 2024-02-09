#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "../header/VertexBuffer.h"
class VertexBufferLayout;
class VertexArray {
private :
    unsigned int m_RendererID;


public:
VertexArray();
~VertexArray();


void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
void Bind() const;
void UnBind() const;

};

#endif
