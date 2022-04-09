// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};
cbuffer CameraConstantBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 Pos : POSITION;
};
float4x4 scale=
{
2,0,0,0,
0,2,0,0,
0,0,2,0,
0,0,0,1
};

struct PSIn
{
    float4 Pos : SV_Position;
    float2 tex : TEXCOORD;
    float3 viewDir : TEXCOORD3;
};
PSIn main(VertexShaderInput input)
{
    PSIn output;

    float4 inPos = float4(input.Pos, 1);
    output.tex = (sign(inPos.xy) + 1.0) / 2.0;
    output.viewDir = cameraPosition.xyz -inPos.xyz;
    //inPos.xyz += cameraPosition;
//sky position to clipping space
    output.Pos = mul(inPos, model);
    
    output.Pos = mul(output.Pos, view);
    output.Pos = mul(output.Pos, projection);
    return output;
}