
struct DSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};
struct HSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    //float tessellationFactor : TESS;
};
struct HS_CONTROL_POINT_OUTPUT
{
    float3 vPosition : WORLDPOS;
    float3 color : COLOR0;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};



#define NUM_CONTROL_POINTS 4

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HSInput, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.EdgeTessFactor[0] =
		Output.EdgeTessFactor[1] =
		Output.EdgeTessFactor[2] = Output.EdgeTessFactor[3] = 302;
    Output.InsideTessFactor[0] = Output.InsideTessFactor[1] = 15;

    return Output;
}

[domain("quad")]
//[partitioning("integer")]
//[partitioning("fractional_odd")]
[partitioning("fractional_even")]
//[outputtopology("point")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
[maxtessfactor(64.0f)]
DSInput main(InputPatch<HSInput, 4> ip, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DSInput output;
    output.pos = ip[i].pos;
    output.tex = ip[i].tex;
    output.normal = ip[i].normal;
    output.tangent = ip[i].tangent;
    output.binormal = ip[i].binormal;
;
    return output;
}