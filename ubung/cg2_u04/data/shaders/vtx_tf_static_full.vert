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
        mat4 normal_matrix;

} object;



// All the possible attributes.
layout(location = 0) in vec3 position_os;
layout(location = 1) in vec3 normal_os;
layout(location = 2) in vec3 tangent_os;
layout(location = 3) in vec3 bitangent_os;
layout(location = 4) in vec2 texcoord;
layout(location = 5) in vec4 color;
layout(location = 6) in vec4 boneWeights;
layout(location = 7) in uvec4 boneIDs;
layout(location = 8) in uint boneCnt;


// The position in Object Space
out vec3 v_position_os;
// The position in World Space
out vec3 v_position_ws;
// The tangent in World Space
out vec3 v_tangent_ws;
// The bitangent in World Space
out vec3 v_bitangent_ws;
// The normal in World Space
out vec3 v_nrm_ws;
// The texture coord
out vec2 v_texcoord;
// The vertex color
out vec4 v_color;
// The viewing rays direction in World Space
out vec3 v_view_ray_ws;


void transfrom_varyings()
{
    // 1 (a) Transform all the in their appropriate spaces.
    //     v_position_os = ...;
    //     v_position_ws = ...;
    //     v_nrm_ws = ...;
    //     v_texcoord = ...;
    //     v_color = ...;
}

vec4 transfrom_position_cs()
{
    // 1 (a) Return the position in Clip Space
    // return ...

    return vec4(position_os,1.0);

}
