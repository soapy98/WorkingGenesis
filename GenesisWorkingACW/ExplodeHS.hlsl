struct HullShaderInput
{
    float4 position : SV_POSITION;
};

struct PatchConstantOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DomainShaderInput
{
    float4 position : SV_POSITION;
};

PatchConstantOutput PatchConstantFunction(InputPatch<HullShaderInput, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
    PatchConstantOutput output;
    output.edges[0] = output.edges[1] = output.edges[2] = output.edges[3] = 100;

    output.inside[0] = output.inside[1] = 10;

    return output;
}

[domain("quad")]
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

    return output;
}