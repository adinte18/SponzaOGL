//--------------------------------------------------------------------------------------------------------
// SKYBOX FRAGMENT SHADER (GLSL language)
//--------------------------------------------------------------------------------------------------------
#version 430

precision highp float;

in vec3 tex_coord;
layout(binding = 0) uniform samplerCube uSkyTex;

out vec4 frag;

void main()
{	
	// The texture cube map is sampled with 3D coords
	frag = texture(uSkyTex, tex_coord);
}