#version 430 core

in vec3 v_view_ray_ws;
layout(binding = 4) uniform samplerCube skybox;

vec4 get_albedo()
{
   return texture(skybox,v_view_ray_ws);

}

