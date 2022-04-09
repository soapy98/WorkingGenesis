cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

cbuffer InverseViewConstantBuffer : register(b1)
{
    matrix inverse;
}

cbuffer CameraConstantBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
}

cbuffer timeConstantBuffer : register(b3)
{
    float time;
    float3 padding2;
}
#define vec3 float3
#define vec4 float4
#define mat4  float4x4
#define fract frac
#define mix lerp
#define iTime time
#define vec2 float2
#define mat3 float3x3
// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 canvasXY : TEXCOORD0;
};

#define NUMBER_OF_LIGHTS 1

static int MAX_MARCHING_STEPS = 50;
static float MIN_DIST = 1.0;
static float MAX_DIST = 50.0;
static float EPSILON = 0.0001;

struct Light
{
    float4 ambientColour;
    float4 diffuseColour;
    float4 specularColour;
    float3 lightPosition;
    float specularPower;
};

static Light lights[NUMBER_OF_LIGHTS] =
{
	//LightOne
    { 0.2, 0.2, 0.2, 1.0, 0.4, 0.4, 0.4, 1.0, 0.4, 0.4, 0.4, 1.0, 20.0, 15.0, 20.0, 30 },
	//LightTwo
    //{ 0.2, 0.2, 0.6, 1.0, 0.4, 0.4, 0.4, 1.0, 0.4, 0.4, 0.4, 1.0, -20.0, -15, -20.0, 100 },
};


struct Ray
{
    float3 o; //origin
    float3 d; //direction

};

float length2(float2 p)
{
    return sqrt(p.x * p.x + p.y * p.y);
}

float length6(float2 p)
{
    p = p * p * p;
    p = p * p;
    return pow(p.x + p.y, 1.0 / 6.0);
}

float length8(float2 p)
{
    p = p * p;
    p = p * p;
    p = p * p;
    return pow(p.x + p.y, 1.0 / 8.0);
}

float dot2(float2 v)
{
    return dot(v, v);
}
mat3 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}

/**
 * Rotation matrix around the Y axis.
 */
mat3 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}

/**
 * Rotation matrix around the Z axis.
 */
mat3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1)
    );
}
//Sphere Signed Distance Function with a radius of 1 (p = sphere point location)
float sphereSDF(float3 p, float s)
{
    return length(p) - s;
}

