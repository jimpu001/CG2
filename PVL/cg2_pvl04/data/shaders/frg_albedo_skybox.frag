#version 430 core
in VertexData{
    vec3 position_os;
    vec3 position_ws;
    vec3 tangent_ws;
    vec3 bitangent_ws;
    vec3 nrm_ws;
    vec2 texcoord;
    vec4 color;
    vec3 view_ray_ws;
} varying_in;


layout(binding = 4) uniform samplerCube skybox;


vec4 get_albedo()
{
   return texture(skybox,varying_in.view_ray_ws);

}

