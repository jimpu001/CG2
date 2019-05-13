#version 430 core
// Attributes
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nrml;
layout(location = 4) in vec2 tex_coord;


layout(std140,binding = 1) uniform cameraDataBlock
{
	mat4 view;
	mat4 inv_view;
	mat4 projection;
	mat4 inv_projection;
	vec4 near_far_width_height;

} camera;


layout(std140,binding = 2) uniform objectDataBlock
{
	mat4 model;
	mat4 normal;

} object;


// Output varyings
out vec4 vclr;
out vec3 vnrm_ws;
out vec3 vpos_ws;

//layout(location=0) uniform mat4 view;
//layout(location=1) uniform mat4 projection;

void main()
{
	gl_Position = camera.projection * camera.view * object.model * vec4(pos,1.0);
	vnrm_ws = (vec4(nrml,0.0) * object.normal).xyz;
	vpos_ws = (vec4(pos,1.0) * object.model).xyz;

	vclr = vec4(1.0f);
}
