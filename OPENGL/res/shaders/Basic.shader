#shader vertex
    #version 330 core
    
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec4 informations;
    out vec4 info;
    void main()
    {
      info=informations;
      gl_Position =position; 
    };


#shader fragment

   #version 330 core
    
   layout(location = 0) out vec4 color;
   uniform vec4 u_color;
   in vec4 info;

   

    void main()
    { 

       color= u_color;
    };