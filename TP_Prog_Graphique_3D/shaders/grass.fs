#version 430
precision highp float;

#define M_PI 3.14159265358979

// OUTPUT
out vec4 oFragmentColor;

in vec2 tex_coord;
in float colorVariation;

layout (location = 4) uniform sampler2D grassTex;
layout (location = 9) uniform sampler2D grassAtlas;

// MAIN PROGRAM
void main()
{
    vec4 color = texture(grassTex, tex_coord);
    if (color.a < 0.25) discard;
    color.xyz = mix(color.xyz, 0.5 * color.xyz, colorVariation);
    oFragmentColor = color;
}
