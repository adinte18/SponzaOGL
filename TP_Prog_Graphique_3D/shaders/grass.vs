#version 430

// INPUT
layout(location = 1) in vec3 position_in;

void main()
{
	gl_Position = vec4(position_in, 1.0); 
}