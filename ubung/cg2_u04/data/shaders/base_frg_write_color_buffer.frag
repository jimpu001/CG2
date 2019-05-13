#version 430 core

// 3 (b):
// force early depth test:
// layout(early_fragment_tests) in;
layout(location = 0) out vec4 frag_color;





vec4 get_albedo();
vec4 get_material_props();
vec3 get_normal();
vec3 lighting(in vec3 albedo,
              in vec4 material_props,
              in vec3 normal);

void main()
{
    vec4 albedo = get_albedo();


    // 3 (b):
    // Alpha Test
    if(albedo.a < 0.5)
        discard;

    // calculate color
    frag_color.rgb =lighting(albedo.rgb,
                             get_material_props(),
                             get_normal());

    // transform back to 'gamma compressed' colors
     frag_color.rgb = pow(frag_color.rgb,vec3(1.0/2));
}
