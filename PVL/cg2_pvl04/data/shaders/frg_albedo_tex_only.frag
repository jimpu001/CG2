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

layout(binding = 0) uniform sampler2D albedo_map;

vec4 get_albedo()
{
   return texture(albedo_map,varying_in.texcoord);
}

