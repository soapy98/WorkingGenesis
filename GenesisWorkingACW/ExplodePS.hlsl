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
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
};


float4 main(PixelShaderInput input) : SV_TARGET
{ 
 //   float col = 1 - smoothstep(0, 0.4, length(input.uv - float2(0.5, 0.5)));
 //float4 c=  (float4) col;
    float distfromcenter = distance(float2(0.5, 0.5), input.uv);
    float4 rColor = smoothstep(float4(lerp(time, 0.6, 1), 0, 0, 0), float4(1, 1, 1, 1), (float4) sin(time));
    return rColor;
}
  