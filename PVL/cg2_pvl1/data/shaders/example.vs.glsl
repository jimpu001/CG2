#version 430 core
// Attributes
layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 nrml;
layout(location = 4) in vec4 tex_coord;
// Uniforms
layout(location = 0) uniform mat4 MVP;


// Output varyings
out vec4 vclr;
void main()
{
	gl_Position = MVP * pos;

	// fake shading!
	vclr = vec4(vec3(1,0.5,0)*clamp(dot(normalize(vec3(1,1,1)),nrml.rgb),0.3,1.0),1);
}
