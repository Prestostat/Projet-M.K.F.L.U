#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer {
    private:
    unsigned int m_Renderer_ID;
    unsigned int m_count;
    public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void Bind() const ;
    void Unbind() const; 
    inline unsigned int GetCount() const {return m_count;}

};
#endif