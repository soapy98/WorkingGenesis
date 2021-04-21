// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};
cbuffer Camera : register(b1)
{
    float3 CamPos;
    float padding;
}
struct DSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
 
    float3 viewDirection : TEXCOORD1;
};


struct PatchConstantOutput
{
    float EdgeTessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};


#define NUM_CONTROL_POINTS 16
static float PI = 3.14159265359;
[domain("quad")]
PSInput main(in PatchConstantOutput input, float2 domain : SV_DomainLocation, const OutputPatch<DSInput, 4> patch)
{
    PSInput output;
    
    output.positionW.x = (1.0f + 0.5f * cos((domain.y + 0.5f) * (2 * PI))) * cos((domain.x + 0.5f) * (2 * PI));
    output.positionW.y = (1.0f + 0.5f * cos((domain.y + 0.5f) * (2 * PI))) * sin((domain.x + 0.5f) * (2 * PI));
    output.positionW.z = 0.5f * sin((domain.y + 0.5f) * (2 * PI));


    output.viewDirection = normalize(CamPos.xyz - output.positionW);

    output.positionH = mul(float4(output.positionW, 1.0f), model);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);

    return output;
    
}