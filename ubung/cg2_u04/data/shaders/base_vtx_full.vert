#version 430 core

invariant gl_Position;

vec4 transfrom_position_cs();
void transfrom_varyings();

void main()
{
    gl_Position = transfrom_position_cs();
    transfrom_varyings();
}
