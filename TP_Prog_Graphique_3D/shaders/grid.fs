#version 430
precision highp float;

out vec4 color;

layout (location = 4) uniform float intensity;

void main()
{
	color = vec4(vec3(intensity), 1.0f);
}