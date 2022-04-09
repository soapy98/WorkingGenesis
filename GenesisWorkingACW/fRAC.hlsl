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

vec3 palette(float d)
{
    return mix(vec3(0.2, 0.7, 0.9), vec3(1., 0., 1.), d);
}

vec2 rotate(vec2 p, float a)
{
    float c = cos(a);
    float s = sin(a);
    return mul(p, mat2(c, s, -s, c));
}

float map(vec3 p)
{
    for (int i = 0; i < 8; ++i)
    {
        float t = iTime * 0.2;
        p.xz = rotate(p.xz, t);
        p.xy = rotate(p.xy, t * 1.89);
        p.xz = abs(p.xz);
        p.xz -= .5;
    }
    return dot(sign(p), p) / 5.;
}
struct outputPS
{
    float4 colour : SV_TARGET;
    float depth : SV_DEPTH;
};
float4 rm(vec3 ro, vec3 rd)
{
    float t = 0.;
    vec4 col = (vec4) (0.);
    float d = 0;
    for (float i = 0.; i < 64.; i++)
    {
        vec3 p = ro + rd * t;
        d = map(p) * .5;
        if (d < EPSILON)
        {
            col += float4(d, col.yzw);
        }
        if (d > MAX_DIST)
        {
            discard;
        }
        //col+=vec3(0.6,0.8,0.8)/(400.*(d));
        col += float4(palette(length(p) * .1) / (400. * (d)), 1);
        t += d;
    }
    return mul(col, 1. / (d * 100.));
  
    //pv = mul(pv, projection);
    //outout.depth = pv.z / pv.w;
    //outout.colour = float4(surfacePoint, 1);
    //return outout;

}


struct Ray
{
    float3 o; //origin
    float3 d; //direction

};
float4 main(PixelShaderInput input) : SV_Target
{
    outputPS output;

    float3 PixelPos = float3(input.canvasXY, -1);
    float4 fragColor = (float4) 0;

    Ray eyeray;
    eyeray.o = mul(float4(float3(0.0f, 0.0f,10.0f), 1.0f), inverse);
    eyeray.d = normalize(mul(float4(PixelPos, 0.0f), inverse));
      return rm(eyeray.o, eyeray.d);

    
}