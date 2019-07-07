#version 430 core

layout(early_fragment_tests) in;

in vec2 v_texcoord;

layout(location = 0) out vec4 frag_color;

layout(binding=0) uniform sampler2D tex;
layout(binding=1) uniform sampler2D depthBuffer;

layout(std140,binding = 1) uniform cameraDataBlock
{
        mat4 view;
        mat4 inv_view;
        mat4 projection;
        mat4 inv_projection;
        vec4 near_far_width_height;
} camera;

layout(std140,binding = 6) uniform postProcessingDataBlock
{
    vec4 brightness_contrast_gamma_saturation;
    vec4 boxSize_e_e_e;
    vec4 direction_level_e;
    vec4 focalDistance_focalSize_maxLevel_e;
    vec4 viewport;
    vec4 ghost;
    vec4 ghost_center_radius;
} pp;

float linearize_depth(float z_win)
{
        float n=camera.near_far_width_height.x;
        float f=camera.near_far_width_height.y;

	return (n*f) / (z_win * (f-n) -f);
}

void main()
{
        float z_eye=linearize_depth(texture(depthBuffer, v_texcoord).r);
        float dist = abs(z_eye + pp.focalDistance_focalSize_maxLevel_e.x);
        float factor = min(dist / pp.focalDistance_focalSize_maxLevel_e.y, pp.focalDistance_focalSize_maxLevel_e.z);
        frag_color=textureLod(tex, v_texcoord, factor);
}
