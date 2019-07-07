#version 430 core


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
	vec4 color = texture(tex, v_texcoord);

	// apply post-processing effects to color.rgb, keep original alpha
        frag_color.a = color.a;

        color.rgb = vec3(pp.brightness_contrast_gamma_saturation.x) + color.rgb * pp.brightness_contrast_gamma_saturation.y;
        color.rgb = pow(color.rgb, vec3(pp.brightness_contrast_gamma_saturation.z));

        float luminance = dot(vec3(0.2126, 0.7152, 0.0722), color.rgb);
        color.rgb = mix(vec3(luminance), color.rgb, pp.brightness_contrast_gamma_saturation.w);

        frag_color.rgb = color.rgb;
}
