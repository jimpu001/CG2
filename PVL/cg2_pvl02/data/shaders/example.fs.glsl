#version 430 core

// Input varying
in vec4 vclr;

in vec3 vnrm_ws;
in vec3 vpos_ws;

// Output fragment color
out vec4 fclr;

struct Light
{
    vec4 position;
    vec4 color_AND_ambi;
    vec4 spot_dir_AND_cut_off;
    vec4 spot_exponent_AND_size_AND_enabled; // z  unused!
    mat4 worldSpaceToShadowMap;

    // some macros to access the light data
#define Lpos(l) l.position.xyz
#define Lcolor(l) l.color_AND_ambi.xyz
#define Lspot_dir(l) l.spot_dir_AND_cut_off.xyz

#define Lambient(l) l.color_AND_ambi.w
#define Lspot_cutoff(l) l.spot_dir_AND_cut_off.w
#define Lspot_exponent(l) l.spot_exponent_AND_size_AND_enabled.x
#define Lsize(l) l.spot_exponent_AND_size_AND_enabled.y
#define Lenabled(l) (l.spot_exponent_AND_size_AND_enabled.w > 0.5)
};


layout(std140, binding = 4) uniform globalLightDataBlock
{
    Light lights[8];
}globalLights;



layout(std140,binding = 1) uniform cameraDataBlock
{
	mat4 view;
	mat4 inv_view;
	mat4 projection;
	mat4 inv_projection;
	vec4 near_far_width_height;

} camera;


//layout(location=2) uniform mat4 inv_view;

void main()
{
	mat4 cam = camera.inv_view;
	vec3 cam_pos_ws = cam[3].xyz;
	vec3 light_color = vec3(0.0,0.0,0.0);
	for(int i =0; i< 8;i++){
		const Light one_light = globalLights.lights[i];
		if(!Lenabled(one_light))
			continue;

		vec3 L = Lpos(one_light)-vpos_ws;
		float d = length(L);
		L /= d;

		vec3 N = normalize(vnrm_ws);
		vec3 V = normalize(cam_pos_ws-vpos_ws);
		vec3 H = normalize(V+L);

		float atten = 1.0/(1+d*d);
		float NdotL = clamp(dot(N,L),0.0,1.0);
		vec3 diff = Lcolor(one_light) * NdotL;

		vec3 spec = vec3(0.0);
		if(NdotL >= 0.0)
			spec = Lcolor(one_light) * clamp(pow(dot(N,H),64.0),0,1);
		vec3 ambi = Lcolor(one_light) * Lambient(one_light);
		light_color += atten*(spec+diff)+ambi;

	}

	fclr.rgb= vclr.rgb*light_color;
	fclr.a = 1.0;
}
