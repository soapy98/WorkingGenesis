Texture2D txDiffuse : register(t0);
SamplerState samp1 : register(s0);

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 viewDir : TEXCOORD3;
};
float4 main(PixelShaderInput input) : SV_TARGET
{
    return txDiffuse.Sample(samp1, input.tex);
    
}