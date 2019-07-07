#version 430 core

layout(std140, binding = 3) uniform materialDataBlock
{
    vec4 albedo;
    vec4 properties;
    vec4 properties_scale;
    vec4 properties_offset;
} material;

vec4 get_albedo()
{
    return vec4(material.albedo);
}

