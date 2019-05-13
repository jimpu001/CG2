#version 430 core

// Input varying
in vec3 v_view_ray_ws;


// Output fragment color
out vec4 fclr;

layout(binding = 4)
uniform samplerCube skybox;


void main()
{
    fclr= texture(skybox,v_view_ray_ws);
}



