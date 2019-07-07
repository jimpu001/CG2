#version 430 core
// This is the noperation shader used generate depth maps etc.

//layout(early_fragment_tests) in;
vec4 get_albedo();

void main(void)
{
    vec4 albedo = get_albedo();
    if(albedo.a < 0.5)
        discard;
}
