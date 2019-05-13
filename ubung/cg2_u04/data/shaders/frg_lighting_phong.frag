#version 430 core

in vec3 v_position_ws;
const float CG2PI = 3.14159265359;


layout(std140,binding = 1) uniform cameraDataBlock
{
        mat4 view;
        mat4 inv_view;
        mat4 projection;
        mat4 inv_projection;
        vec4 near_far_width_height;

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



}global;




vec3 one_light(in int light_id, in vec3 N, in vec3 fpos, in vec3 albedo,in vec2 mp)
{
    // 1 (c)
    return albedo;

}


vec3 lighting(in vec3 albedo, in vec4 material_props, in vec3 normal)
{

    vec3 res_color = vec3(0);
    for(int i =0 ; i < 8;i++){
        if(Lenabled(i)){
            res_color += one_light(i,
                                   normal,
                                   v_position_ws,
                                   albedo,
                                   material_props.xy
                                   );
        }
    }

    return res_color;
}
