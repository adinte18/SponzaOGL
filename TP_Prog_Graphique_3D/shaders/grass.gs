#version 430
#define PI 3.14159265358979
#define NUM_TEXTURES_X 4.0
#define NUM_TEXTURES_Y 2.0
#define FAR 0
#define MID 1
#define CLOSE 2
#define CLOSEST 3

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

layout (location = 1) uniform mat4 u_model;
layout (location = 2) uniform mat4 u_view;
layout (location = 3) uniform mat4 u_projection;
layout (location = 5) uniform sampler2D u_wind;
layout (location = 6) uniform float u_time;
layout (location = 7) uniform float grass_min_size;
layout (location = 8) uniform float wind_speed;
layout (location = 10) uniform vec3 u_camerapos;
layout (location = 12) uniform int height;
layout (location = 13) uniform int width;

out vec2 tex_coord;
out float color_variation;

mat4 rotationX(in float angle);
mat4 rotationY(in float angle);
mat4 rotationZ(in float angle);
float random (vec2 st);
float noise (in vec2 st);
float fbm ( in vec2 _st);


const float c_min_size = grass_min_size;
const float LOD1 = 10.0f;
const float LOD2 = 30.0f;
const float LOD3 = 60.0f;


void createQuad(vec3 base_position, mat4 crossmodel){
	vec4 vertexPosition[4];
	vertexPosition[0] = vec4(-0.5, 0.0, 0.0, 0.0); 	// down left
	vertexPosition[1] = vec4( 0.5, 0.0, 0.0, 0.0);		// down right
	vertexPosition[2] = vec4(-0.5, 1.0, 0.0, 0.0);		// up left
	vertexPosition[3] = vec4( 0.5, 1.0, 0.0, 0.0);		// up right

	vec2 textCoords[4];

    float offsetX = random(base_position.xz * 0.1);  // Offset aléatoire en X
    float offsetY = random(base_position.zy * 0.1);  // Offset aléatoire en Y

    int textureX = int(mod(offsetX * 4.0, 4.0)); // Sélection aléatoire sur l'axe X (de 0 à 3)
    int textureY = int(mod(offsetY * 2.0, 2.0)); // Sélection aléatoire sur l'axe Y (de 0 à 1)

    textCoords[0] = vec2(textureX * 0.25, textureY * 0.5);                  // Coin en bas à gauche
    textCoords[1] = vec2((textureX + 1) * 0.25, textureY * 0.5);           // Coin en bas à droite
    textCoords[2] = vec2(textureX * 0.25, (textureY + 1) * 0.5);            // Coin en haut à gauche
    textCoords[3] = vec2((textureX + 1) * 0.25, (textureY + 1) * 0.5);     // Coin en haut à droite

	float grass_size = random(gl_in[0].gl_Position.xz) * (1.0f - c_min_size) 
															+ c_min_size;


	vec2 windDirection = vec2(1.0, 1.0); 
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
	
	mat4 modelRandY = rotationY(random(base_position.zx)*PI);

	// pour chaque coin du quad
	for(int i = 0; i < 4; i++) {
		// pour appliquer le vent seulement sur les coins du dessus
		if (i == 2 ) modelWindApply = modelWind;
		// calcul de la position finale
	    gl_Position = 	    
			gl_Position = u_projection * u_view *
            (gl_in[0].gl_Position + modelWindApply*modelRandY*crossmodel*(vertexPosition[i]*grass_size));

 

		color_variation = fbm(gl_in[0].gl_Position.xz);

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

float noise (in vec2 st) {
	vec2 i = floor(st);
	vec2 f = fract(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));

	// Smooth Interpolation

	// Cubic Hermine Curve.  Same as SmoothStep()
	vec2 u = f*f*(3.0-2.0*f);
	// u = smoothstep(0.,1.,f);

	// Mix 4 coorners percentages
	return mix(a, b, u.x) +
	(c - a)* u.y * (1.0 - u.x) +
	(d - b) * u.x * u.y;
}
#define NUM_OCTAVES 5
float fbm ( in vec2 _st) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100.0);
	// Rotate to reduce axial bias
	mat2 rot = mat2(cos(0.5), sin(0.5),
	-sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(_st);
		_st = rot * _st * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}


void createGrass(int numberQuads) {
    mat4 model0, model45, modelm45, model_far;
	model_far = mat4(0.5f);
    model0 = mat4(1.0f);
    model45 = rotationY(radians(45));
    modelm45 = rotationY(-radians(45));

    switch (numberQuads) {
		case FAR: {
			createQuad(gl_in[0].gl_Position.xyz, model_far);
			break;
		}
        case MID: {
            createQuad(gl_in[0].gl_Position.xyz, model0);
            break;
        }
        case CLOSE: {
            createQuad(gl_in[0].gl_Position.xyz, model0);
            createQuad(gl_in[0].gl_Position.xyz, modelm45);
            break;
        }
        case CLOSEST: {
            createQuad(gl_in[0].gl_Position.xyz, model0);
            createQuad(gl_in[0].gl_Position.xyz, model45);
            createQuad(gl_in[0].gl_Position.xyz, modelm45);
            break;
        }
    }
}


void main()
{
	mat4 viewModel = inverse(u_model * u_view);
	vec3 cameraPos = vec3(viewModel[3]);
	
	vec3 distance_with_camera = gl_in[0].gl_Position.xyz - cameraPos ;

	float dist_length = length(distance_with_camera); // finally

	if (dist_length < LOD1) { createGrass(CLOSEST); }
	if (dist_length >= LOD1 && dist_length < LOD2){ createGrass(CLOSE); }
	if (dist_length >= LOD2 && dist_length < LOD3){ createGrass(MID); }
	if (dist_length > LOD3) {createGrass(FAR);}

}