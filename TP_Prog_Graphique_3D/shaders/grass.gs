#version 430
#define PI 3.14159265358979

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout (location = 1) uniform mat4 u_model;
layout (location = 2) uniform mat4 u_view;
layout (location = 3) uniform mat4 u_projection;

out vec2 tex_coord;

mat4 rotationX(in float angle);
mat4 rotationY(in float angle);
mat4 rotationZ(in float angle);
float random (vec2 st);

float grass_size;
const float c_min_size = 2.4f;

void createQuad(vec3 base_position, mat4 crossmodel){
	vec4 vertexPosition[4];
	vertexPosition[0] = vec4(-0.2, 0.0, 0.0, 0.0); 	// down left
	vertexPosition[1] = vec4( 0.2, 0.0, 0.0, 0.0);		// down right
	vertexPosition[2] = vec4(-0.2, 0.2, 0.0, 0.0);	// up left
	vertexPosition[3] = vec4( 0.2, 0.2, 0.0, 0.0);	// up right

	vec2 textCoords[4];
	textCoords[0] = vec2(0.0, 0.0);						// down left
	textCoords[1] = vec2(1.0, 0.0);						// down right
	textCoords[2] = vec2(0.0, 1.0);						// up left
	textCoords[3] = vec2(1.0, 1.0);						// up right

	mat4 modelRandY = rotationY(random(base_position.zx)*PI);


	for(int i = 0; i < 4; i++) {
		gl_Position = u_projection * u_view * (gl_in[0].gl_Position 
			+ modelRandY * crossmodel * (vertexPosition[i] * grass_size));
	
		tex_coord = textCoords[i];
	    EmitVertex();
    }
    EndPrimitive();
}

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle )
{
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}


void createGrass()
{
	mat4 model0, model45, modelm45;
	model0 = mat4(1.0f);
	model45 = rotationY(radians(45));
	modelm45 = rotationY(-radians(45));
 
	createQuad(gl_in[0].gl_Position.xyz, model0);
	createQuad(gl_in[0].gl_Position.xyz, model45);
	createQuad(gl_in[0].gl_Position.xyz, modelm45);	
}

void main()
{
    grass_size = random(gl_in[0].gl_Position.xz) * (1.0f - c_min_size) 
    		+ c_min_size;

	createGrass();
}