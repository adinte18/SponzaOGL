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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w * 0.5f + 0.5f;

    float bias = max(0.05f * (1.f - dot(normal, lightDir)), 0.005f);
    //projCoords.z -= bias;

    float currentDepth = projCoords.z;

    float closestDepth= texture(uShadow, projCoords.xy).r;

    if (currentDepth >= 0.99f) return 1.f;

    //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(uShadow, 0);
    for (int x = -1; x <= 1; ++x){
        for (int y = -1; y <= 1; ++y){
            float pcfDepth = texture(uShadow, projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if (projCoords.z > 1.0) shadow = 0.0f;

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

	vec3 Ia = uLightIntensity * uKa;

    if (normalMapping){
        diffuseTerm = max(0.0, dot(normal, TBN * lightDir));
    } else diffuseTerm = max(0.0, dot(normal, lightDir)); 

    Id = vec3(tex) * uLightIntensity * vec3(diffuseTerm);

	vec3 Is = vec3(0.0);
    if (diffuseTerm > 0.0)
	{
		vec3 viewDir = normalize(-v_pos.xyz);
		vec3 halfDir = normalize(viewDir + lightDir);
		float specularTerm = max(0.0, pow(dot(normal, halfDir), uNs));
		Is = uLightIntensity * uKs * vec3(specularTerm);
		Is /= (uNs + 2.f) / (2.f * M_PI);
	}

    hdrColor = 0.3*Id + 0.3*Ia + 0.3*Is;

    float luminance = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));

    float mappedExposure = 1.0 - exp(-exposure * luminance);
    vec3 mappedColor = hdrColor * mappedExposure / (luminance + 0.01);

    mappedColor = pow(mappedColor, vec3(1.0 / gamma));


    // Shadow calculation
    float shadow = ShadowCalculation(FragPosLightSpace, lightDir, normal);

    float shadowIntensity = 0.4f;
    vec3 shColor = mix(mappedColor, mappedColor* (1.0 - shadow), shadowIntensity);

    oFragmentColor = vec4(shColor, 1.0);
}
