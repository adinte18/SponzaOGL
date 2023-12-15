//--------------------------------------------------------------------------------------------------------
// SKYBOX VERTEX SHADER (GLSL language)
//--------------------------------------------------------------------------------------------------------
#version 430

layout(location = 1) in vec3 position_in;

// model-view-proj matrix
layout(location = 0) uniform mat4 viewMatrix;
layout(location = 1) uniform mat4 projMatrix;

out vec3 tex_coord;

void main()
{
	// we use the position of the vertex for the 3D tex coords
	tex_coord = position_in;
	vec4 P4 = projMatrix * viewMatrix * vec4(position_in, 1.0);
	gl_Position = P4.xyww; 	// after projection (after the VS) the pipeline apply xyz /= w
							// and for the environment map we want all the fragments in Z equal to 1 so we set the Z position to w to have w/w = 1 
}