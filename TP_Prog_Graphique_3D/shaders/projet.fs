#version 430
precision highp float;

#define M_PI 3.14159265358979

in vec3 v_pos;
in vec3 v_norm;
in vec2 tex_coord;
in mat3 TBN; 

// OUTPUT
out vec4 oFragmentColor;

//UNIFORM
layout(location = 4) uniform vec3 uLightIntensity;
layout(location = 5) uniform vec3 uLightPos;
layout(location = 6) uniform vec3 uKa;
layout(location = 7) uniform vec3 uKd;
layout(location = 8) uniform vec3 uKs;
layout(location = 9) uniform float uNs;
layout(location = 10) uniform bool normalMapping;
layout (location = 11) uniform float gamma;
layout (location = 12) uniform float exposure;


layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D uNormals;


// MAIN PROGRAM
void main()
{
    vec3 normal;
    vec4 tex = texture(uTex, tex_coord);
    vec3 normalTex = texture(uNormals, tex_coord).xyz;

    vec3 Id = vec3(0.f);
    vec3 hdrColor = vec3(0.f);

    if (normalMapping){
        normal = normalize(normalTex * 2.0 - 1.0);
    } else normal = normalize(v_norm);

    if(gl_FrontFacing == false) normal = -normal;

    vec3 lightDir = normalize(uLightPos - v_pos);

    float diffuseTerm;

    if (normalMapping){
        diffuseTerm = max(0.f, dot(normal, TBN * lightDir));
    } else diffuseTerm = max(0.f, dot(normal, lightDir)); 

    Id = vec3(tex) * uLightIntensity * vec3(diffuseTerm);

    hdrColor += Id;

    // Gamma correction and tonemapping
    hdrColor.x = pow(hdrColor.x, 1.f / gamma);
    hdrColor.y = pow(hdrColor.y, 1.f / gamma);
    hdrColor.z = pow(hdrColor.z, 1.f / gamma);

    hdrColor = 1 - exp(-hdrColor * exposure);

    oFragmentColor = vec4(hdrColor, 1.f);
}