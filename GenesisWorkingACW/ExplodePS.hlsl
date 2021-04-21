#define vec2 float2
#define vec3 float3
#define vec4 float4

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};
cbuffer CameraBuffer : register(b1)
{
    float3 cameraPos;
    float cameraPadding;
};
cbuffer TotalTimeConstantBuffer : register(b2)
{
    float time;
    float3 pad;
}
// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    //float cent : Float;
};
float dist(vec2 p0, vec2 pf)
{
    return sqrt((pf.x - p0.x) * (pf.x - p0.x) + (pf.y - p0.y) * (pf.y - p0.y));
}

//void mainImage(out vec4 fragColor, in vec2 fragCoord)
//{
//    //mouse controlled version
//    //float d = dist(iResolution.xy*0.5,fragCoord.xy)*(iMouse.x/iResolution.x+0.1)*0.01;
    
//    //automatic version
//    float d = dist(iResolution.xy * 0.5, fragCoord.xy) * (sin(5) + 1.5) * 0.003;
//    fragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), d);
//}

// A pass-through function for the (interpolated) color data.

float4 main(PixelShaderInput input) : SV_TARGET
{
 //   float col = 1 - smoothstep(0, 0.4, length(input.uv - float2(0.5, 0.5)));
 //float4 c=  (float4) col;
    float distfromcenter = distance(float2(0.5, 0.5), input.uv);
    float4 rColor = lerp(float4(0, 0, time * 5, 1), float4(time * 2, 1, 1, 1), saturate(distfromcenter));
    return rColor;
}
  