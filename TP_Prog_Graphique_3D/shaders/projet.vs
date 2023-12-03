#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 normales_in;
layout(location = 3) in vec2 tex_coord_in;
layout(location = 4) in vec3 tangentes_in;

//OUTPUT
out vec3 v_pos;
out vec3 v_norm;
out vec2 tex_coord;
out mat3 TBN;
out vec4 FragPosLightSpace;

// UNIFORM
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;
layout(location = 3) uniform mat3 uMVnormalMatrix;
layout(location = 13) uniform mat4 lightSpaceMatrix;


// MAIN PROGRAM
void main()
{
	tex_coord = tex_coord_in;

	//normal in view space
	v_norm = uMVnormalMatrix * normales_in;

	//Matrice TBN
    vec3 T = normalize(uMVnormalMatrix * tangentes_in);
    vec3 B = cross(normalize(v_norm), T);
    TBN = transpose(mat3(T, B, normalize(v_norm)));

	vec4 viewPos = uModelMatrix * vec4(position_in, 1.0);
	v_pos = viewPos.xyz;

	//get the pos in world space
	gl_Position = uProjectionMatrix * uViewMatrix * vec4(v_pos.xyz, 1.0f);

	FragPosLightSpace = lightSpaceMatrix * vec4(v_pos.xyz, 1.0f);
}