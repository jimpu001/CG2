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


layout(std140, binding = 3) uniform materialDataBlock
{
    vec4 albedo;
    vec4 properties;
    vec4 properties_scale;
    vec4 properties_offset;
} material;

layout(binding = 2) uniform sampler2D mp_map;


vec4 get_material_props()
{

    vec4 mp = texture(mp_map,varying_in.texcoord);
    return mp*material.properties_scale+material.properties_offset;
}

