cbuffer timeConstantBuffer : register(b0)
{
    float time;
    float3 padding2;
}

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR0;
};
float3 colorA = float3(0.149, 0.141, 0.912);
float3 colorB = float3(1.000, 0.833, 0.224);
// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float pct = abs(sin(time));
	// Pass the color through without modification.
    input.color = lerp(colorB, colorA, pct);
    return float4(input.color, 1.0f);
}
