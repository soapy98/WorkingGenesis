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
//struct DS_OUTPUT
//{
//    float4 vPosition : SV_POSITION;
//};

//struct HS_CONTROL_POINT_OUTPUT
//{
//    float3 vPosition : WORLDPOS;
//};
////
////struct HS_CONSTANT_DATA_OUTPUT
////{
////	float EdgeTessFactor[3]			: SV_TessFactor;
////	float InsideTessFactor			: SV_InsideTessFactor;
////};
//struct HS_CONSTANT_DATA_OUTPUT
//{
//    float EdgeTessFactor[4] : SV_TessFactor;
//    float InsideTessFactor[2] : SV_InsideTessFactor;
//};


static float PI = 3.14159265359;
//[domain("quad")]
//DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch)
//{
//    DS_OUTPUT Output;
//    //Output.vPosition.x = 1.0f * cos((domain.x + 0.5f) * (2 * PI)) * sin((domain.y + 0.5f) * (2 * PI)) * patch[0].vPosition;
//    //Output.vPosition.y = 1.0f * sin((domain.x + 0.5f) * (2 * PI)) * sin((domain.y + 0.5f) * (2 * PI)) * patch[1].vPosition;
//    //Output.vPosition.z = 1.0f * cos((domain.y + 0.5f) * (2 * PI)) * patch[2].vPosition;
//    //Output.vPosition.w = 1;
//    Output.vPosition = float4(domain.x * patch[0].vPosition + domain.y * patch[1].vPosition * patch[2].vPosition, 1);
//    Output.vPosition = mul(float4(Output.vPosition.xyz, 1.0f), model);
//    Output.vPosition = mul(Output.vPosition, view);
//    Output.vPosition = mul(Output.vPosition, projection);
//    return Output;
//}
// A constant buffer that stores the three basic column-major matrices for composing geometry.
struct PatchConstantOutput
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};
struct DomainShaderInput
{
    float3 position : BEZIERPOS;
};

struct PixelShaderInput
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    //float3 tangent : TANGENT;
    //float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};
float3 calcNormal(float3 a, float3 b, float3 c)
{
    return normalize(cross(b - a, c - a));
}

float3 calcTangent(float3 v1, float3 v2, float2 tuVector, float2 tvVector)
{
    float3 tangent;
    float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

    tangent.x = (tvVector.y * v1.x - tvVector.x * v2.x) * den;
    tangent.y = (tvVector.y * v1.y - tvVector.x * v2.y) * den;
    tangent.z = (tvVector.y * v1.z - tvVector.x * v2.z) * den;

    float length = sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);

    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    return tangent;
}

float3 calcBinormal(float3 v1, float3 v2, float2 tuVector, float2 tvVector)
{
    float3 binormal;
    float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

    binormal.x = (tuVector.x * v2.x - tuVector.y * v1.x) * den;
    binormal.y = (tuVector.x * v2.y - tuVector.y * v1.y) * den;
    binormal.z = (tuVector.x * v2.z - tuVector.y * v1.z) * den;

    float length = sqrt(binormal.x * binormal.x + binormal.y * binormal.y + binormal.z * binormal.z);

    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;

    return binormal;
}


float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

float noise(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);

    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0 + 113.0 * p.z;

    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
		lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
		lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
			lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}
float3 SphereNormal(float3 cent, float3 pos)
{
    return normalize(pos - cent);
}
[domain("tri")]
PixelShaderInput main(in PatchConstantOutput input, in const float3 domain : SV_DomainLocation, const OutputPatch<DomainShaderInput, 3> patch)
{
    PixelShaderInput output;
    output.positionW.x = 1.0f * cos((domain.x + 0.5f) * (2 * PI)) * sin((domain.y + 0.5f) * (2 * PI)) * patch[0].position;
    output.positionW.y = 1.0f * sin((domain.x + 0.5f) * (2 * PI)) * sin((domain.y + 0.5f) * (2 * PI)) * patch[1].position;
    output.positionW.z = 1.0f * cos((domain.z + 0.5f) * (2 * PI)) * patch[2].position;
   
    output.viewDirection = normalize(cameraPos.xyz - output.positionW);
    output.uv = (sign(output.positionW.xy) + 1.0) / 2.0;
    float3 p0 = mul(float4(patch[0].position, 1.0f), model);

    float3 p1 = mul(float4(patch[1].position, 1.0f), model);

    float3 p2 = mul(float4(patch[2].position, 1.0f), model);
    float3 d1 = p1 - p0;
    float3 d2 = p2 - p0;
    
    float height = noise(output.positionW);
    output.normal = normalize(cross(d1, d2));
    
    //output.positionW += height * float4(output.normal,1);
    output.positionH = mul(float4(output.positionW, 1.0f), model);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);
    return output;
}