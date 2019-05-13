#version 430 core
invariant gl_Position;
vec4 transfrom_position_cs();

void main()
{
    // 3 (a):
    //gl_Position = vec4(0,0,0,0);
	gl_Position=transfrom_position_cs();

}
