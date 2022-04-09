cbuffer TotalTimeConstantBuffer : register(b0)
{
    float time;
    float3 pad;
}
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 position : POSITION;
};

// Per-pixel color data passed through the pixel shader.
struct HullShaderInput
{
    float3 position : POSITION;
};

// Simple shader to do vertex processing on the GPU.
HullShaderInput main(VertexShaderInput input)
{
    HullShaderInput output;
    float3 pos = input.position;
    
    output.position = pos;

    //output.position.y += cos(time);
    return output;
}