#version 430 core

layout(std140,binding = 3) uniform materialDataBlock
{
    vec4 albedo;
    vec4 properties;
} material;


vec4 get_albedo()
{
    // 2 (a):
    // return ...
    return vec4(1.0);
}

