#define NUMBER_OF_LIGHTS 1

struct Light
{
    float3 ambientColour;
    float3 diffuseColour;
    float3 specularColour;
    float3 lightPosition;
    float specularPower;
};
Texture2D txDiffuse : register(t0);
SamplerState samp1 : register(s0);
static Light lights[NUMBER_OF_LIGHTS] =
{
	//LightOne
    { 0.2, 0.2, 0.2, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.0, 20.0, 0.0, 20 },
	//LightTwo
	//{0.2, 0.2, 0.2, 1.0, 0.4, 0.4, 0.4, 1.0, 0.4, 0.4, 0.4, 1.0, 0.0, -5.0, 0.0, 20},
};

struct PixelShaderInput
{
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 viewDirection : TEXCOORD1;
};
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
float4 PhongIllumination(float3 pos, float3 normal, float3 viewDir, float3 diffuse)
{
    float3 totalAmbient = float3(0.0f, 0.0f, 0.0f);
    float3 totalDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 totalSpecular = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    {
        totalAmbient += lights[i].ambientColour * diffuse;

        float3 lightDirection = normalize(lights[i].lightPosition - pos);
        float nDotL = dot(normal, lightDirection);
        float3 reflection = normalize(reflect(-lightDirection, normal));
        float rDotV = max(0.0f, dot(reflection, viewDir));

        totalDiffuse += saturate(lights[i].diffuseColour * nDotL * diffuse);

        if (nDotL > 0.0f)
        {
            float3 specularIntensity = float3(1.0, 1.0, 1.0);
            totalSpecular += lights[i].specularColour * (pow(rDotV, lights[i].specularPower)) * specularIntensity;
        }
    }
    return float4(totalAmbient + totalDiffuse + totalSpecular, 1.0f);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
   return txDiffuse.Sample(samp1, input.uv);
	////Range is held in [0,1], need to expand that to [-1,1]
    //bumpMap = (bumpMap * 2.0f) - 1.0f;
    //float3 bumpNormal = normalize((bumpMap.x * input.normal) + (bumpMap.y * input.normal) + (bumpMap.z * input.normal));
    //float4 colour = PhongIllumination(input.positionW, bumpNormal, input.viewDirection, float3(0.6735f, 0.6196f, 0.6275f));

    //return colour;
}