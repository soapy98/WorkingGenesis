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
cbuffer TotalTimeConstantBuffer : register(b2)
{
    float time;
    float3 pad;
}
Texture2D displacementTexture;
SamplerState sampleType : register(s0);


static float PI = 3.14159265359;

// A constant buffer that stores the three basic column-major matrices for composing geometry.
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
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelShaderInput
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
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
[domain("quad")]
PixelShaderInput main(in PatchConstantOutput input, in const float2 uvCoord : SV_DomainLocation, const OutputPatch<DomainShaderInput, 3> patch)
{
    PixelShaderInput output;
    float3 p0 = mul(float4(patch[0].position, 1.0f), model);

    float3 p1 = mul(float4(patch[1].position, 1.0f), model);

    float3 p2 = mul(float4(patch[2].position, 1.0f), model);
    float3 d1 = p1 - p0;
    float3 d2 = p2 - p0;
   //calcNormal(patch[0].position, patch[1].position, patch[2].position);

    //output.normal = normalize(mul(output.normal, (float3x3) model));
	//Calculate new vertex position
    output.positionW.x = 1.0f * cos((uvCoord.x + 0.5f) * (2 * PI)) * sin((uvCoord.y + 0.5f) * (2 * PI));
    output.positionW.y = 1.0f * sin((uvCoord.x + 0.5f) * (2 * PI)) * sin((uvCoord.y + 0.5f) * (2 * PI));
    output.positionW.z = 1.0f * cos((uvCoord.y + 0.5f) * (2 * PI));
    output.tex = (sign(output.positionW.xy) + 1.0) / 2.0;
    output.tex = uvCoord.x * patch[0].tex + uvCoord.y * patch[1].tex + uvCoord.x * patch[2].tex + uvCoord.y; //* patch[3].tex;
    output.normal = uvCoord.x * patch[0].normal + uvCoord.y * patch[1].normal + uvCoord.x * patch[2].normal + uvCoord.y; // * patch[3].normal;
        //output.tangent = uvCoord.x * patch[0].tangent + uvCoord.y * patch[1].tangent + uvCoord.x * patch[2].tangent + uvCoord.y * patch[3].tangent;
        //output.binormal = uvCoord.x * patch[0].binormal + uvCoord.y * patch[1].binormal + uvCoord.x * patch[2].binormal + uvCoord.y * patch[3].binormal;

    output.normal = normalize(output.normal);
        //output.tangent = normalize(output.tangent);
        //output.binormal = normalize(output.binormal);

	//Displacement mapping
    float mipLevel = clamp((distance(output.positionW, cameraPos) - 20.0f) / 20.0f, 0.0f, 6.0f).r;

	//Sample height map
    float height = noise(sin(output.positionW.y * time));
    //height = noise(height);
    //output.positionW.y += (height * 0.5);
    //output.positionW.x += normalize(output.positionW.z) * (2 * (height - 0.5));
    output.viewDirection = normalize(cameraPos.xyz - output.positionW);

    output.positionH = mul(float4(output.positionW, 1.0f), model);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);

    return output;
}