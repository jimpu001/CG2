#version 430 core

//in vec3 v_position_ws;
//in vec3 v_tangent_ws;
//in vec3 v_bitangent_ws;
//in vec3 v_nrm_ws;
//in vec2 v_texcoord;
//in vec4 v_color;
// layout(binding = 2) uniform sampler2D mp_map;

layout(std140,binding = 3) uniform materialDataBlock
{
    vec4 albedo;
    vec4 properties;
} material;



vec4 get_material_props()
{
    return vec4(0.5);
}

