
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
cbuffer TotalTimeConstantBuffer : register(b2)
{
    float time;
    float3 pad;
}
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
    float2 uv : TEXCOORD0;
    float4 col : COLOR;
};

[maxvertexcount(6)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutputStream)
{
    PixelShaderInput output = (PixelShaderInput) 0;
    float quadsize = 0.2;
    float3 leftvec = view._11_21_31;
    float3 upvec = view._12_22_32;
    float4 pos = input[0].pos;
    pos.z *= sin(time);
    //pos.y *= cos(time) * 0.5;
    pos.x += cos(time) * 0.5;
    float3 initQuadVert = quadsize * m_quad[1];
    float4 particlePos = float4(initQuadVert, 1.0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    
    particlePos.xyz += pos.xyz;

    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);

    OutputStream.Append(output);

    initQuadVert = quadsize * m_quad[0];
    particlePos = float4(initQuadVert, 1.0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
 
    particlePos.xyz += pos.xyz;

    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);

    OutputStream.Append(output);

    initQuadVert = quadsize * m_quad[3];
    particlePos = float4(initQuadVert, 1.0);
    
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
 
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    float dist3 = distance(float2(0.5, 0.5), output.uv);

    OutputStream.Append(output);
    OutputStream.RestartStrip();

    initQuadVert = quadsize * m_quad[0];
    particlePos = float4(initQuadVert, 1.0);
    
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;

    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);


    float dist4 = distance(float2(0.5, 0.5), output.uv);
    OutputStream.Append(output);
    
    initQuadVert = quadsize * m_quad[2];
    particlePos = float4(initQuadVert, 1.0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;
       
    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);

    OutputStream.Append(output);
    
    initQuadVert = quadsize * m_quad[3];
    particlePos = float4(initQuadVert, 1.0);
    output.uv = (sign(particlePos.xy) + 1.0) / 2.0;
    particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    particlePos.xyz += pos.xyz;

    particlePos = mul(particlePos, view);
    output.pos = mul(particlePos, projection);
    OutputStream.Append(output);
    OutputStream.RestartStrip();

}