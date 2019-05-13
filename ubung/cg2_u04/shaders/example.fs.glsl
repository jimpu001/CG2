#version 430 core

// Input varying
in vec4 vclr;

in vec3 vnrm_ws;
in vec3 vpos_ws;
in vec2 vtex;

// Output fragment color
out vec4 fclr;

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
    // some macros to access the light data
#define Lpos(i) global.lights[i].position.xyz
#define Lis_dir(i) (global.lights[i].position.w == 0)
#define Lcolor(i) global.lights[i].color_AND_ambi.xyz
#define Lspot_dir(i) global.lights[i].spot_dir_AND_cut_off.xyz

#define Lambient(i) global.lights[i].color_AND_ambi.w
#define Lspot_cutoff(i) global.lights[i].spot_dir_AND_cut_off.w
#define Lspot_exponent(i) global.lights[i].spot_exponent_AND_size_AND_enabled.x
#define Lsize(i) global.lights[i].spot_exponent_AND_size_AND_enabled.y
#define Lenabled(i) (global.lights[i].spot_exponent_AND_size_AND_enabled.w > 0.5)


}global;



layout(std140,binding = 1) uniform cameraDataBlock
{
	mat4 view;
	mat4 inv_view;
	mat4 projection;
	mat4 inv_projection;
	vec4 near_far_width_height;

} camera;


 layout(binding = 0) uniform sampler2D albedo_map;

void main()
{
	vec3 cam_pos_ws = camera.inv_view[3].xyz;
	vec3 light_color = vec3(0.0,0.0,0.0);
	vec3 light_color_spec = vec3(0.0,0.0,0.0);

        vec4 albedo  = texture(albedo_map,vtex);
        // alpha test.
        if(albedo.a < 0.5 )
            discard;

        vec3 N = normalize(vnrm_ws);
        vec3 V = normalize(cam_pos_ws-vpos_ws);

	for(int i =0; i< 8;i++){
                if(!Lenabled(i))
			continue;


                vec3 L = Lpos(i);
                if(!Lis_dir(i))
			L -= vpos_ws;

		float d = length(L);
		L /= d;

		vec3 H = normalize(V+L);

		float atten = 1.0/(1+d*d);
                if(Lis_dir(i))
			atten = 1.0;

		float NdotL = clamp(dot(N,L),0.0,1.0);
                vec3 diff = Lcolor(i) * NdotL;

		vec3 spec = vec3 (0);
		if(NdotL > 0.0){
                        spec = Lcolor(i) * clamp(pow(dot(N,H),8.0),0,1)*0.2;
		}

                vec3 ambi = Lcolor(i) * Lambient(i);
		light_color += atten*diff+ambi;
		light_color_spec += atten*spec;

	}
        fclr.rgb= albedo.rgb *light_color+light_color_spec;
        // Gamma Correction
        // fclr.rgb = pow(fclr.rgb,vec3(1.0/2));

	fclr.a = 1.0;
}
