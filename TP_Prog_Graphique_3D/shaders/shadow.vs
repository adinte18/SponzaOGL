#version 430

// INPUT
layout(location = 1) in vec3 position_in;

// UNIFORM
layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 lightSpaceMatrix;

// MAIN PROGRAM
void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(position_in, 1.f);
}