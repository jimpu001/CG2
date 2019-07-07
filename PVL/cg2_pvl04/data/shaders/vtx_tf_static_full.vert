#version 430 core

layout(std140,binding = 1) uniform cameraDataBlock
{
    mat4 view;
    mat4 inv_view;
    mat4 projection;
    mat4 inv_projection;
    vec4 near_far_width_height;
    vec4 position;
} camera;

layout(std140,binding = 2) uniform objectDataBlock
{
    mat4 model;
    mat4 normal_matrix;
} object;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 texcoord;
layout(location = 5) in vec4 color;
layout(location = 6) in vec4 boneWeights;
layout(location = 7) in uvec4 boneIDs;
layout(location = 8) in uint boneCnt;

out VertexData{
    vec3 position_os;
    vec3 position_ws;
    vec3 tangent_ws;
    vec3 bitangent_ws;
    vec3 nrm_ws;
    vec2 texcoord;
    vec4 color;
    vec3 view_ray_ws;
} varying_out;

void transfrom()
{
    varying_out.position_os			= position;
    varying_out.position_ws			= vec3(object.model * vec4(position, 1.0)).xyz;
    varying_out.view_ray_ws			= varying_out.position_ws.xyz - camera.inv_view[3].xyz;
    varying_out.tangent_ws			= normalize(mat3(object.model) * (tangent));
    varying_out.bitangent_ws			= normalize(mat3(object.model) * (bitangent));
    varying_out.nrm_ws				= normalize(mat3(object.normal_matrix) * normal);
    varying_out.texcoord			= texcoord;
    varying_out.color                           = color;
}

vec4 transfrom_position_cs()
{
    return camera.projection * camera.view * object.model * vec4(position, 1.0);
}
