#ifndef RENDERER_H
#define RENDERER_H
#include "///usr/include/GL/glew.h"
#include<GLFW/glfw3.h>
#include "../header/IndexBuffer.h"
#include "../header/VertexArray.h"
#include "../header/Shader.h"


#define ASSERT(x) if (!(x)) __builtin_trap();

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__,__LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);



class Renderer{
    public:
    void Clear() const ;
    void Draw(const VertexArray& va,const IndexBuffer&ib, const Shader& shader) const;
    
};

#endif