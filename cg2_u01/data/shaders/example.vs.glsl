#version 430 core
// Attributes
layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 clr;
// Uniforms
layout(location = 0) uniform mat4 MVP;

// Output varyings
out vec4 vclr;
void main()
{
	gl_Position = MVP * pos;

	// A1 c): Diese Zeile auskommentieren ...
	//vclr = vec4(0.42,0.54,0.15,1);
	// und diese Zeile einkommentieren ....
	vclr = clr;
}
