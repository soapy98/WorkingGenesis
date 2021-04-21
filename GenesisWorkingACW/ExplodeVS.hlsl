cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

cbuffer CameraPositionConstantBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}
cbuffer TotalTimeConstantBuffer : register(b2)
{
    float time;
    float3 pad;
}
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float3 modelPosition = mul(float4(input.pos.xyz, 1.0f), model).xyz;
    float dist = distance(modelPosition, cameraPosition);
    output.pos = float4(input.pos.xyz, 1);
    
	// Transform the vertex position into projected space.
    //pos = mul(pos, model);
    //pos = mul(pos, view);
    //pos = mul(pos, projection);
    //output.pos = pos;


    
	// Pass the color through without modification.
    output.color = float4(1, 1, 1, 1);

    return output;
}