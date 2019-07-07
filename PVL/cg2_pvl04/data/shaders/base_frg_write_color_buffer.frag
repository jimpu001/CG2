#version 430 core

layout(early_fragment_tests) in;
layout(location = 0) out vec4 frag_color;

vec4 get_albedo();
vec4 get_material_props();
vec3 get_normal();
float get_specular();
vec3 get_displacement();
vec3 lighting(in vec3 albedo, in vec4 material_props, in vec3 normal);

void main()
{
    vec4 albedo = get_albedo();
//    if(albedo.a < 0.5)
//        discard;

    frag_color.rgb = lighting(albedo.rgb,get_material_props(),get_normal());
    frag_color.a = 1.0;

    // Both shifted to the pp_tonemapping step.
    // apply Reinhards tone mapping
    // frag_color.rgb = frag_color.rgb/(frag_color.rgb+vec3(1.0));
}
