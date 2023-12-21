#version 430

layout(location = 1) in vec3 position_in;

layout(location = 1) uniform mat4 modelMatrix;
layout(location = 2) uniform mat4 viewMatrix;
layout(location = 3) uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position_in, 1);
}