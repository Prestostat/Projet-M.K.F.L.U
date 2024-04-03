#shader vertex
    #version 330 core
    
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec2 texCoord;
    out vec2 v_TexCoord;
    uniform mat4 u_MVP;
    void main()
    { 
      gl_Position =u_MVP * position; 
      v_TexCoord = texCoord;
    };


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
uniform vec4 u_color;
uniform sampler2D u_Texture;
uniform vec4 information;

void main()
{   
    vec2 pixelCoords = gl_FragCoord.xy;
    float dst=((pixelCoords.x/960.0) - information.z) * ((pixelCoords.x/960.0) - information.z) +((pixelCoords.y/960.0) - information.w) * ((pixelCoords.y/960.0) - information.w) ;
    
    
    vec4 texColor = texture(u_Texture,v_TexCoord);
    color= texColor;
    color.w=0.4;
    
};