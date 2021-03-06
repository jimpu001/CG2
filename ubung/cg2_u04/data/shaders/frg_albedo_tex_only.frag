#version 430 core

in vec2 v_texcoord;
layout(binding = 0) uniform sampler2D albedo_map;


vec4 get_albedo()
{
    return texture(albedo_map,v_texcoord);
}

