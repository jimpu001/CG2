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
	int size=int(pp.boxSize_e_e_e.x)/2;
	vec2 delta=vec2(1.0)/vec2(textureSize(tex,0));
	ivec2 pos;
	vec4 acc=vec4(0);

	for (pos.y = -size; pos.y <= size; pos.y++) {
		for (pos.x = -size; pos.x <= size; pos.x++) {
			acc += textureLod(tex, v_texcoord + vec2(pos) * delta, 0);
		}
	}
	frag_color = acc/(pp.boxSize_e_e_e.x*pp.boxSize_e_e_e.x);
}
