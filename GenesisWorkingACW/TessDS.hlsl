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
static float PI = 3.14159265359;

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
    float3 positionW : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
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
float3 rand3(float3 c)
{
    float j = 4096.0 * sin(dot(c, float3(17.0, 59.4, 15.0)));
    float3 r;
    r.z = frac(512.0 * j);
    j *= .125;
    r.x = frac(512.0 * j);
    j *= .125;
    r.y = frac(512.0 * j);
    return r - 0.5;
}
float _snoise(float3 p)
{
    const float F3 = 0.3333333;
    const float G3 = 0.1666667;
    float3 s = floor(p + dot(p, float3(F3, F3, F3)));
    float3 x = p - s + dot(s, float3(G3, G3, G3));
	 
    float3 e = step(float3(0.0, 0.0, 0.0), x - x.yzx);
    float3 i1 = e * (1.0 - e.zxy);
    float3 i2 = 1.0 - e.zxy * (1.0 - e);
	 	
    float3 x1 = x - i1 + G3;
    float3 x2 = x - i2 + 2.0 * G3;
    float3 x3 = x - 1.0 + 3.0 * G3;
	 
    float4 w, d;
	 
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);
	 
    w = max(0.6 - w, 0.0);
	 
    d.x = dot(rand3(s), x);
    d.y = dot(rand3(s + i1), x1);
    d.z = dot(rand3(s + i2), x2);
    d.w = dot(rand3(s + 1.0), x3);
	 
    w *= w;
    w *= w;
    d *= w;
	 
    return dot(d, float4(52.0, 52.0, 52.0, 52.0));
}

float snoisefracal(float3 m)
{
    return 0.5333333 * _snoise(m)
				+ 0.2666667 * _snoise(2.0 * m)
				+ 0.1333333 * _snoise(4.0 * m)
				+ 0.0666667 * _snoise(8.0 * m);
}
float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}
float rand2(float2 n)
{
    return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
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
PixelShaderInput main(in PatchConstantOutput input, in const float2 domain : SV_DomainLocation, const OutputPatch<DomainShaderInput, 4> patch)
{
    PixelShaderInput output;
    output.positionW.x = 1.0f * cos((domain.x + 0.5f) * (2 * PI)) * sin((domain.y + 0.5f) * (2 * PI)) * patch[0].position;
    output.positionW.y = 1.0f * sin((domain.x + 0.5f) * (2 * PI)) * sin((domain.y + 0.5f) * (2 * PI)) * patch[1].position;
    output.positionW.z = 1.0f * cos((domain.y + 0.5f) * (2 * PI)) * patch[2].position;
   
    output.viewDirection = normalize(cameraPos.xyz - output.positionW);
    output.uv = domain; //(sign(output.positionW.xy) + 1.0) / 2.0;
  
    float3 p0 = mul(float4(patch[0].position, 1.0f), model);

    float3 p1 = mul(float4(patch[1].position, 1.0f), model);

    float3 p2 = mul(float4(patch[2].position, 1.0f), model);
    float3 d1 = p1 - p0;
    float3 d2 = p2 - p0;
    output.normal = normalize(cross(d1, d2));
    output.positionH = mul(float4(output.positionW, 1.0f), model);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);
    return output;
}