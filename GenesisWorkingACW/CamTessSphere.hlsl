cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};
cbuffer CameraBuffer : register(b1)
{
    float3 cameraPos;
    float cameraPadding;
};
struct HullShaderInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
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
 
};

PatchConstantOutput PatchConstantFunction(InputPatch<HullShaderInput, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    PatchConstantOutput output;
    float3 modelPosition = mul(float4(inputPatch[patchId].position, 1.0f), model).xyz;
    float distanceToCamera = distance(modelPosition, cameraPos);

    float tess = saturate((5.0f - distanceToCamera) / (5.0f - 1.0f));

    tess = 1.0f + tess * (64.0f - 1.0f);
 
        
    output.edges[0] = output.edges[1] = output.edges[2] = output.edges[3] = tess;

    output.inside[0] = output.inside[1] = tess;

    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
//[outputtopology("point")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
[maxtessfactor(64.0f)]
DomainShaderInput main(InputPatch<HullShaderInput, 4> inputPatch, uint outputControlPointID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainShaderInput output;
    output.position = inputPatch[outputControlPointID].position;
    output.tex = inputPatch[outputControlPointID].tex;


    return output;
}