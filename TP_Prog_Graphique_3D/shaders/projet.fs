#version 430
precision highp float;

#define M_PI 3.14159265358979

in vec3 v_pos;
in vec3 v_norm;
in vec2 tex_coord;
in mat3 TBN; 
in vec4 FragPosLightSpace;

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
layout (location = 15) uniform float biasMax;

layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D uNormals;
layout(binding = 2) uniform sampler2D uShadow;

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uShadow, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;

}

// MAIN PROGRAM
void main()
{
    vec3 normal;
    vec4 tex = texture(uTex, tex_coord);
    vec3 normalTex = texture(uNormals, tex_coord).xyz;

    vec3 Id = vec3(0.0);
    vec3 hdrColor = vec3(0.0);

    if (normalMapping){
        normal = normalize(normalTex * 2.0 - 1.0);
    } else normal = normalize(v_norm);

    if(gl_FrontFacing == false) normal = -normal;

    vec3 lightDir = normalize(uLightPos - v_pos);

    float diffuseTerm;

    if (normalMapping){
        diffuseTerm = max(0.0, dot(normal, TBN * lightDir));
    } else diffuseTerm = max(0.0, dot(normal, lightDir)); 

    Id = vec3(tex) * uLightIntensity * vec3(diffuseTerm);

    hdrColor += Id;

    float luminance = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));

    float mappedExposure = 1.0 - exp(-exposure * luminance);
    vec3 mappedColor = hdrColor * mappedExposure / (luminance + 0.01);

    mappedColor = pow(mappedColor, vec3(1.0 / gamma));

    // Shadow calculation
    float shadow = ShadowCalculation(FragPosLightSpace);

    vec3 shColor = (1.0 - shadow) * mappedColor;

    oFragmentColor = vec4(shColor, 1.0);
}
