#shader vertex
    #version 330 core
    
    layout(location = 0) in vec4 position;
    
    void main()
    { 

      gl_Position =position; 
    };


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 u_color;
uniform vec4 information;

in vec4 info;

void main()
{ 
    // Récupérer les coordonnées du pixel en cours de traitement
    vec2 pixelCoords = gl_FragCoord.xy;
    float dst=((pixelCoords.x/960.0) - information.z) * ((pixelCoords.x/960.0) - information.z) +((pixelCoords.y/960.0) - information.w) * ((pixelCoords.y/960.0) - information.w) ;

    if (dst> information.y*information.y) {
        discard;
    }

    // Utilisation de la couleur uniforme
    color.xyz = u_color.xyz;
    color.w=information.x*(information.y*information.y-dst)*(information.y*information.y-dst)/(information.y*information.y*information.y*information.y);
};