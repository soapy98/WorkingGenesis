cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

cbuffer InverseViewConstantBuffer : register(b1)
{
    matrix inverse;
}

cbuffer CameraConstantBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
}

cbuffer timeConstantBuffer : register(b3)
{
    float time;
    float3 padding2;
}
#define vec3 float3
#define vec4 float4
#define mat4  float4x4
#define fract frac
#define mix lerp
#define iTime time
#define vec2 float2
#define mat3 float3x3
#define mat2 float2x2
// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 canvasXY : TEXCOORD0;
};
static int MAX_MARCHING_STEPS = 255;
static float MIN_DIST = 1.0;
static float MAX_DIST = 100.0;
static float EPSILON = 0.0001;



#define iterations 17
#define formuparam 0.53

#define volsteps 20
#define stepsize 0.1

#define zoom   0.800
#define tile   0.850
#define speed  0.010 

#define brightness 0.0015
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.850


float4 main(PixelShaderInput input) : SV_Target
{
	//get coords and direction
    vec2 uv = input.canvasXY;
    vec3 dir = vec3(uv * zoom, 1.);

	//mouse rotation
    //float a1 = .5 + iMouse.x / iResolution.x * 2.;
    //float a2 = .8 + iMouse.y / iResolution.y * 2.;
    //mat2 rot1 = mat2(cos(a1), sin(a1), -sin(a1), cos(a1));
    //mat2 rot2 = mat2(cos(a2), sin(a2), -sin(a2), cos(a2));
    //dir.xz *= rot1;
    //dir.xy *= rot2;
   vec3 from = vec3(1., .5, 0.5);
    from += vec3(time * 2., time, -2.);
    //from.xz *= rot1;
    //from.xy *= rot2;
	
	//volumetric rendering
    float s = 0.1, fade = 1.;
    vec3 v = (vec3) (0.);
    for (int r = 0; r < volsteps; r++)
    {
        vec3 p = from + s * dir * .5;
        p = abs((vec3) (tile) - fmod(p, (vec3) (tile * 2.))); // tiling fold
        float pa, a = pa = 0.;
        for (int i = 0; i < iterations; i++)
        {
            p = abs(p) / dot(p, p) - formuparam; // the magic formula
            a += abs(length(p) - pa); // absolute sum of average change
            pa = length(p);
        }
        float dm = max(0., darkmatter - a * a * .001); //dark matter
        a *= a * a; // add contrast
        if (r > 6)
            fade *= 1. - dm; // dark matter, don't render near
		//v+=vec3(dm,dm*.5,0.);
        v += fade;
        v += vec3(s, s * s, s * s * s * s) * a * brightness * fade; // coloring based on distance
        fade *= distfading; // distance fading
        s += stepsize;
    }
    v = mix((vec3) (length(v)), v, saturation); //color adjust
    return vec4(v * .01, 1.);
	
}