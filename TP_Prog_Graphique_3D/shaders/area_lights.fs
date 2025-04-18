#version 430
precision highp float;

out vec4 frag_out;

in vec3 w_pos;
in vec3 w_norm;
in vec2 texcoord;

layout (location = 1) uniform mat4 model;
layout (location = 2) uniform mat4 view;
layout (location = 8) uniform float intensity;
layout (location = 9) uniform vec3 spec_color;
layout (location = 10) uniform vec3 diff_color;
layout (location = 11) uniform float roughness;

layout (location = 12) uniform vec3 pt1;
layout (location = 13) uniform vec3 pt2;
layout (location = 14) uniform vec3 pt3;
layout (location = 15) uniform vec3 pt4;
layout (location = 17) uniform bool double_sided;
layout (location = 18) uniform bool texture_based;

layout (binding = 2) uniform sampler2D f_texture;
layout (binding = 3) uniform sampler2D f_roughness;
layout (binding = 4) uniform sampler2D f_displacement;
layout (binding = 5) uniform sampler2D f_normal;


uniform sampler2D LTC1;
uniform sampler2D LTC2;

const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;


// ===================================================================================================

/* Cette ressource a �t� utilis�e pour certaines parties de code : https://blog.selfshadow.com/ltc/webgl/ltc_quad.html */

// Vector form without project to the plane (dot with the normal)
// Use for proxy sphere clipping
vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided) {
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = Minv * transpose(mat3(T1, T2, N));

    // polygon (allocate 4 vertices for clipping)
    vec3 L[4];
    // transform polygon from LTC back to origin Do (cosine weighted)
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);
    L[3] = Minv * (points[3] - P);

    // use tabulated horizon-clipped sphere
    // check if the shading point is behind the light
    vec3 dir = points[0] - P; // LTC space
    vec3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0);

    // cos weighted space
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    // integrate
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    // form factor of the polygon in direction vsum
    float len = length(vsum);

    float z = vsum.z/len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z*0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(LTC2, uv).w;

    float sum = len*scale;
    if (!behind && !twoSided)
        sum = 0.0;

    // Outgoing radiance (solid angle) for the entire polygon
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

// PBR-maps for roughness (and metallic) are usually stored in non-linear
// color space (sRGB), so we use these functions to convert into linear RGB.
vec3 PowVec3(vec3 v, float p)
{
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}
// =================================================================================================== 


const float gamma = 2.2;
vec3 ToLinear(vec3 v) { return PowVec3(v, gamma); }
vec3 ToSRGB(vec3 v)   { return PowVec3(v, 1.0/gamma); }


void main()
{

    vec3 mDiffuse = texture(f_texture, texcoord).xyz;
    vec4 t_roughness = texture(f_roughness, texcoord); 
	//extraire la position de camera
	mat4 viewModel = inverse(model * view);
	vec3 cameraPos = vec3(viewModel[3]);

    vec3 result = vec3(0.0f);

    vec3 pts[4] = {pt1, pt2, pt3, pt4};

    vec3 N = normalize(w_norm);
    vec3 V = normalize(cameraPos - w_pos);
    vec3 P = w_pos;
    vec2 uv = vec2(0.0f);
    float dotNV = clamp(dot(N, V), 0.0f, 1.0f);
    if (texture_based){
        uv = vec2(t_roughness.w, sqrt(1.0 - dotNV));
    }
    else {
        uv = vec2(roughness, sqrt(1.0 - dotNV));
    }
    uv = uv*LUT_SCALE + LUT_BIAS;

    // get 4 parameters for inverse_M
    vec4 t1 = texture(LTC1, uv);

    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(LTC2, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    vec3 diff = LTC_Evaluate(N, V, P, mat3(1), pts, double_sided);

    vec3 spec = LTC_Evaluate(N, V, P, Minv, pts, double_sided);
    spec *= ToLinear(spec_color)*t2.x + (1.0 - ToLinear(spec_color))*t2.y;

    if (texture_based) { result = intensity*(spec + mDiffuse * diff) ; }
    else { result = intensity * (spec + diff_color * diff) ; }

	frag_out = vec4(ToSRGB(result), 1.0f);
}