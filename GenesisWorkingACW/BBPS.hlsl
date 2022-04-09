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
// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    //float cent : Float;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float col = 1 - smoothstep(0, 0.5, length(input.uv - float2(0.5, 0.5)));
    if (col < 0.8)
        discard;
    return (float4) col;
   
  
}
