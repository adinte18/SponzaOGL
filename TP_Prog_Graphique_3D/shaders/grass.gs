#version 430
#define PI 3.14159265358979
#define NUM_TEXTURES_X 4.0
#define NUM_TEXTURES_Y 2.0
#define FAR 0
#define MID 1
#define CLOSE 2
#define CLOSEST 3

/*

=================================================DISCLAIMER=================================================

Ces tutoriels ont été pris comme reference : 
https://vulpinii.github.io/tutorials/grass-modelisation/fr/
https://roystan.net/articles/grass-shader/

Il est important de souligner que je n'ai en aucun cas effectué de copier-coller direct des tutoriels en question.
Au contraire, j'ai pris le temps de les comprendre,et de l'appliquer à ma manière en utilisant les idées des tutoriels.

=================================================DISCLAIMER=================================================

*/

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

layout (location = 1) uniform mat4 u_model;
layout (location = 2) uniform mat4 u_view;
layout (location = 3) uniform mat4 u_projection;
layout (location = 5) uniform sampler2D u_wind;
layout (location = 6) uniform float u_time;
layout (location = 7) uniform float grass_min_size;
layout (location = 8) uniform float wind_speed;
layout (location = 10) uniform float more_flowers;

out vec2 tex_coord;
out float color_variation;

/*==================================UTILS==================================*/
mat4 rotationX(float angle) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY(float angle)
{
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ(float angle) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}


float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

//J'ai pris la fonction d'ici : https://thebookofshaders.com/11/
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
/*==================================UTILS==================================*/


const float c_min_size = grass_min_size;
const float LOD1 = 10.0f;
const float LOD2 = 30.0f;
const float LOD3 = 60.0f;


mat4 wind_calculation(){
	vec2 windDirection = vec2(1.0, 1.0); 
	float speed = wind_speed * u_time;
	// Tiling + animation
	vec2 tiled_uv = gl_in[0].gl_Position.xz * 0.05 + windDirection * speed;

	tiled_uv.x = fract(tiled_uv.x); 
	tiled_uv.y = fract(tiled_uv.y);
 
	vec4 wind = texture(u_wind, tiled_uv);

	mat4 m_wind =  (rotationX(wind.x*PI*0.75f - PI*0.25f) * 
		  			rotationZ(wind.y*PI*0.75f - PI*0.25f) ); 

	return m_wind;
}

void billboard(vec3 base_position, mat4 grass_model){
	vec4 v_pos[4];
	v_pos[0] = vec4(-0.5, 0.0, 0.0, 0.0); 		// down left
	v_pos[1] = vec4( 0.5, 0.0, 0.0, 0.0);		// down right
	v_pos[2] = vec4(-0.5, 1.0, 0.0, 0.0);		// up left
	v_pos[3] = vec4( 0.5, 1.0, 0.0, 0.0);		// up right

	vec2 t_coord[4];

	//Pretraitement atlas
	float offsetx = random(base_position.xz * 0.1);
	float offsety = random(base_position.zy * 0.1);

	int column = int(mod(offsetx * 4.0, 4.0)); // selectionner les 4 colonnes
	int row;

	bool preferFlower = random(base_position.xy) < more_flowers; // la proba de choisir une fleur

	if (preferFlower) {
		row = 0;
	} else {
		row = 1;
	}

	t_coord[0] = vec2(column * 0.25, row * 0.5);
	t_coord[1] = vec2((column + 1) * 0.25, row * 0.5);
	t_coord[2] = vec2(column * 0.25, (row + 1) * 0.5);
	t_coord[3] = vec2((column + 1) * 0.25, (row + 1) * 0.5);

	//la taille d'herbe
	float grass_size = random(gl_in[0].gl_Position.xz) * c_min_size;

	color_variation = noise(gl_in[0].gl_Position.xz);

	for(int i = 0; i < 4; i++) {
		gl_Position = u_projection * u_view * 
        (gl_in[0].gl_Position + wind_calculation() * grass_model * (v_pos[i]*grass_size));

		tex_coord = t_coord[i];
		EmitVertex();
	}
	EndPrimitive();
}

void draw_grass(int dst) {
	mat4 rotationEnY = rotationY(random(gl_in[0].gl_Position.zx)*PI);

    mat4 model0, model45, modelm45, model_far;
	model_far = mat4(0.5f) * rotationEnY;
    model0 = mat4(1.0f) * rotationEnY;
    model45 = rotationY(radians(45)) * rotationEnY;
    modelm45 = rotationY(-radians(45)) * rotationEnY;

    switch (dst) {
		case FAR: {
			//Petit billboard
			billboard(gl_in[0].gl_Position.xyz, model_far);
			break;
		}
        case MID: {
			//Billboard simple
            billboard(gl_in[0].gl_Position.xyz, model0);
            break;
        }
        case CLOSE: {
			//2 billboards croisees en forme de X
            billboard(gl_in[0].gl_Position.xyz, model0);
            billboard(gl_in[0].gl_Position.xyz, modelm45);
            break;
        }
        case CLOSEST: {
			//3 billboards croisees en forme de *
            billboard(gl_in[0].gl_Position.xyz, model0);
            billboard(gl_in[0].gl_Position.xyz, model45);
            billboard(gl_in[0].gl_Position.xyz, modelm45);
            break;
        }
    }
}


void main()
{
	//extraire la position de camera
	mat4 viewModel = inverse(u_model * u_view);
	vec3 cameraPos = vec3(viewModel[3]);
	
	//calculer la distance entre la camera et le quad
	vec3 distance_with_camera = gl_in[0].gl_Position.xyz - cameraPos ;
	float dst = length(distance_with_camera);

	//LOD basé sur la distance
	if (dst < LOD1) { draw_grass(CLOSEST); }
	if (dst >= LOD1 && dst < LOD2){ draw_grass(CLOSE); }
	if (dst >= LOD2 && dst < LOD3){ draw_grass(MID); }
	if (dst > LOD3) {draw_grass(FAR);}
}