// A constant buffer that stores the three basic column-major matrices for composing geometry.
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
//Type definitions
struct PatchConstantOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};
struct DomainShaderInput
{
    float3 position : BEZIERPOS;
};

struct PixelShaderInput
{
    float4 positionH : SV_POSITION;
    //float3 positionW : POSITION;
    //float3 normal : NORMAL;
    //float3 tangent : TANGENT;
    //float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD0;
};

float4 BernsteinBasis(float t)
{
    float invT = 1.0f - t;

    return float4(invT * invT * invT, 3.0f * t * invT * invT, 3.0f * t * t * invT, t * t * t);
}

float4 dBernsteinBasis(float t)
{
    float invT = 1.0f - t;

    return float4(-3 * invT * invT, 3 * invT * invT - 6 * t * invT, 6 * t * invT - 3 * t * t, 3 * t * t);
}

float3 EvaluateBezier(const OutputPatch<DomainShaderInput, 16> patch,
	float4 basisU,
	float4 basisV)
{
    float3 value = float3(0, 0, 0);
    value = basisV.x * (patch[0].position * basisU.x + patch[1].position * basisU.y + patch[2].position * basisU.z + patch[3].position * basisU.w);
    value += basisV.y * (patch[4].position * basisU.x + patch[5].position * basisU.y + patch[6].position * basisU.z + patch[7].position * basisU.w);
    value += basisV.z * (patch[8].position * basisU.x + patch[9].position * basisU.y + patch[10].position * basisU.z + patch[11].position * basisU.w);
    value += basisV.w * (patch[12].position * basisU.x + patch[13].position * basisU.y + patch[14].position * basisU.z + patch[15].position * basisU.w);

    return value;
}
[domain("quad")]
PixelShaderInput main(in PatchConstantOutput input, in const float2 uvCoord : SV_DomainLocation, const OutputPatch<DomainShaderInput, 16> patch)
{
    PixelShaderInput output;

    float4 basisU = BernsteinBasis(uvCoord.x);
    float4 basisV = BernsteinBasis(uvCoord.y);
   
    float3 p = EvaluateBezier(patch, basisU, basisV);

    output.viewDirection = normalize(cameraPos.xyz - p);
    float4 pos = float4(p, 1);
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.positionH = pos;
    return output;
}