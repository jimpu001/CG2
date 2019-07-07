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

layout(binding = 1) uniform sampler2D normal_map;


vec3 get_normal()
{

    mat3 tangent2WS = mat3(normalize(varying_in.tangent_ws),
                           normalize(varying_in.bitangent_ws),
                           normalize(varying_in.nrm_ws)
                           );




    return normalize(tangent2WS * ((2.0 * (texture(normal_map, varying_in.texcoord).rgb) - vec3(1.0))));

}

