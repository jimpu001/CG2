#version 430 core

vec4 transfrom_position_cs();
void transfrom();

invariant gl_Position;

void main()
{
    gl_Position = transfrom_position_cs();
    transfrom();
}
