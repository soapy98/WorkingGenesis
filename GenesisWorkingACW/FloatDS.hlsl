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


static float PI = 3.14159265359;

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
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
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
float CubicHermite(float A, float B, float C, float D, float t)
{
    float a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
    float b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
    float c = -A / 2.0f + C / 2.0f;
    float d = sin(B * time);
  
    return a * t * t * t + b * t * t + c * t + d;
}
float4 EvaluateCubic(float4 base)
{
    return float4(base.x + base.y, base.y / 3, -base.z / 3, base.z + base.w);
}
float4 BernsteinBasis(float t)
{
    float invT = 1.0f - t;

    return float4(invT * invT * invT, 3.0f * t * invT * invT, 3.0f * t * t * invT, t * t * t);
}
[domain("quad")]
PixelShaderInput main(in PatchConstantOutput input, in const float2 domain : SV_DomainLocation, const OutputPatch<DomainShaderInput, 4> patch)
{
    PixelShaderInput output;
    float4 base = BernsteinBasis(domain.x);
    output.positionW = EvaluateCubic(base);
    //output.positionW.x = CubicHermite(domain.x * patch[0].position.x, domain.x * patch[1].position.x, domain.x * patch[2].position.x, domain.x * patch[3].position.x, 109);
    //output.positionW.y = CubicHermite(domain.y * patch[0].position.y, domain.y * patch[1].position.y, domain.y * patch[2].position.y, domain.y * patch[3].position.y, 2);
    output.positionW.z = 1.0f * cos((domain.y + 0.5f) * (2 * PI));
    //output.positionW *= sin(time);
    output.viewDirection = normalize(cameraPos.xyz - output.positionW);
    output.tex = (sign(output.positionW.xy) + 1.0) / 2.0;
    float3 p0 = mul(float4(patch[0].position, 1.0f), model);

    float3 p1 = mul(float4(patch[1].position, 1.0f), model);

    float3 p2 = mul(float4(patch[2].position, 1.0f), model);
    float3 d1 = p1 - p0;
    float3 d2 = p2 - p0;
    output.normal = normalize(cross(d1, d2));
    output.tex = domain.x * patch[0].tex + domain.y * patch[1].tex + domain.x * patch[2].tex + domain.y * patch[3].tex;
    output.normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.x * patch[2].normal + domain.y * patch[3].normal;
    output.tangent = domain.x * patch[0].tangent + domain.y * patch[1].tangent + domain.x * patch[2].tangent + domain.y * patch[3].tangent;
    output.binormal = domain.x * patch[0].binormal + domain.y * patch[1].binormal + domain.x * patch[2].binormal + domain.y * patch[3].binormal;

    output.tangent = normalize(output.tangent);
    output.binormal = normalize(output.binormal);
    
    //float height = noise(cos(output.positionW.y*time) );
   
    	//Displacement mapping
    float mipLevel = clamp((distance(output.normal, cameraPos) + 20.0f) / 20.0f, 0.0f, 10.0f);

	//Sample height map
    //Cool overlap effect
    //float height = noise(cos(output.positionW.y * time));
    //output.positionW += normalize(output.positionW) * (2 * (height - 0.6));
    
    
    
    float height = noise(sin(output.positionW * time));
    //output.positionW += normalize(output.positionW) * (2 * (height - 0.6));
    output.positionH = mul(float4(output.positionW, 1.0f), model);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);
    return output;
}