
// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

static const float3 m_quad[4] =
{
    float3(-1, 1, 0),
float3(-1, -1, 0),
float3(1, 1, 0),
float3(1, -1, 0),
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION;
};
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

[maxvertexcount(100)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutputStream)
{
    PixelShaderInput output = (PixelShaderInput) 0;
    float quadsize = 1;
    float3 leftvec = view._11_21_31;
    float3 upvec = view._12_22_32;
    float4 pos = input[0].pos;
    output.pos = mul(input[0].pos, view);
    //output.pos = mul(output.pos, projection);

    float3 initQuadVert = quadsize * m_quad[1];
    float4 particlePos = float4(initQuadVert, 1.0);
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
    particlePos.z += 3;
    particlePos = mul(particlePos, model);
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    output.color = float4(1, 1, 1, 0);
    OutputStream.Append(output);

    initQuadVert = quadsize * m_quad[0];
    particlePos = float4(initQuadVert, 1.0);
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
    particlePos.z += 3;
    particlePos = mul(particlePos, model);
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    output.color = float4(1, 0.3, 0.4, 0);
    OutputStream.Append(output);

    initQuadVert = quadsize * m_quad[3];
    particlePos = float4(initQuadVert, 1.0);
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
    particlePos.z += 3;
    particlePos = mul(particlePos, model);
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    output.color = float4(1, 0, 0, 0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    OutputStream.Append(output);
    OutputStream.RestartStrip();

    initQuadVert = quadsize * m_quad[0];
    particlePos = float4(initQuadVert, 1.0);
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
    particlePos.z += 3;
    particlePos = mul(particlePos, model);
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    output.color = float4(0.1, 1, 1, 0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    OutputStream.Append(output);
    initQuadVert = quadsize * m_quad[2];
    particlePos = float4(initQuadVert, 1.0);
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
    particlePos.z += 3;
    particlePos = mul(particlePos, model);
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    output.color = float4(1, 1, 0.5, 0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    OutputStream.Append(output);
    initQuadVert = quadsize * m_quad[3];
    particlePos = float4(initQuadVert, 1.0);
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
    particlePos.z += 3;
    particlePos = mul(particlePos, model);
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    output.color = float4(1, 0.4, 1, 0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    OutputStream.Append(output);
    OutputStream.RestartStrip();

}
