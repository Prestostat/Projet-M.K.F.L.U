#ifndef VERTEXBUFFER_H 
#define VERTEXBUFFER_H

class VertexBuffer {
    private:
    unsigned int m_Renderer_ID;
    public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const ; 

};
#endif