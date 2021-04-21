
struct VSInput
{
    float4 pos : POSITION;
};
struct HSInput
{
	//float4 position : SV_POSITION;
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    //float tessellationFactor : TESS;
};

HSInput main(VSInput input)
{
    HSInput output;
    output.position = input.pos;
    output.tex = (sign(input.pos.xy) + 1.0) / 2.0;
    //output.normal = mul(input.pos.xyz, (float3x3) model);
    return output;
}