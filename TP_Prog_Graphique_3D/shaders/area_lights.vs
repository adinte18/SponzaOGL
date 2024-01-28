#version 430 core

layout (location = 1) in vec3 position_in;
layout (location = 2) in vec3 normal_in;
layout (location = 3) in vec2 texcoord_in;

layout (location = 1) uniform mat4 model;
layout (location = 2) uniform mat4 view;
layout (location = 3) uniform mat4 projection;
layout (location = 4) uniform mat3 normalMatrix;

out vec3 w_pos;
out vec3 w_norm;
out vec2 texcoord;

void main()
{
	texcoord = texcoord_in;

	w_norm = normalMatrix * normal_in;

	vec4 worldPos = model * vec4(position_in, 1.0);
	w_pos = worldPos.xyz;

	gl_Position = projection * view * worldPos;
}