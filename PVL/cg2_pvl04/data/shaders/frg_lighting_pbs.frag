#version 430 core

const float CG2PI = 3.14159265359;

layout(std140,binding = 1) uniform cameraDataBlock
{
    mat4 view;
    mat4 inv_view;
    mat4 projection;
    mat4 inv_projection;
    vec4 near_far_width_height;
	vec4 position;

} camera;

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

float get_shadow(vec3 pos, int index);

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = CG2PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 light_pbr (in int light_id,
                in vec3 L,
                in vec3 V,
                in vec3 H,
                in vec3 N,
                in float NdotL,
                in float attenuation,
                in vec3 albedo,
                in vec2 mp)
{

    vec3 F0 = vec3(0.04);

    F0 = mix(F0, albedo, mp.y);
    vec3 radiance  = Lcolor(light_id) * attenuation;

    // cook-torrance brdf
    float D = DistributionGGX(N, H, mp.x);
    float G = GeometrySmith(N, V, L, mp.x);

    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0,1.0), F0);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - mp.y;

    vec3 nominator    = D * G * F;

    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL;
    vec3 specular     = nominator / max(denominator, 0.0001);

    return (kD * albedo / CG2PI + specular) * radiance * NdotL;
}




vec3 one_light(in int light_id, in vec3 N, in vec3 fpos, in vec3 albedo,in vec2 mp)
{

    float size = Lsize(light_id);

    vec3 L = Lpos(light_id) - fpos;
    float dist = length(L);
    float attenuation = clamp(-((dist*dist)/(size*size))+1,0,1);
    if(size <0 ){
        attenuation = 1.0;
    }

    if(Lis_dir(light_id))
    {
        L = Lpos(light_id);
        attenuation = 1.0;
    }



    // Fragment to far away.
    if(attenuation <=0){
        return vec3(0);
    }

    L = normalize(L);

    vec3 V = normalize(camera.inv_view[3].xyz-fpos);

    float NdotL=dot(N,L);
    vec3 ambient = Lambient(light_id)*albedo*Lcolor(light_id);


    // Fragment is facing the wrong direction
    if (NdotL <= 0.0)
        return ambient;



    float spot = 1.0;
	if(light_id!=0){
		if(Lspot_cutoff(light_id) > -1.0)
		{
			spot = max(dot(Lspot_dir(light_id), -L),0.0);

			if (spot >= Lspot_cutoff(light_id))
			{
				spot=pow(spot, Lspot_exponent(light_id));
			}
			else
				spot = 0.0;
		}
	}


    // Light is outside the spot
    if(spot == 0.0)
        return ambient;

    float shadow_factor = get_shadow(fpos,light_id);
    if(shadow_factor < 0.001)
        return ambient;


    return ambient +light_pbr(light_id,
                              L,
                              V,
                              normalize(L+V),
                              N,
                              NdotL,
                              spot* attenuation*shadow_factor,
                              albedo,mp);

}

vec3 lighting(in vec3 albedo, in vec4 material_props, in vec3 normal)
{


    vec3 res_color = vec3(0);
    for(int i =0 ; i < 8;i++)
    {
        if(Lenabled(i))
        {
           // float shadowFactor = get_shadow(varying_in.Position_ws,i);
            res_color += one_light(i,normal,varying_in.position_ws,albedo,material_props.xy) ;
        }
    }
    return res_color;
}

