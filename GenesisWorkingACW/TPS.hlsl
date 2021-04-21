#define PI 3.1415926535897932384626433832795
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

cbuffer InverseConstantBuffer : register(b1)
{
    matrix inverse;
}

cbuffer CameraConstantBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
}
cbuffer TotalTimeBuffer : register(b3)
{
    float time;
}
#define vec3 float3
#define vec2 float2
#define vec4 float4
#define LOWDETAIL
#define mix lerp

struct VS_QUAD
{
    float4 Position : SV_POSITION;
    float2 canvasXY : TEXCOORD0;
};

struct outputPS
{
    float4 colour : SV_TARGET;
    float depth : SV_DEPTH;
};
struct Ray
{
    float3 o; // origin
    float3 d; // direction
};
float hash(vec2 uv)
{
    return frac(sin(dot(uv, vec2(73211.171, 841.13))) * 32131.18128);
}

float noise(vec2 uv)
{
    vec2 ipos = floor(uv);
    vec2 fpos = frac(uv);
    
    float a = hash(ipos);
    float b = hash(ipos + vec2(1, 0));
    float c = hash(ipos + vec2(0, 1));
    float d = hash(ipos + vec2(1, 1));
    
    vec2 t = smoothstep(0.0, 1.0, fpos);
    
    return mix(mix(a, b, t.x), mix(c, d, t.x), t.y);
}

float fbm(vec2 uv)
{
    float acc = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < 6; ++i)
    {
        acc += amp * noise(freq * uv);
        freq *= 2.0;
        amp *= 0.5;
    }
    return acc;
}

float intersect(in vec3 ro, in vec3 rd, in float t_max, out bool hit)
{
    hit = false;
    float t = 0.1;
    
    for (int i = 0; i < 256; ++i)
    {
        vec3 p = ro + t * rd;
        float h = p.y - fbm(p.xz);
        if (h < 0.004 * t || t > t_max)
        {
            if (t <= t_max)
            {
                hit = true;
            }
            break;
        }

        
        t += 0.45 * h;
    }
    return t;
}

float4 main(VS_QUAD input) : SV_Target
{
    vec2 uv = input.canvasXY;
    float3 ro = mul(float4(0, 3, 0, 1), inverse).xyz;
    float3 pos = float3(input.canvasXY, -1);
    float3 rd = normalize(mul(float4(pos.x,pos.y+2,pos.z, 0), inverse)).xyz;
   
    //vec3 cam_z = normalize(at - ro);
    //vec3 cam_x = normalize(cross(vec3(0, 1, 0), cam_z));
    //vec3 cam_y = normalize(cross(cam_z, cam_x));
    //vec3 rd = cam_x * uv.x + cam_y * uv.y + 2.0 * cam_z;
    
    ro += vec3(0.0, 0, fmod(1.0, 100.0));
    vec3 l = normalize(vec3(0.5, -1.5, 0.5));
    
    bool hit;
    float t_max = 27.5;
    float t = intersect(ro, rd, t_max, hit);
    
    vec3 p = ro + t * rd;
    p.y += cameraPosition.y;
    //NOTE(chen): a hack, note dpdx and dpdz are not actual partial derivatives, just some offseted vector
    vec3 px = vec3(p.x + 0.0001, -2.0, p.z);
    vec3 pz = vec3(p.x, 0.0, p.z + 0.0001);
    vec3 dpdx = vec3(px.x, fbm(px.xz), px.z);
    vec3 dpdz = vec3(pz.x, fbm(pz.xz), pz.z);
    
    vec3 normal = normalize(cross(dpdz, dpdx));
    
    vec3 background = vec3(0.8, 0.8, 0.5);
    vec3 col = background;
    if (hit)
    {
        float occ = p.y;
        float dl = 0.25 * pow(dot(normal, -l) + 1.0, 2.0);
        float lighting = 0.9 * dl + 0.3;
        vec3 mat = vec3(0.8, 0.3, 0.0);
        
        col = mix(occ * lighting * mat, background, pow(t / t_max, 1.5));
    }
    else
        discard;
    
    return vec4(col, 1.0);
}