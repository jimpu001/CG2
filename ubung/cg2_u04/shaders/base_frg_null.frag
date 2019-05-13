#version 430 core
// This is the no-operation shader used generate depth maps etc.


vec4 get_albedo();
void main(void)
{
    // 3 (b):
	vec4 albedo = get_albedo();
	if(albedo.a < 0.5)
        discard;
}
