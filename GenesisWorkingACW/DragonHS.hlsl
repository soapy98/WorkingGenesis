struct HullShaderInput
{
    float3 position : POSITION;
};

struct PatchConstantOutput
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct DomainShaderInput
{
    float3 position : BEZIERPOS;
};

PatchConstantOutput PatchConstantFunction(InputPatch<HullShaderInput, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
    PatchConstantOutput output;

    float tessellationFactor = 100.0f;

    output.edges[0] = output.edges[1] = output.edges[2] = tessellationFactor;

    output.inside =  tessellationFactor;

    return output;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
DomainShaderInput main(InputPatch<HullShaderInput, 3> inputPatch, uint outputControlPointID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainShaderInput output;
    output.position = inputPatch[outputControlPointID].position;

    return output;
}