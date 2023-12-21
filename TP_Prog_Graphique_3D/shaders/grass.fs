#version 430
precision highp float;

#define M_PI 3.14159265358979

// OUTPUT
out vec4 oFragmentColor;

in vec2 tex_coord;

layout (location = 4) uniform sampler2D grassTex;

// MAIN PROGRAM
void main()
{
    vec4 color = texture(grassTex, tex_coord);
    if (color.a < 0.05) discard;
    oFragmentColor = color;
}
