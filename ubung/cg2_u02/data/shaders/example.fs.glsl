#version 430 core

layout(location = 4) uniform vec3 cam_pos_ws;

// Input varying
in vec4 vclr;

in vec3 vnrm_ws;
in vec3 vpos_ws;

// Output fragment color
out vec4 fclr;

struct LightData{
	vec4 position_ws;
	vec4 color;
	vec4 speed; // This can be ignored !!!
};

// (f)
// copy/past the uniform buffer block

layout(std140, binding = 0) 
uniform globalLightDataBlock{
	LightData lights[8]; 
}globalLights; 

// (c)
//layout(location = 5) uniform vec4 light_pos;
//layout(location = 6) uniform vec4 light_color;

//LightData one_light = LightData(light_pos,light_color,vec4(0.0));
LightData one_light;

void main()
{
	// (f): iterate over all 8 light sources
	for(int i=0;i<8;i++){
		one_light.position_ws = globalLights.lights[i].position_ws;
		one_light.color = globalLights.lights[i].color;

		// (a):
		vec3 l_ws = normalize(one_light.position_ws.xyz-vpos_ws);
		vec3 n_ws = normalize(vnrm_ws);
		float diffuse = max(dot(n_ws,l_ws),0);
		

		// (b):
		float d = length(one_light.position_ws.xyz-vpos_ws);
		float a = 1/(1+d*d);

		//spekular
		vec3 v_ws = normalize(cam_pos_ws-vnrm_ws);
		vec3 h_ws = normalize(l_ws+v_ws);
		float spekular = max(dot(n_ws,h_ws),0);

		if(diffuse==0.0){
			spekular = 0.0;
		}
		else{
			spekular = pow(spekular,64);
		}
		vec3 scattered;
		vec3 reflect;
		scatt += diffuse*one_light.color.xyz*a;
		reflect += spekular*one_light.color.xyz*a;
		}
	fclr =vec4(vclr.rgb*scattered + reflect,1.0);
	//fclr.a = 1.0;
	
}
