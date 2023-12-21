#version 430
#define PI 3.14159265358979
#define NUM_TEXTURES_X 4.0
#define NUM_TEXTURES_Y 2.0

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout (location = 1) uniform mat4 u_model;
layout (location = 2) uniform mat4 u_view;
layout (location = 3) uniform mat4 u_projection;
layout (location = 5) uniform sampler2D u_wind;
layout (location = 6) uniform float u_time;
layout (location = 7) uniform float grass_min_size;
layout (location = 8) uniform float wind_speed;

out vec2 tex_coord;

mat4 rotationX(in float angle);
mat4 rotationY(in float angle);
mat4 rotationZ(in float angle);
float random (vec2 st);

float grass_size;
const float c_min_size = grass_min_size;


void createQuad(vec3 base_position, mat4 crossmodel, vec2 texOffset){
	vec4 vertexPosition[4];
	vertexPosition[0] = vec4(-0.2, 0.0, 0.0, 0.0); 	// down left
	vertexPosition[1] = vec4( 0.2, 0.0, 0.0, 0.0);		// down right
	vertexPosition[2] = vec4(-0.2, 0.5, 0.0, 0.0);	// up left
	vertexPosition[3] = vec4( 0.2, 0.5, 0.0, 0.0);	// up right

	vec2 textCoords[4];
	textCoords[0] = vec2(0.0, 0.0);						// down left
	textCoords[1] = vec2(1.0, 0.0);						// down right
	textCoords[2] = vec2(0.0, 1.0);						// up left
	textCoords[3] = vec2(1.0, 1.0);						// up right

	float randomSeed = 0.5; // Seed value for initial direction, can be any constant value

    vec2 randDirection = normalize(vec2(random(vec2(randomSeed, 0.0)), random(vec2(randomSeed + 1.0, 0.0))));

	vec2 windDirection = randDirection; // direction du vent
	float windStrength = wind_speed;	// force du vent
	// coordonnées de textures du vent qui se déplace
	vec2 uv = base_position.xz/10.0 + windDirection * windStrength * u_time ;
	uv.x = mod(uv.x,1.0); // on ramère la coordonnée modulo 1
	uv.y = mod(uv.y,1.0);
 
	vec4 wind = texture(u_wind, uv); // on récupère la valeur rgba
	// on calcule la matrice qui permet d'incliner le quad en fonction de la 
	// direction et force du vent
	mat4 modelWind =  (rotationX(wind.x*PI*0.75f - PI*0.25f) * 
		  		rotationZ(wind.y*PI*0.75f - PI*0.25f)); 

	mat4 modelWindApply = mat4(1);
	// pour chaque coin du quad
	for(int i = 0; i < 4; i++) {
		// pour appliquer le vent seulement sur les coins du dessus
		if (i == 2 ) modelWindApply = modelWind;
		// calcul de la position finale
		gl_Position = u_projection * u_view * 
			(gl_in[0].gl_Position + modelWindApply*crossmodel*
        				(vertexPosition[i]*grass_size));
 
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
 
	float textureIndex = 1.0; // Replace this with your method to determine the texture index
	vec2 textureOffset = vec2(mod(textureIndex, NUM_TEXTURES_X), floor(textureIndex / NUM_TEXTURES_X)) / vec2(NUM_TEXTURES_X, NUM_TEXTURES_Y);


	createQuad(gl_in[0].gl_Position.xyz, model0, textureOffset);
	createQuad(gl_in[0].gl_Position.xyz, model45, textureOffset);
	createQuad(gl_in[0].gl_Position.xyz, modelm45, textureOffset);	
}

void main()
{
    grass_size = random(gl_in[0].gl_Position.xz) * (1.0f - c_min_size) 
    		+ c_min_size;

	createGrass();
}