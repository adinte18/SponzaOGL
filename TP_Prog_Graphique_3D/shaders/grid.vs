#version 430 core

layout (location = 1) in vec3 aPosition;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexcoord;

layout (location = 1) uniform mat4 model;
layout (location = 2) uniform mat4 view;
layout (location = 3) uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
}