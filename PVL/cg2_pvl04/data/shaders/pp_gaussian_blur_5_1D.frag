#version 430 core

layout(early_fragment_tests) in;

in vec2 v_texcoord;

layout(location = 0) out vec4 frag_color;

layout(binding=0) uniform sampler2D tex;

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

void main()
{

    vec2 direction = pp.direction_level_e.xy * vec2(1.0)/vec2(textureSize(tex,int(pp.direction_level_e.z)));
        frag_color =
                (5.0/16.0f) * textureLod(tex, v_texcoord -1.2 * direction, pp.direction_level_e.z) +
		(6.0/16.0f) * textureLod(tex, v_texcoord, pp.direction_level_e.z) +
                (5.0/16.0f) * textureLod(tex, v_texcoord +1.2 * direction, pp.direction_level_e.z);
}
