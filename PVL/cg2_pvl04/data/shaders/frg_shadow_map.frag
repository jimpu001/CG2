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

layout(binding = 5) uniform sampler2DShadow shadow_map;

struct Light
{
    vec4 position;
    vec4 color_AND_ambi;
    vec4 spot_dir_AND_cut_off;
    vec4 spot_exponent_AND_size_AND_enabled; // z  unused!
    mat4 worldSpaceToShadowMap;
};

layout(std140, binding = 4) uniform globalLightDataBlock
{
    Light lights[8];
#define Lpos(i) global.lights[i].position.xyz
#define Lis_dir(i) (global.lights[i].position.w == 0)
#define Lcolor(i) global.lights[i].color_AND_ambi.xyz
#define Lspot_dir(i) global.lights[i].spot_dir_AND_cut_off.xyz

#define Lambient(i) global.lights[i].color_AND_ambi.w
#define Lspot_cutoff(i) global.lights[i].spot_dir_AND_cut_off.w
#define Lspot_exponent(i) global.lights[i].spot_exponent_AND_size_AND_enabled.x
#define Lsize(i) global.lights[i].spot_exponent_AND_size_AND_enabled.y
#define Lenabled(i) (global.lights[i].spot_exponent_AND_size_AND_enabled.w > 0.5)
} global;


float getShadowFactor(vec4 pos, vec2 offset)
{
    // sampler does the perspective divide + compare for us!
    return textureProj(shadow_map, pos + vec4(offset*pos.w, 0.0, 0.0));
}

vec2 shadowRes = vec2(textureSize(shadow_map,0));
int size=int(global.lights[0].spot_exponent_AND_size_AND_enabled.z -1)/2;

float get_shadow(vec3 pos, int index)
{
    if (index > 0)
    {
        // currently, we support shadows only for light source 0
        return 1.0;
    }

	vec4 pos_shadow = global.lights[0].worldSpaceToShadowMap * vec4(pos, 1);
        
    // shadow map for light 0
	ivec2 opos;

    float shadowFactor = 0.0;
    for(opos.x = -size; opos.x <= size; opos.x++) {
		for(opos.y = -size; opos.y <= size; opos.y++) {					
				shadowFactor += getShadowFactor(pos_shadow, vec2(opos) / shadowRes);
		}
	}

    return shadowFactor / ((2*size+1)*(2*size+1));

}
