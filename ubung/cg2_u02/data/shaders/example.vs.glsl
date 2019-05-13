#version 430 core
// Attributes
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nrml;
layout(location = 4) in vec2 tex_coord;

// Uniforms
layout(location = 0) uniform mat4 M;
layout(location = 1) uniform mat4 V;
layout(location = 2) uniform mat4 P;


// Output varyings
out vec4 vclr;
out vec3 vnrm_ws;
out vec3 vpos_ws;

void main()
{
	gl_Position = P*V*M * vec4(pos,1.0);
	vnrm_ws = (M*vec4(nrml,0.0)).xyz;
	vpos_ws = (M* vec4(pos,1.0)).xyz;

	vclr = vec4(1.0f);
}
