#shader vertex
    #version 330 core
    
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec2 texCoord;
    layout(location = 2) in vec3 color;
    out vec2 v_TexCoord;
    out vec3 v_color;
    
    void main()
    { 
      gl_Position =position; 
      v_TexCoord = texCoord;
    };


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
in vec3 v_color;
uniform vec4 u_color;
uniform sampler2D u_Texture;

void main()
{   
    vec4 texColor = texture(u_Texture,v_TexCoord);
    color.xyz= v_color;
    color.w=u_color.w;
    
};