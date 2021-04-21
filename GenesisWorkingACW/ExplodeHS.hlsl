struct HullShaderInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float tessellationFactor : TESS;
};

struct PatchConstantOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DomainShaderInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};

PatchConstantOutput PatchConstantFunction(InputPatch<HullShaderInput, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
    PatchConstantOutput output;
    float te = inputPatch[0].tessellationFactor;
    output.edges[0] = output.edges[1] = output.edges[2] = output.edges[3] = 100;

    output.inside[0] = output.inside[1] = 100;

    return output;
}

[domain("quad")]
//[partitioning("integer")]
//[partitioning("fractional_odd")]
[partitioning("fractional_even")]
//[outputtopology("point")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
[maxtessfactor(64.0f)]
DomainShaderInput main(InputPatch<HullShaderInput, 3> inputPatch, uint outputControlPointID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainShaderInput output;
    output.position = inputPatch[outputControlPointID].position;
    output.tex = inputPatch[outputControlPointID].tex;
    output.normal = inputPatch[outputControlPointID].normal;


    return output;
}