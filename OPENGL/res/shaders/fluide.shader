#shader vertex
    #version 330 core
    
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec2 pos_carre;
    layout(location = 2) in vec4 color;
    layout(location = 3) in vec2 texCoord;
    layout(location = 4) in float TexID;
    layout(location = 5) in vec2 rayons2;
    out vec2 v_pos_carre;
    out vec2 v_rayons2;
    out vec2 v_TexCoord;
    out vec4 v_color;
    flat out int v_TexID;
    uniform mat4 u_MVP;
    void main()
    { 
      gl_Position =u_MVP *position; 
      v_TexCoord = texCoord;
      v_pos_carre= pos_carre;
      v_rayons2=rayons2;
      v_color = color;
      v_TexID=int(TexID);
  
    };  


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_pos_carre;
in vec2 v_rayons2;
in vec2 v_TexCoord;
in vec4 v_color;
flat in int v_TexID; // Utilisez flat pour v_TexID

uniform sampler2D u_Texture[32];

void main()
{   
    vec2 pixelCoords = gl_FragCoord.xy;
    float dst2 = ((pixelCoords.x / 960.0) - 0.5 * (v_pos_carre.x + 1)) * ((pixelCoords.x / 960.0) - 0.5 * (v_pos_carre.x + 1)) +
                 ((pixelCoords.y / 960.0) - 0.5 * (v_pos_carre.y + 1)) * ((pixelCoords.y / 960.0) - 0.5 * (v_pos_carre.y + 1));
  
    if (v_TexID > 0.5) {
        if (dst2 > v_rayons2.y) {
            discard;
        }
        vec4 texColor;
            
        if(v_TexID==1){ texColor= texture(u_Texture[0], v_TexCoord);}
        else if(v_TexID==2){ texColor = texture(u_Texture[1], v_TexCoord);}
        else if(v_TexID==3){ texColor = texture(u_Texture[2], v_TexCoord);}
        else if(v_TexID==4){ texColor = texture(u_Texture[3], v_TexCoord);}
        else if(v_TexID==5){ texColor = texture(u_Texture[4], v_TexCoord);}
        else if(v_TexID==6){ texColor = texture(u_Texture[5], v_TexCoord);}
        else if(v_TexID==7){ texColor = texture(u_Texture[6], v_TexCoord);}
        else if(v_TexID==8){ texColor = texture(u_Texture[7], v_TexCoord);}

      
        //vec4 texColor = texture(u_Texture[1], v_TexCoord); // Utilisez TexID comme indice de texture
        color = texColor;
        color.w = v_color.w;
    } 
    else {
        if (dst2 > v_rayons2.x) {
            discard;
        }
        color = v_color;
    }  
}
