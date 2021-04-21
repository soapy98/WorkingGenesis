cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

cbuffer InverseConstantBuffer : register(b1)
{
    matrix inverse;
}

cbuffer CameraConstantBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
}
cbuffer TotalTimeBuffer : register(b3)
{
    float time;
}
#define vec3 float3
#define vec2 float2
#define vec4 float4
#define LOWDETAIL
//#define HIGH_QUALITY_NOISE

struct VS_QUAD
{
    float4 Position : SV_POSITION;
    float2 canvasXY : TEXCOORD0;
};


struct outputPS
{
    float4 colour : SV_TARGET;
    float depth : SV_DEPTH;
};
struct Ray
{
    float3 o; // origin
    float3 d; // direction
};

const float3x3 rotationMatrix = float3x3(1.0, 0.0, 0.0, 0.0, 0.7, -0.7, 0.0, 0.7, 0.7);

float hash(vec2 p)
{
	vec3 p3 = frac(vec3(p.xyx) * 10.21);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float noise(vec2 P)
{
    float size = 256.0;
    float s = (1.0 / size);
    vec2 pixel = P * size + 0.5;
    vec2 f = frac(pixel);
    pixel = (floor(pixel) / size) - vec2(s / 2.0, s / 2.0);
    float C11 = hash(pixel + vec2(0.0, 0.0));
    float C21 = hash(pixel + vec2(s, 0.0));
    float C12 = hash(pixel + vec2(0.0, s));
    float C22 = hash(pixel + vec2(s, s));
    float x1 = lerp(C11, C21, f.x);
    float x2 = lerp(C12, C22, f.x);
    return lerp(x1, x2, f.y);
}

float fbm(vec2 p)
{
    float a = 0.5, b = 0.0, t = 0.0;
    for (int i = 0; i < 5; i++)
    {
        b *= a;
        t *= a;
        b += noise(p);
        t += 1.0;
        p /= 2.0;
    }
    return b /= t;
}

float map(vec3 p)
{
    float h = p.y - 20.0 * fbm(p.xz * 0.003);
    return max(min(h, 0.55), p.y - 20.0);
}

bool raymarch(inout vec3 ro, vec3 rd)
{
    float t = 0.0;
    for (int i = 0; i < 128; i++)
    {
        float d = map(ro + rd * t);
        t += d;
        if (d < t * 0.001)
        {
            ro += t * rd;
            return true;
        }
    }
    return false;
}

vec3 shading(vec3 ro, vec3 rd)
{
    vec3 c = (rd.y * 2.0) * 0.1;
    vec3 sk = ro;
    if (raymarch(ro, rd))
    {
        vec2 p = ro.xz;
        vec2 g = abs(frac(p - 0.5) - 0.5) / fwidth(p);
        float s = min(g.x, g.y);
        float f = min(length(ro - sk) / 64., 1.);
        return lerp(1.5 - vec3(s, s, s), c, f);
    }
    return (vec3)0.0;
}
static float MIN_DIST = 1.0;
static float MAX_DIST = 100.0;
static float EPSILON = 0.0001;
outputPS main(VS_QUAD input):SV_Target
{
    outputPS output;
    
    float3 PixelPos = float3(input.canvasXY, -1);


    Ray eyeray;
    eyeray.o = mul(float4(float3(2.0f, 10.0f, 2.0f), 1.0f), inverse);
    eyeray.d = normalize(mul(float4(PixelPos, 0.0f), inverse));
    
    float4 distanceAndColour = float4(shading(eyeray.o, eyeray.d), 1);
    if (distanceAndColour.x > MAX_DIST - EPSILON)
    {
        discard;
    }

    float3 surfacePoint = cameraPosition + distanceAndColour.x * eyeray.d;

    float4 pv = mul(float4(surfacePoint, 1.0f), view);
    pv = mul(pv, projection);
    output.depth = pv.z / pv.w;

     output.colour = float4(distanceAndColour.yzw, 1.0f);

    return output; //float4(lerp(colour.xyz, float3(1.0f, 0.97255f, 0.86275f), 1.0 - exp(-0.0005 * distanceAndColour.x * distanceAndColour.x * distanceAndColour.x)), 1.0f);
    
	
    
    
    
    //return vec4(shading(ro, rd), 1.0);
}
//outputPS main(VS_QUAD input) : SV_Target
//{

//    outputPS output;

//    float3 PixelPos = float3(input.canvasXY, -5);


//    Ray eyeray;
//    eyeray.o = mul(float4(float3(0.0f, 0.0f, 0.0f), 1.0f), inverse);
//    eyeray.d = normalize(mul(float4(PixelPos, 0.0f), inverse));
    
//    float4 distanceAndColour = float4(render(eyeray.o, eyeray.d), 1);
//    if (distanceAndColour.x < MAX_DIST - 0.0001)
//    {
//        discard;
//    }

//    float3 surfacePoint = cameraPosition + distanceAndColour.x * eyeray.d;

//    float4 pv = mul(float4(surfacePoint, 1.0f), view);
//    pv = mul(pv, projection);
//    output.depth = pv.z / pv.w;

//    output.colour = float4(distanceAndColour.yzw, 1.0f);

//    output.colour = float4(lerp(output.colour.xyz, float3(1.0f, 0.97255f, 0.86275f), 1.0 - exp(-0.0005 * distanceAndColour.x * distanceAndColour.x * distanceAndColour.x)), 1.0f);
//    return output;
	
	// render
	
   //return vec4(color, 1.0);
//}
//-----------------------------------------------------------------------------------
//https://www.shadertoy.com/view/Ws3XRs