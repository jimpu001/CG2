#version 430 core
layout(std140,binding = 1) uniform cameraDataBlock
{
    mat4 view;
    mat4 inv_view;
    mat4 projection;
    mat4 inv_projection;
    vec4 near_far_width_height;
} camera;


const vec4 pos_cs[4] = { vec4(-1,-1,1,1),
                         vec4( 1,-1,1,1),
                         vec4(-1, 1,1,1),
                         vec4( 1, 1,1,1)};

out vec3 v_view_ray_ws;
void main()
{
    // This is called attributeless rendering:
    // gl_VertexID is a buildin attribute giving the vertex id.
    // We are rendering 4 vertices as a GL_TRIANGLE_STRIP
    // So we get 4 vertex shader invocations, one for each corner.
    // The array pos_cs[4] holds the vertex positions in clip space.
	vec4 v = camera.inv_view*camera.inv_projection*pos_cs[gl_VertexID];
	v.xyz = v.xyz/v.w;
    gl_Position = pos_cs[gl_VertexID];
    v_view_ray_ws = v.xyz-camera.inv_view[3].xyz; //camera.inv_view[3].xyz就是摄像机在worldspace的位置

}
