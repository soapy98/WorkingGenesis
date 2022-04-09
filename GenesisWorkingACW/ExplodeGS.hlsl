#define vec3 float3
#define vec4 float4
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
    float4 positionH : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 0.1;
    vec3 direction = normal * ((sin(0.1) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}


vec3 GetNormal(vec3 inp, vec3 inp1, vec3 inp2)
{
    vec3 a = vec3(inp) - vec3(inp1);
    vec3 b = vec3(inp2) - vec3(inp1);
    return normalize(cross(a, b));
}




[maxvertexcount(6)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<PixelShaderInput> OutputStream)
{
    PixelShaderInput output = (PixelShaderInput) 0;
    float quadsize = .5;
    //float3 leftvec = view._11_21_31;
    //float3 upvec = view._12_22_32;
   
    //output.pos = mul(input[0].pos, view);
    //output.pos = mul(output.pos, projection);
    float4 cent = (input[0].pos + input[1].pos + input[2].pos)/3;
      float3 norm =  output.normal=GetNormal(input[0].pos.xyz, input[1].pos.xyz, input[2].pos.xyz);
    for (uint j = 0; j < 3; j++)
    {
        float4 pos = input[j].pos;
        output.normal = norm;
        output.positionH = pos;
        output.uv = (sign(input[j].pos.xy) + 1.0) / 2.0;
        OutputStream.Append(output);
    }
    OutputStream.RestartStrip();
    for (uint i = 0; i < 3; i++)
    {
        float4 pos = input[i].pos;
        int next = (i + 1) % 3;
        norm.xy *= sin(time);
        vec4 e = explode(input[i].pos, norm);
        //pos = e;
        output.positionH = pos;
        output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
        OutputStream.Append(output);
        float4 pos2 = cent + float4(norm, 0) * 2;
        e = explode(pos2, norm);
        output.positionH = e;
        output.uv = (sign(e.xy) + 1.0) / 2.0;
        OutputStream.Append(output);
        float4 pos3 = explode(input[next].pos, norm);
        output.positionH = input[next].pos;
        output.uv = (sign(pos3) + 1.0) / 2.0;
        OutputStream.Append(output);
        OutputStream.RestartStrip();
    }
   
    
}
