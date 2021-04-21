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
    float3 positionW : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 viewDirection : TEXCOORD1;
};
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 1;
    vec3 direction = normal * ((sin(0.1) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}


vec3 GetNormal(vec3 inp, vec3 inp1, vec3 inp2)
{
    vec3 a = vec3(inp) - vec3(inp1);
    vec3 b = vec3(inp2) - vec3(inp1);
    return normalize(cross(a, b));
}




[maxvertexcount(10)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<PixelShaderInput> OutputStream)
{
    PixelShaderInput output = (PixelShaderInput) 0;
    float quadsize = .5;
    //float3 leftvec = view._11_21_31;
    //float3 upvec = view._12_22_32;
   
    //output.pos = mul(input[0].pos, view);
    //output.pos = mul(output.pos, projection);
    for (uint i = 0; i < 3; i++)
    {
        float4 pos = input[0].pos;
        float3 norm = GetNormal(input[0].pos.xyz, input[1].pos.xyz, input[2].pos.xyz);
        
        output.normal = mul(norm, (float3x3) model);
        norm.xyz *= sin(time * 3);
        vec4 e = explode(input[i].pos, norm);
        pos = e;
	// Transform the vertex position into projected space.
        pos = mul(pos, model);
        pos = mul(pos, view);
        pos = mul(pos, projection);
        output.positionH = pos;
        //output.pos = input[i].pos;
        output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
        OutputStream.Append(output);
    }
    OutputStream.RestartStrip();
 //   for (uint i = 0; i < 3; i++)
 //   {
        
 //       float3 norm = GetNormal(input[0].pos.xyz, input[1].pos.xyz, input[2].pos.xyz);
        
 //       output.Norm = norm;
 //       norm.xyz *= sin(time * 3);
 //       vec4 e = explode(input[i].pos, norm);
 //       float4 pos = e;
 //       pos.x += 3;
	//// Transform the vertex position into projected space.
 //       pos = mul(pos, model);
 //       pos = mul(pos, view);
 //       pos = mul(pos, projection);
 //       output.pos = pos;
 //       //output.pos = input[i].pos;
 //       output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
 //       OutputStream.Append(output);
 //   }
 //   OutputStream.RestartStrip();
    //initQuadVert = quadsize * m_quad[0];
    //particlePos = float4(initQuadVert, 1.0);
    //particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    //particlePos.xyz += pos.xyz;
    //particlePos = mul(particlePos, view);
    //output.pos = mul(particlePos, projection);
    //output.color = float4(1, 1, 1, 0);
    //OutputStream.Append(output);

    //initQuadVert = quadsize * m_quad[3];
    //particlePos = float4(initQuadVert, 1.0);
    //particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    //particlePos.xyz += pos.xyz;
    //particlePos = mul(particlePos, view);
    //output.pos = mul(particlePos, projection);
    //output.color = float4(1, 1, 1, 0);
    //OutputStream.Append(output);
    //OutputStream.RestartStrip();

    //initQuadVert = quadsize * m_quad[0];
    //particlePos = float4(initQuadVert, 1.0);
    //particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    //particlePos.xyz += pos.xyz;
    //particlePos = mul(particlePos, view);
    //output.pos = mul(particlePos, projection);
    //output.color = float4(0.1, 1, 1, 0);
    //OutputStream.Append(output);
    //initQuadVert = quadsize * m_quad[2];
    //particlePos = float4(initQuadVert, 1.0);
    //particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    //particlePos.xyz += pos.xyz;
    //particlePos = mul(particlePos, view);
    //output.pos = mul(particlePos, projection);
    //output.color = float4(1, 1, 0.5, 0);
    //OutputStream.Append(output);
    //initQuadVert = quadsize * m_quad[3];
    //particlePos = float4(initQuadVert, 1.0);
    //particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
    //particlePos.xyz += pos.xyz;
    //particlePos = mul(particlePos, view);
    //output.pos = mul(particlePos, projection);
    //output.color = float4(1, 0.4, 1, 0);
    //OutputStream.Append(output);
    //OutputStream.RestartStrip();



 //   for (uint i = 0; i < 3; i++)
 //   {
        
 //       float3 norm = GetNormal(input[0].pos.xyz, input[1].pos.xyz, input[2].pos.xyz);
 //       vec4 e = explode(input[i].pos, norm);
 //       pos = e;
	//// Transform the vertex position into projected space.
 //       pos = mul(pos, model);
 //       pos = mul(pos, view);
 //       pos = mul(pos, projection);
 //       output.pos = pos;
 //       //output.pos = input[i].pos;
 //       output.color = float4(1, 1, 1, 1);
 //       output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
 //       OutputStream.Append(output);
 //   }
 //   OutputStream.RestartStrip();
 //   float3 leftvec = view._11_21_31;
 //   float3 upvec = view._12_22_32;
 //   for (uint i = 1; i < 4; i++)
 //   {
 //       float quadsize = 0.3;
 //       float3 initQuadVert = quadsize * m_quad[i];
 //       float4 particlePos = float4(initQuadVert, 1.0);
 //       particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
 //       particlePos.xyz += pos.xyz;
 //       particlePos = mul(particlePos, view);
 //       output.pos = mul(particlePos, projection);
 //       output.color = float4(1, 0, 1, 0);
 //       OutputStream.Append(output);

 //   }
 //   OutputStream.RestartStrip();
 //   for (uint i = 1; i < 4; i++)
 //   {
 //       float quadsize = 0.3;
 //       float3 initQuadVert = quadsize * gl_positions[i];
 //       float4 particlePos = float4(initQuadVert, 1.0);
 //       particlePos.xyz = initQuadVert.x * leftvec + initQuadVert.y * upvec;
 //       particlePos.xyz += pos.xyz;
 //       particlePos = mul(particlePos, view);
 //       output.pos = mul(particlePos, projection);
 //       //output.CanvasXY = gl_positions[i];
 //       output.color = float4(1, 0, 1, 0);
 //       OutputStream.Append(output);

 //   }
 //   OutputStream.RestartStrip();
    
 //   for (uint i = 0; i < 3; i++)
 //   {
 //       float4 pos = input[i].pos;
 //       /*float3 norm = GetNormal(input[0].pos.xyz, input[1].pos.xyz, input[2].pos.xyz);
 //       vec4 e = explode(input[i].pos, norm);
 //       pos = e;*/
 //       // Transform the vertex position into projected space.
 ///*       pos = mul(pos, model);
 //       pos = mul(pos, view);
 //       pos = mul(pos, projection);*/
 //       output.pos = pos;
 //       //output.pos = input[i].pos;
 //       output.color = input[i].color;
 //       output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
 //       OutputStream.Append(output);
 //   }
 //   OutputStream.RestartStrip();
 //   for (uint i = 0; i < 3; i++)
 //   {
 //       float4 pos = input[i].pos;
 //       pos += 0.8;
	//// Transform the vertex position into projected space.
 //       pos = mul(pos, model);
 //       pos = mul(pos, view);
 //       pos = mul(pos, projection);
 //       output.pos = pos;
 //       //output.pos = input[i].pos;
 //       output.color = input[i].color;
 //       output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
 //       OutputStream.Append(output);
 //   }
 //   OutputStream.RestartStrip();
    
 //   for (uint i = 0; i < 3; i++)
 //   {
       
 //       float4 pos = input[i].pos;
 //       pos -= 0.7;
	//// Transform the vertex position into projected space.
 //       pos = mul(pos, model);
 //       pos = mul(pos, view);
 //       pos = mul(pos, projection);
 //       output.pos = pos;
 //       //output.pos = input[i].pos;
 //       output.color = input[i].color;
 //       output.uv = (sign(input[i].pos.xy) + 1.0) / 2.0;
 //       OutputStream.Append(output);
 //   }
 //   OutputStream.RestartStrip();
    
    
}