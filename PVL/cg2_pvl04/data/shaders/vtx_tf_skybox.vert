#version 430 core

layout(std140,binding = 1) uniform cameraDataBlock
{
        mat4 view;
        mat4 inv_view;
        mat4 projection;
        mat4 inv_projection;
        vec4 near_far_width_height;
} camera;


layout(std140,binding = 2) uniform objectDataBlock
{
        mat4 model;
        mat4 normal;

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

const vec4 poses[4] = { vec4(-1,-1,1,1),
                        vec4( 1,-1,1,1),
                        vec4(-1, 1,1,1),
                        vec4( 1, 1,1,1)
                      };

void transfrom()
{
    vec4 v = camera.inv_projection * poses[gl_VertexID];
    v.xyz = v.xyz/v.w;
    v = camera.inv_view* v;

    varying_out.view_ray_ws = v.xyz - camera.inv_view[3].xyz;
    varying_out.position_ws = v.xyz;
    varying_out.nrm_ws = -normalize(varying_out.view_ray_ws);
}


vec4 transfrom_position_cs()
{
    return poses[gl_VertexID];
}