float cubeSDF(float3 p, float3 b)
{
    float3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

float torusSDF(float3 p, float2 t)
{
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float hexagonalPrismSDF(float3 p, float2 h)
{
    const float3 k = float3(-0.8660254, 0.5, 0.57735);

    p = abs(p);
    p.xy -= 2.0 * min(dot(k.xy, p.xy), 0.0) * k.xy;

    float2 d = float2(length(p.xy - float2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.y - h.x), p.z - h.y);

    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float roundBoxSDF(float3 p, float3 b, float r)
{
    float3 q = abs(p) - b;
    return min(max(q.x, max(q.y, q.z)), 0.0) + length(max(q, 0.0)) - r;
}

float cylinderSDF(float3 p, float2 h)
{
    float2 d = abs(float2(length(p.xz), p.y)) - h;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinder6SDF(float3 p, float2 h)
{
    return max(length6(p.xz) - h.x, abs(p.y) - h.y);
}

float cylinderSDF(float3 p, float3 a, float3 b, float r)
{
    float3 pa = p - a;
    float3 ba = b - a;
    float baba = dot(ba, ba);
    float paba = dot(pa, ba);

    float x = length(pa * baba - ba * paba) - r * baba;
    float y = abs(paba - baba * 0.5) - baba * 0.5;
    float x2 = x * x;
    float y2 = y * y * baba;
    float d = (max(x, y) < 0.0) ? -min(x2, y2) : (((x > 0.0) ? x2 : 0.0) + ((y > 0.0) ? y2 : 0.0));
    return sign(d) * sqrt(abs(d)) / baba;
}

float torus82SDF(float3 p, float2 t)
{
    float2 q = float2(length2(p.xz) - t.x, p.y);
    return length8(q) - t.y;
}

float boxSDF(float3 p, float3 b)
{
    float3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

float coneSDF(float3 p, float3 c)
{
    float2 q = float2(length(p.xz), p.y);
    float d1 = -q.y - c.z;
    float d2 = max(dot(q, c.xy), q.y);
    return length(max(float2(d1, d2), 0.0)) + min(max(d1, d2), 0.0);
}

float cappedConeSDF(float3 p, float h, float r1, float r2)
{
    float2 q = float2(length(p.xz), p.y);

    float2 k1 = float2(r2, h);
    float2 k2 = float2(r2 - r1, 2.0 * h);
    float2 ca = float2(q.x - min(q.x, (q.y < 0.0) ? r1 : r2), abs(q.y) - h);
    float2 cb = q - k1 + k2 * clamp(dot(k1 - q, k2) / dot2(k2), 0.0, 1.0);
    float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
    return s * sqrt(min(dot2(ca), dot2(cb)));
}

float roundConeSDF(float3 p, float r1, float r2, float h)
{
    float2 q = float2(length(p.xz), p.y);

    float b = (r1 - r2) / h;
    float a = sqrt(1.0 - b * b);
    float k = dot(q, float2(-b, a));

    if (k < 0.0)
        return length(q) - r1;
    if (k > a * h)
        return length(q - float2(0.0, h)) - r2;

    return dot(q, float2(a, b)) - r1;
}

float roundConeSDF(float3 p, float3 a, float3 b, float r1, float r2)
{
    float3 ba = b - a;
    float l2 = dot(ba, ba);
    float rr = r1 - r2;
    float a2 = l2 - rr * rr;
    float il2 = 1.0 / l2;

    float3 pa = p - a;
    float y = dot(pa, ba);
    float z = y - l2;
    float x2 = dot2(pa * l2 - ba * y);
    float y2 = y * y * l2;
    float z2 = z * z * l2;

    float k = sign(rr) * rr * rr * x2;
    if (sign(z) * a2 * z2 > k)
        return sqrt(x2 + z2) * il2 - r2;
    if (sign(y) * a2 * y2 < k)
        return sqrt(x2 + y2) * il2 - r1;
    return (sqrt(x2 * a2 * il2) + y * rr) * il2 - r1;
}

float ellipsoidSDF(float3 p, float3 r)
{
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0) / k1;

}

float equilateralTriangleSDF(float2 p)
{
    const float k = 1.73205; //sqrt(3.0);
    p.x = abs(p.x) - 1.0;
    p.y = p.y + 1.0 / k;
    if (p.x + k * p.y > 0.0)
        p = float2(p.x - k * p.y, -k * p.x - p.y) / 2.0;
    p.x += 2.0 - 2.0 * clamp((p.x + 2.0) / 2.0, 0.0, 1.0);
    return -length(p) * sign(p.y);
}

float triPrismSDF(float3 p, float2 h)
{
    float3 q = abs(p);
    float d1 = q.z - h.y;
    h.x *= 0.866025;
    float d2 = equilateralTriangleSDF(p.xy / h.x) * h.x;
    return length(max(float2(d1, d2), 0.0)) + min(max(d1, d2), 0.);
}

float hexPrismSDF(float3 p, float2 h)
{
    float3 q = abs(p);

    const float3 k = float3(-0.8660254, 0.5, 0.57735);
    p = abs(p);
    p.xy -= 2.0 * min(dot(k.xy, p.xy), 0.0) * k.xy;
    float2 d = float2(
		length(p.xy - float2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.y - h.x),
		p.z - h.y);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float octahedronSDF(float3 p, float s)
{
    p = abs(p);

    float m = p.x + p.y + p.z - s;

    float3 q;
    if (3.0 * p.x < m)
        q = p.xyz;
    else if (3.0 * p.y < m)
        q = p.yzx;
    else if (3.0 * p.z < m)
        q = p.zxy;
    else
        return m * 0.57735027;

    float k = clamp(0.5 * (q.z - q.y + s), 0.0, s);
    return length(float3(q.x, q.y - s + k, q.z - k));
}

float4 unionSDF(float4 sdfDisOne, float4 sdfDisTwo)
{
    return (sdfDisOne.x < sdfDisTwo.x) ? sdfDisOne : sdfDisTwo;
}

float intersectSDF(float4 sdfDisOne, float4 sdfDisTwo)
{
    return (sdfDisOne.x > sdfDisTwo.x) ? sdfDisOne : sdfDisTwo;
}

float differenceSDF(float4 sdfDisOne, float4 sdfDisTwo)
{
    return (sdfDisOne.x < -sdfDisTwo.x) ? sdfDisOne : float4(-sdfDisTwo.x, sdfDisTwo.yzw);
}

float3 twistSDF(float3 p, float rep)
{
    float c = cos(rep * p.y + rep);
    float s = sin(rep * p.y + rep);
    float2x2 m = float2x2(c, -s, s, c);
    return float3(mul(p.xz, m), p.y);
}

float softAbs2(float x, float a)
{
    float xx = 2.0 * x / a;
    float abs2 = abs(xx);

    if (abs2 < 2.0)
    {
        abs2 = 0.5 * xx * xx * (1.0 - abs2 / 6) + 2.0 / 3.0;
    }

    return abs2 * a / 2.0;
}

//Intersect2
float4 softMax2(float4 sdfDisOne, float4 sdfDisTwo, float blendFactor)
{
    float r = 0.5 * (sdfDisOne.x + sdfDisTwo.x + softAbs2(sdfDisOne.x - sdfDisTwo.x, blendFactor));

    return float4(r, lerp(sdfDisOne.yzw, sdfDisTwo.yzw, blendFactor));
}

//Union2
float4 softMin2(float4 sdfDisOne, float4 sdfDisTwo, float blendFactor)
{
    float r = -0.5 * (-sdfDisOne.x - sdfDisTwo.x + softAbs2(sdfDisOne.x - sdfDisTwo.x, blendFactor));

    return float4(r, lerp(sdfDisOne.yzw, sdfDisTwo.yzw, blendFactor));
}

float smoothUnion(float d1, float d2, float k)
{
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return lerp(d2, d1, h) - k * h * (1.0 - h);
}

float smoothSubtraction(float d1, float d2, float k)
{
    float h = clamp(0.5 - 0.5 * (d2 + d1) / k, 0.0, 1.0);
    return lerp(d2, -d1, h) + k * h * (1.0 - h);
}

float smoothIntersection(float d1, float d2, float k)
{
    float h = clamp(0.5 - 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return lerp(d2, d1, h) + k * h * (1.0 - h);
}

float4 mandelBulb(float3 pos)
{
    float Power = 3.0 + 4.0 * (sin(time / 30.0) + 1.0);
	//float Power = 8.0f;

    float3x3 rotMatrix = float3x3(
		float3(cos(time * 0.2), -sin(time * 0.2), 0.0),
		float3(sin(time * 0.2), cos(time * 0.2), 0.0),
		float3(0.0, 0.0, 1.0)
	);

    float3x3 rotMatrix2 = float3x3(
		float3(cos(time * 0.2), 0.0, -sin(time * 0.2)),
		float3(0.0, 1.0, 0.0),
		float3(sin(time * 0.2), 0.0, cos(time * 0.2))
	);

    float3x3 rotMatrix3 = float3x3(
		float3(1.0, 0.0, 0.0),
		float3(0.0, cos(time * 0.2), -sin(time * 0.2)),
		float3(0.0, sin(time * 0.2), cos(time * 0.2))
	);

    float3 z = mul(rotMatrix, mul(rotMatrix2, mul(rotMatrix3, pos + float3(0.0f, 0.0f, -0.5f))));

    float dr = 1.0;
    float r = 0.0;
    for (int i = 0; i < 8; i++)
    {
        r = length(z);
        if (r > 1.5f)
            break;

		// convert to polar coordinates
        float theta = acos(z.z / r);
        float phi = atan2(z.x, z.y);

        dr = pow(r, Power - 1.0) * Power * dr + 1.0;

		// scale and rotate the point
	
        float zr = pow(r, Power);
        theta = theta * Power;
        phi = phi * Power;

		// convert back to cartesian coordinates
        z = zr * float3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += pos;
    }

    return float4(0.5 * log(r) * r / dr, saturate(float3(lerp(0.0f, 1.0f, sin(time)), lerp(0.0f, 1.0f, -sin(time)), lerp(0.0f, 1.0f, cos(time)))));

	//return float4(0.5*log(r)*r / dr, saturate((float3)lerp(Power, z, sin(time))));
}


static float3 va = float3(0.0, 0.57735, 0.0);
static float3 vb = float3(0.0, -1.0, 1.15470);
static float3 vc = float3(1.0, -1.0, -0.57735);
static float3 vd = float3(-1.0, -1.0, -0.57735);

float4 SierpinskiTetrahedron(float3 pos)
{
    float s = 1.0;
    float r = 1.0;
    float dm;
    float3 v;
    for (int i = 0; i < 8; i++)
    {
        float d;
        d = dot(pos - va, pos - va);
        v = va;
        dm = d;
        d = dot(pos - vb, pos - vb);
        if (d < dm)
        {
            v = vb;
            dm = d;
        }
        d = dot(pos - vc, pos - vc);
        if (d < dm)
        {
            v = vc;
            dm = d;
        }
        d = dot(pos - vd, pos - vd);
        if (d < dm)
        {
            v = vd;
            dm = d;
        }
        pos = v + 2.0 * (pos - v);
        r *= 2.0;
    }

    return float4((sqrt(dm) - 1.0) / r, saturate(pos));
}

vec4 NC0 = vec4(0.0, 157.0, 113.0, 270.0);
vec4 NC1 = vec4(1.0, 158.0, 114.0, 271.0);
//vec4 WS=vec4(10.25,32.25,15.25,3.25);
vec4 WS = vec4(0.25, 0.25, 0.25, 0.25);
vec4 hash4(vec4 n)
{
    return fract(sin(n) * 1399763.5453123);
}
vec3 hash3(vec3 n)
{
    return fract(sin(n) * 1399763.5453123);
}
vec3 hpos(vec3 n)
{
    return hash3(vec3(dot(n, vec3(157.0, 113.0, 271.0)), dot(n, vec3(271.0, 157.0, 113.0)), dot(n, vec3(113.0, 271.0, 157.0))));
}
//vec4 hash4( vec4 n ) { return fract(n*fract(n*0.5453123)); }
//vec4 hash4( vec4 n ) { n*=1.987654321; return fract(n*fract(n)); }
float noise4q(vec4 x)
{
    vec4 n3 = vec4(0, 0.25, 0.5, 0.75);
    vec4 p2 = floor(x.wwww + n3);
    vec4 b = floor(x.xxxx + n3) + floor(x.yyyy + n3) * 157.0 + floor(x.zzzz + n3) * 113.0;
    vec4 p1 = b + fract(p2 * 0.00390625) * vec4(164352.0, -164352.0, 163840.0, -163840.0);
    p2 = b + fract((p2 + 1.0) * 0.00390625) * vec4(164352.0, -164352.0, 163840.0, -163840.0);
    vec4 f1 = fract(x.xxxx + n3);
    vec4 f2 = fract(x.yyyy + n3);
    f1 = f1 * f1 * (3.0 - 2.0 * f1);
    f2 = f2 * f2 * (3.0 - 2.0 * f2);
    vec4 n1 = vec4(0, 1.0, 157.0, 158.0);
    vec4 n2 = vec4(113.0, 114.0, 270.0, 271.0);
    vec4 vs1 = mix(hash4(p1), hash4(n1.yyyy + p1), f1);
    vec4 vs2 = mix(hash4(n1.zzzz + p1), hash4(n1.wwww + p1), f1);
    vec4 vs3 = mix(hash4(p2), hash4(n1.yyyy + p2), f1);
    vec4 vs4 = mix(hash4(n1.zzzz + p2), hash4(n1.wwww + p2), f1);
    vs1 = mix(vs1, vs2, f2);
    vs3 = mix(vs3, vs4, f2);
    vs2 = mix(hash4(n2.xxxx + p1), hash4(n2.yyyy + p1), f1);
    vs4 = mix(hash4(n2.zzzz + p1), hash4(n2.wwww + p1), f1);
    vs2 = mix(vs2, vs4, f2);
    vs4 = mix(hash4(n2.xxxx + p2), hash4(n2.yyyy + p2), f1);
    vec4 vs5 = mix(hash4(n2.zzzz + p2), hash4(n2.wwww + p2), f1);
    vs4 = mix(vs4, vs5, f2);
    f1 = fract(x.zzzz + n3);
    f2 = fract(x.wwww + n3);
    f1 = f1 * f1 * (3.0 - 2.0 * f1);
    f2 = f2 * f2 * (3.0 - 2.0 * f2);
    vs1 = mix(vs1, vs2, f1);
    vs3 = mix(vs3, vs4, f1);
    vs1 = mix(vs1, vs3, f2);
    float r = dot(vs1, (vec4) 0.25);
	//r=r*r*(3.0-2.0*r);
    return r * r * (3.0 - 2.0 * r);
}





float4 EvaluateCubic(float4 base)
{
    return float4(base.x + base.y, base.y / 3, -base.z / 3, base.z + base.w);
}
//Signed Distance Function for the scene, function return value of called SDF 
//Determines location of P relative to the surface of the function (sphere)
float4 sceneSDF(float3 samplePoint)
{
	//Contains hit distance (x) and colour (yzw)
    float4 closestHit = float4(1e10, 0.0f, 0.0f, 0.0f);

    float3 infinPos = float3(abs(samplePoint.x), samplePoint.y-0.51 , abs(samplePoint.z));
    infinPos.xz = fmod(infinPos.xz / 1.0f + .5f, 1.0f) - 0.5f;
 closestHit=   unionSDF(closestHit, float4(triPrismSDF(infinPos, float2(0.03f, 0.01f)), float3(0.4f, 0.8f, 0.8f)));

	

    //float3 facePos = float3(0, 0, 0);
    //float face= smoothUnion(hexPrismSDF(samplePoint - float3(facePos.x, facePos.y , facePos.z), float2(0.1f, 0.03f)), sphereSDF(samplePoint - float3(facePos.x - 0.007f, facePos.y + 0.03f, facePos.z), 0.035f), 0.01f);
   
    //face = smoothUnion(samplePoint, sphereSDF(samplePoint - float3(facePos.x, facePos.y, facePos.z + 0.015f), 0.1f), 0.05f);
 //closestHit = unionSDF(closestHit, float4(face, 0.987f, 0.28f, 0.45f));
   
    //closestHit = unionSDF(closestHit, float4(re.r, 01.987f, 0.128f, 0.45f));

  //Person
    float3 bodyPos = float3(0, 0, 0);
     
    float3 snakePos = float3(0.4, 0.3, 0.6);
    float4 snake = float4(snakePos.x - 0.04, snakePos.y - 0.04, snakePos.z, 0.08);
    //snake = EvaluateCubic(snake) * sin(time) * 2;
    //{
    //    float an = sin(time);
    //    float3 q = snakePos - float3(0.0, 0.0, -.3);
    //    float uni = smoothUnion(sphereSDF(samplePoint - float3(bodyPos.x - 0.06, bodyPos.y + 0.05, bodyPos.z), 0.05), sphereSDF(samplePoint - float3(bodyPos.x - 0.06, bodyPos.y + 0.05, bodyPos.z), 0.05), 0.03);
    //    float d2 = roundBoxSDF(q, float3(0.06, 0.02, 0.07), 0.1);
    //    float dt = smoothSubtraction(uni, d2, 0.03);
    //    uni = smoothUnion(sphereSDF(samplePoint - float3(bodyPos.x + 0.06, bodyPos.y + sin(time * 4) * 0.05, bodyPos.z), 0.05), cylinderSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), 0.01, 0.015, 0.028), 0.1f);
    //    float d = min(dt, uni);
    //    closestHit = unionSDF(closestHit, float4(d, 0.456f, 0.15f, 0.5564));
    //    uni = smoothUnion(sphereSDF(samplePoint - float3(bodyPos.x - 0.06, bodyPos.y + sin(time * 4) * 0.05, bodyPos.z), 0.05), cylinderSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), 0.01, 0.015, 0.028), 0.1f);
    //    closestHit = unionSDF(closestHit, float4(uni, 0.456f, 0.15f, 0.5564));
    //    uni = smoothUnion(sphereSDF(samplePoint - float3(bodyPos.x - 0.06, bodyPos.y + sin(time * 4) * 0.05, bodyPos.z - 0.03), 0.05), cylinderSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), 0.01, 0.015, 0.028), 0.1f);
    //    closestHit = unionSDF(closestHit, float4(uni, 0.456f, 0.15f, 0.5564));
    //    uni = smoothUnion(sphereSDF(samplePoint - float3(bodyPos.x + 0.06, bodyPos.y + frac(sin(time * 4)) * 0.05, bodyPos.z + 0.03), 0.05), cylinderSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), 0.01, 0.015, 0.028), 0.1f);
    //    closestHit = unionSDF(closestHit, float4(uni, 0.456f, 0.15f, 0.5564));
    //    //uni = smoothUnion(uni, d, 0.5);
    //    closestHit = unionSDF(closestHit, float4(uni, 0.456f, 0.15f, 0.5564));
    //    uni = smoothUnion(uni, sphereSDF(samplePoint - float3(bodyPos.x - 0.06, bodyPos.y + 0.05, bodyPos.z), 0.05), 0.02);
    //    closestHit = unionSDF(closestHit, float4(uni, 0.3f, 0.6f, 0.4));
    //}
    //{
    //    float3 pos = float3(0.0, 0.3, 0);
    //    float sphere = sphereSDF(samplePoint - float3(pos.x + sin(time * 20) * 0.1, pos.yz), sin(time) * 0.1 + 0.05);
    //    float s = sphereSDF(samplePoint - float3(pos.x - sin(time * 20) * 0.1, pos.yz), sin(time) * 0.1 + 0.05);
    //    float merge = smoothUnion(s, sphere, 0.5);
    //    float a = coneSDF(samplePoint - float3(pos.x + sin(time * 20) * 0.1, pos.yz), float3(0.02, 0.02, 0.02));
    //    merge = smoothUnion(merge, a, 0.3);

    //    float sub = cylinderSDF(samplePoint - float3(sphere, pos.y, pos.z + sin(time)), float2(0.05, 0.05));
    //    merge = smoothSubtraction(sub, merge, 0.5);
    //    sphere = softMin2(sphereSDF(samplePoint - float3(pos.x + sin(time * 20) * 0.1, pos.yz), 0.05), ellipsoidSDF(samplePoint - float3(pos.x + 0.02, pos.y + 0.004, pos.z), float3(0.04, 0.04, 0.04)), 0.08);
    //    closestHit = unionSDF(closestHit, float4(merge, 0.456f, 0.15f, 0.5564));
    //    sphere = smoothUnion(sphere, ellipsoidSDF(samplePoint - float3(pos.x + 0.02, pos.y + 0.04, pos.z), float3(0.1, 0.1, 0.1)), 0.07);
    //    closestHit = unionSDF(closestHit, float4(sphere, 0.456f, 0.15f, 0.5564));
    //    sphere = softAbs2(sphere, cubeSDF(samplePoint - float3(pos.x + 0.02, pos.y + 0.04, pos.z), float3(0.04, 0.04, 0.04)));
    //    closestHit = unionSDF(closestHit, float4(sphere, 0.4f, 0.55f, 0.5564));
    //}
    {
        //float3 pos = float3(0.0, 0.3, 0);
        //float sphere = sphereSDF(samplePoint - float3(pos.xyz), 0.1);
        // float sphere2 = sphereSDF(samplePoint - float3(pos.x+0.15,pos.y,pos.z), 0.1);
        //float merge = smoothUnion(sphere, sphere2, 0.2);
        //closestHit = unionSDF(closestHit, float4(merge, 0.4f, 0.55f, 0.5564));
        //float sphere3 = sphereSDF(samplePoint - float3(pos.x, pos.y, pos.z + sin(time)+0.02 ), 0.01);
        //float m = smoothSubtraction(sphere3, merge, 0.5);
        
        //closestHit = unionSDF(closestHit, float4(m, 0.4f, 0.55f, 0.5564));
        ////float torus = torusSDF(samplePoint - float3(pos.x + sin(time), pos.y, pos.z), float2(0.1, 0.1));
        ////closestHit = unionSDF(closestHit, float4(torus, 0.4f, 0.67f, 0.5564));
        //float cube = cubeSDF(samplePoint - float3(pos.xy, pos.z + sin(time) * 0.2), float3(0.1, 0.1, 0.1));
        //float sub = smoothUnion(cube, sphere, 0.1);
         //sub = smoothUnion(sphere2, torus, 0.1);
        //closestHit = unionSDF(closestHit, float4(merge, 0.4f, 0.55f, 0.5564));
    }
  
    //    float3 pos = float3(0.0, -0.01, 0);
    //    float s = sphereSDF(samplePoint - float3(0, 0, 0), 0.8);
    //    float a = cubeSDF(samplePoint - float3(0, 0.03, 0), float3(0.2, 0.2, 2));
    //    float sub = smoothSubtraction(a, s, 0.3);
        
    //    closestHit = unionSDF(closestHit, float4(sub, 0.86f, 0.5f, 0.564));
    //    float sphere = smoothUnion(sphereSDF(samplePoint - float3(pos), 0.05), sub, sin(time) + 0.05);
    //    closestHit = unionSDF(closestHit, float4(sphere, 0.456f, 0.15f, 0.5564));
        
  
    //{
    //    float3 pos = float3(-0.4, 0.0, 0.5);
    //    pos.x += sin(time);
    //    pos.z -= cos(time);
    //    float s = sphereSDF(samplePoint - pos, 0.1);
    //    float a = coneSDF(samplePoint - float3(pos.x, pos.y + 0.02, pos.z - 0.02), float3(0.06, 0.06, 0.06));
    //    float sub = smoothUnion(s, a, 0.2);
    //    closestHit = unionSDF(closestHit, float4(sub, 0.456f, 0.15f, 0.5564));
        
    //    float bul = torusSDF(samplePoint - float3(pos.x, sin(time) + pos.y + 0.08, pos.z), float2(0.04, 0.04));
    //    bul = smoothUnion(bul, sub, 0.3);
    //    closestHit = unionSDF(closestHit, float4(bul, 0.1f, 0.15f, 0.89));
    //    bul = torusSDF(samplePoint - float3(pos.x, pos.y - 0.08 * sin(time), sin(time) + pos.z),
    //    float2(0.04, 0.04));
    //    bul = smoothUnion(bul, sub, 0.3);
    //    closestHit = unionSDF(closestHit, float4(bul, 0.1f, 0.16f, 0.89));
    //}
    //float snakeMove = smoothUnion(roundBoxSDF(samplePoint - float3(snakePos.x, snakePos.y, snakePos.z), float3(0.06f, 0.06f, 0.06f), 0.062), roundBoxSDF(samplePoint - float3(snakePos.x, snakePos.y - 0.2f, snakePos.z), float3(0.06f, 0.08f, 0.06f), 0.032),0.03);
    
    ////snakeMove = smoo(snakeMove, coneSDF(samplePoint - float3(snakePos.x - 0.03, snakePos.y + 0.04, snakePos.z + 0.06), float3(0.01f, 0.06f, 0.01f),0.02);
    ////snakeMove = softMax2(snakeMove, coneSDF(samplePoint - float3(snakePos.x - sin(time), snakePos.y, snakePos.z), float3(0.01, 0.01, 0.01)), 0.015);
    
    ////snakeMove = smoothUnion(snakeMove, coneSDF(samplePoint - float3(snakePos.x - cos(time) + 0.003, snakePos.y - 0.02, snakePos.z - 0.6 + sin(time)), float3(0.02, 0.04, 0.06)), 0.03);
    ////snakeMove = smoothUnion(snakeMove, sphereSDF(samplePoint - float3(snakePos.x + cos(time) + 0.003, snakePos.y - 0.02, snakePos.z - 0.6 + sin(time)), 0.03), 0.04);
    ////snakeMove = softMin2(snakeMove, coneSDF(samplePoint - float3(snakePos.x - sin(time), snakePos.y, snakePos.z + cos(time)), float3(0.02, 0.04, 0.02)), 0.064);
    ////snakeMove = softMax2(snakeMove, coneSDF(samplePoint - float3(snakePos.x - sin(time), snakePos.y, snakePos.z), float3(0.02, 0.04, 0.02)),0.08);
    ////snakeMove = smoothUnion(snakeMove, coneSDF(samplePoint - snake.xyz, float3(0.02, 0.04, 0.06)), 0.03);
    //closestHit = unionSDF(closestHit, float4(snakeMove, 0.456f, 0.15f, 0054));
    //closestHit = unionSDF(closestHit, float4(roundConeSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), float3(0.02, 0.0, 0.0), float3(-0.02, 0.06, 0.02), 0.03, 0.01), 0.18f, 0.22f, 1.0f));
    //float uni = smoothUnion(sphereSDF(samplePoint - float3(bodyPos.x + 0.06, bodyPos.y + 0.05, bodyPos.z), 0.05), cylinderSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), 0.01, 0.015, 0.028), 0.1f);
    //uni = smoothUnion(uni, sphereSDF(samplePoint - float3(bodyPos.x-0.06, bodyPos.y+0.05, bodyPos.z), 0.05), 0.02);
    //closestHit = unionSDF(closestHit, float4(uni, 0.3f, 0.6f, 0.4));
    //uni = smoothUnion(uni, roundBoxSDF(samplePoint - float3(bodyPos.xy, bodyPos.z + 0.2), float3(0.05, 0.05, 0.05), 0.0), 0.1);
    
    //closestHit = sphereSDF(closestHit - float3(0.3, 0.5f, 0.3), 1); //    -float4(roundConeSDF(samplePoint - float3(0.3, 0.5f, 0.3), float3(0.02, 0.0, 0.0), float3(-0.02, 0.06, 0.02), 0.03, 0.01));
    //closestHit = unionSDF(closestHit, float4(uni, 0.456f, 0.15f, 0.564));
 //   //MandelBulb
    //closestHit = unionSDF(closestHit, mandelBulb((samplePoint - float3(0.0f, 0.0f, -4.0f))));

	//////SierpinskiTetrahedron
    //closestHit = unionSDF(closestHit, SierpinskiTetrahedron((samplePoint - float3(3.0f, 0.0f, -2.0f))));

	//WobblySphere
    //closestHit = unionSDF(closestHit, float4((sphereSDF(samplePoint - wobblySphere.position, wobblySphere.scale) + lerp((0.04 * sin(30.0 * samplePoint.x) * sin(30.0 * samplePoint.y) * sin(30.0 * samplePoint.z)), (0.04 * sin(60.0 * samplePoint.x) * sin(60.0 * samplePoint.y) * sin(60.0 * samplePoint.z)), sin(time))), float3(0.5, 0.5, 0.5)));
    //closestHit = softAbs2(sphereSDF(samplePoint - float3(bodyPos.x, bodyPos.y, bodyPos.z), .058), cylinderSDF(samplePoint - float3(bodyPos.x, bodyPos.y-0.05, bodyPos.z-0.03), float3(0.02, -0.002, 0.0), float3(-0.02, 0.06, 0.02), 0.016));
	
    
    
    
    ////////Ray Marched Implicit Geometric Primitives
    //closestHit = unionSDF(closestHit, float4(roundConeSDF(samplePoint - float3(0.3, 0.5f, 0.3), float3(0.02, 0.0, 0.0), float3(-0.02, 0.06, 0.02), 0.03, 0.01), 0.18f, 0.22f, 1.0f));
    //closestHit = unionSDF(closestHit, float4(coneSDF(samplePoint - float3(0.0, 0.53f, 0.0), float3(0.16, 0.12, 0.06)), 0.55f, 0.23f, 0.38f));
    //closestHit = unionSDF(closestHit, float4(cappedConeSDF(samplePoint - float3(0.3, 0.5f, 0.0f), 0.03, 0.04, 0.02), 0.80f, 0.78f, 0.45f));
    //closestHit = unionSDF(closestHit, float4(0.6 * torusSDF(twistSDF(samplePoint - float3(0.0, 0.5f, 0.3), 60.0f), float2(0.04, 0.01)), 0.28f, 0.51f, 0.08f));
    //closestHit = unionSDF(closestHit, float4(torusSDF(samplePoint - float3(-0.3, 0.5f, -0.3), float2(0.04, 0.01)), 0.41f, 0.27f, 0.54f));
    //closestHit = unionSDF(closestHit, float4(torus82SDF(samplePoint - float3(0.0, 0.5f, -0.3), float2(0.04, 0.01)), 0.52f, 0.75f, 0.42f));
    //closestHit = unionSDF(closestHit, float4(boxSDF(samplePoint - float3(-0.3, 0.5f, 0.0), float3(0.05f, 0.05f, 0.05f)), 0.31f, 0.47f, 0.63f));
    //closestHit = unionSDF(closestHit, float4(roundBoxSDF(samplePoint - float3(-0.3, 0.5f, 0.3), float3(0.04f, 0.04f, 0.04f), 0.016), 1.0f, 0.27f, 0.0f));
    //closestHit = unionSDF(closestHit, float4(ellipsoidSDF(samplePoint - float3(0.3, 0.5f, -0.3), float3(0.05, 0.05, 0.02)), 0.8f, 0.41f, 0.79f));
    //closestHit = unionSDF(closestHit, float4(triPrismSDF(samplePoint - float3(-0.6, 0.5f, -0.3), float2(0.05, 0.02)), 0.92f, 0.68f, 0.92f));
    //closestHit = unionSDF(closestHit, float4(cylinderSDF(samplePoint - float3(-0.6, 0.5f, 0.0), float3(0.002, -0.002, 0.0), float3(-0.02, 0.06, 0.02), 0.016), 0.78f, 0.38f, 0.08f));
    //closestHit = unionSDF(closestHit, float4(cylinderSDF(samplePoint - float3(-0.6, 0.5f, 0.3), float2(0.02, 0.04)), 0.98f, 0.63f, 0.42f));
    //closestHit = unionSDF(closestHit, float4(cylinder6SDF(samplePoint - float3(0.3, 0.5f, 0.6), float2(0.02, 0.04)), 0.29f, 0.46f, 0.43f));
    //closestHit = unionSDF(closestHit, float4(octahedronSDF(samplePoint - float3(0.0, 0.5f, 0.6), 0.07), 0.46f, 0.61f, 0.52f));
    //closestHit = unionSDF(closestHit, float4(hexPrismSDF(samplePoint - float3(-0.3, 0.5f, 0.6), float2(0.05, 0.01)), 0.59f, 1.0f, 1.0f));
    //closestHit = unionSDF(closestHit, float4(roundConeSDF(samplePoint - float3(-0.6, 0.5f, 0.6), 0.04, 0.02, 0.06), 1.0f, 0.2f, 0.0f));
    float3 basicPos = float3(-0.8, 1.1, -2.5);
    float space = 0.15;
    ////float3(basicPos.x, basicPos.y, basicPos.z);
    ////////Ray Marched Implicit Geometric Primitives
    closestHit = unionSDF(closestHit, float4(roundConeSDF(samplePoint - float3(basicPos.x, basicPos.y, basicPos.z), float3(0.02, 0.0, 0.0), float3(-0.02, 0.06, 0.02), 0.03, 0.01), 0.18f, 0.22f, 1.0f));
    closestHit = unionSDF(closestHit, float4(coneSDF(samplePoint - float3(basicPos.x - space, basicPos.y, basicPos.z), float3(0.16, 0.12, 0.06)), 0.55f, 0.23f, 0.38f));
    closestHit = unionSDF(closestHit, float4(cappedConeSDF(samplePoint - float3(basicPos.x - (2 * space), basicPos.y, basicPos.z), 0.03, 0.04, 0.02), 0.80f, 0.78f, 0.45f));
    closestHit = unionSDF(closestHit, float4(0.6 * torusSDF(cubeSDF(samplePoint - float3(basicPos.x - (3 * space), basicPos.y, basicPos.z), 60.0f), float2(0.04, 0.01)), 0.28f, 0.51f, 0.08f));
    basicPos.y -= 0.15;
    closestHit = unionSDF(closestHit, float4(torusSDF(samplePoint - float3(basicPos.x, basicPos.y, basicPos.z), float2(0.04, 0.01)), 0.41f, 0.27f, 0.54f));
    closestHit = unionSDF(closestHit, float4(torus82SDF(samplePoint - float3(basicPos.x - space, basicPos.y, basicPos.z), float2(0.04, 0.01)), 0.52f, 0.75f, 0.42f));
    closestHit = unionSDF(closestHit, float4(boxSDF(samplePoint - float3(basicPos.x - (2 * space), basicPos.y, basicPos.z), float3(0.05f, 0.05f, 0.05f)), 0.31f, 0.47f, 0.63f));
    closestHit = unionSDF(closestHit, float4(roundBoxSDF(samplePoint - float3(basicPos.x - (3 * space), basicPos.y, basicPos.z), float3(0.04f, 0.04f, 0.04f), 0.016), 1.0f, 0.27f, 0.0f));
    basicPos.y -= 0.15;
    closestHit = unionSDF(closestHit, float4(ellipsoidSDF(samplePoint - float3(basicPos.x, basicPos.y, basicPos.z), float3(0.05, 0.05, 0.02)), 0.8f, 0.41f, 0.79f));
    closestHit = unionSDF(closestHit, float4(triPrismSDF(samplePoint - float3(basicPos.x - space, basicPos.y, basicPos.z), float2(0.05, 0.02)), 0.92f, 0.68f, 0.92f));
    closestHit = unionSDF(closestHit, float4(cylinderSDF(samplePoint - float3(basicPos.x - (2 * space), basicPos.y, basicPos.z), float3(0.002, -0.002, 0.0), float3(-0.02, 0.06, 0.02), 0.016), 0.78f, 0.38f, 0.08f));
    closestHit = unionSDF(closestHit, float4(cylinderSDF(samplePoint - float3(basicPos.x - (3 * space), basicPos.y, basicPos.z), float2(0.02, 0.04)), 0.98f, 0.63f, 0.42f));
    basicPos.y -= 0.15;
    closestHit = unionSDF(closestHit, float4(cylinder6SDF(samplePoint - float3(basicPos.x, basicPos.y, basicPos.z), float2(0.02, 0.04)), 0.29f, 0.46f, 0.43f));
    closestHit = unionSDF(closestHit, float4(octahedronSDF(samplePoint - float3(basicPos.x - space, basicPos.y, basicPos.z), 0.07), 0.46f, 0.61f, 0.52f));
    closestHit = unionSDF(closestHit, float4(hexPrismSDF(samplePoint - float3(basicPos.x - (2 * space), basicPos.y, basicPos.z), float2(0.05, 0.01)), 0.59f, 1.0f, 1.0f));
    closestHit = unionSDF(closestHit, float4(roundConeSDF(samplePoint - float3(basicPos.x - (3 * space), basicPos.y, basicPos.z), 0.04, 0.02, 0.06), 1.0f, 0.2f, 0.0f));


    return closestHit;
}

//Calculate surface normals using the gradiant around a point by sampling through SDF
float3 estimateGradiantNormal(float3 p)
{
    return normalize(float3(sceneSDF(float3(p.x + EPSILON, p.y, p.z)).x - sceneSDF(float3(p.x - EPSILON, p.y, p.z)).x,
		sceneSDF(float3(p.x, p.y + EPSILON, p.z)).x - sceneSDF(float3(p.x, p.y - EPSILON, p.z)).x,
		sceneSDF(float3(p.x, p.y, p.z + EPSILON)).x - sceneSDF(float3(p.x, p.y, p.z - EPSILON)).x));
}

float4 PhongIllumination(float surfacePoint, float3 normal, float shininess, float3 rayDirection, float4 diffuseColour)
{
    float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    {
        totalAmbient += lights[i].ambientColour * diffuseColour;

        float3 lightDirection = normalize(lights[i].lightPosition - surfacePoint);
        float nDotL = dot(normal, lightDirection);
        float3 reflection = normalize(reflect(-lightDirection, normal));
		//float3 reflection = normalize(((2.0f * normal) * nDotL) - lightDirection);
        float rDotV = max(0.0f, dot(reflection, -rayDirection));

        totalDiffuse += saturate(lights[i].diffuseColour * nDotL * diffuseColour);

        if (nDotL > 0.0f)
        {
            float4 specularIntensity = float4(1.0, 1.0, 1.0, 1.0);
            totalSpecular += lights[i].specularColour * pow(pow(rDotV, lights[i].specularPower), shininess) * specularIntensity;
        }
    }

    return totalAmbient + totalDiffuse + totalSpecular;
}

//start = starting distance away from origin
//end = max travel distance away from origin
float4 rayMarching(Ray ray, float start, float end)
{
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        float4 distanceAndColour = sceneSDF(ray.o + depth * ray.d);

        if (distanceAndColour.x < EPSILON)
        {
			//Hit the surface
            return float4(depth, distanceAndColour.yzw);
        }

		//Move along the ray
        depth += distanceAndColour.x;

        if (depth >= end)
        {
			//Give up, nothing hit along max travel distance
            return float4(end, 0.0f, 0.0f, 0.0f);
        }
    }

    return float4(end, 0.0f, 0.0f, 0.0f);
}

struct outputPS
{
    float4 colour : SV_TARGET;
    float depth : SV_DEPTH;
};

// A pass-through function for the (interpolated) color data.
outputPS main(PixelShaderInput input)
{
    outputPS output;

    float3 PixelPos = float3(input.canvasXY, -1);


    Ray eyeray;
    eyeray.o = mul(float4(float3(0.0f, 0.0f, 0.0f), 1.0f), inverse);
    eyeray.d = normalize(mul(float4(PixelPos, 0.0f), inverse));

    float4 distanceAndColour = rayMarching(eyeray, EPSILON, MAX_DIST);

    if (distanceAndColour.x > MAX_DIST - EPSILON)
    {
        discard;
	
    }

    float3 surfacePoint = cameraPosition + distanceAndColour.x * eyeray.d;

    float4 pv = mul(float4(surfacePoint, 1.0f), view);
    pv = mul(pv, projection);
    output.depth = pv.z / pv.w;

    output.colour = PhongIllumination(surfacePoint, estimateGradiantNormal(surfacePoint), 60.0f, eyeray.d, float4(distanceAndColour.yzw, 1.0));

    output.colour = float4(lerp(output.colour.xyz, float3(1.0f, 0.97255f, 0.86275f), 1.0 - exp(-0.0005 * distanceAndColour.x * distanceAndColour.x * distanceAndColour.x)), 1.0f);

    return output;
}
//https://www.shadertoy.com/view/lt3BW2