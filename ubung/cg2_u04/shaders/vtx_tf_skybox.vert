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


out vec3 v_position_ws;
out vec3 v_tangent_ws;
out vec3 v_bitangent_ws;
out vec3 v_nrm_ws;
out vec2 v_texcoord;
out vec4 v_color;
out vec3 v_view_ray_ws;

const vec4 poses[4] = { vec4(-1,-1,1,1),
                        vec4( 1,-1,1,1),
                        vec4(-1, 1,1,1),
                        vec4( 1, 1,1,1)
                      };

void transfrom_varyings()
{
    vec4 v = (camera.inv_view* camera.inv_projection * poses[gl_VertexID]);
    v.xyz = v.xyz/v.w;
    v_view_ray_ws = v.xyz - camera.inv_view[3].xyz;
    v_position_ws = v.xyz;
    v_nrm_ws = -normalize(v_view_ray_ws);
}


vec4 transfrom_position_cs()
{
    return poses[gl_VertexID];
}
