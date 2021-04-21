struct HullShaderInput
{
    float3 position : POSITION;
};

struct PatchConstantOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DomainShaderInput
{
    float3 position : BEZIERPOS;
};

PatchConstantOutput PatchConstantFunction(InputPatch<HullShaderInput, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    PatchConstantOutput output;

    float tessellationFactor = 100.0f;

    output.edges[0] = output.edges[1] = output.edges[2] = output.edges[3] = tessellationFactor;

    output.inside[0] = output.inside[1] = tessellationFactor;

    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
DomainShaderInput main(InputPatch<HullShaderInput, 4> inputPatch, uint outputControlPointID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainShaderInput output;
    output.position = inputPatch[outputControlPointID].position;

    return output;
}