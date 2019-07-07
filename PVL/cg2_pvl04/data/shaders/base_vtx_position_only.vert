#version 430 core
invariant gl_Position;
vec4 transfrom_position_cs();
void transfrom();

invariant gl_Position;

void main()
{
    gl_Position = transfrom_position_cs();
}
