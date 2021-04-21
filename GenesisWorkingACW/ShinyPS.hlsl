static float nearPlane = 1.0;
static float farPlane = 100.0;
static float4 LightColor = float4(1, 1, 1, 1);
static float3 LightPos = float3(5, 100, -5);
static float4 backgroundColor = float4(0.1, 0.2, 0.3, 1);
static float4 sphereColor_1 = float4(1, 1, 0, 1); //sphere1 color
static float4 sphereColor_2 = float4(0, 1, 0, 1); //sphere2 color
static float4 sphereColor_3 = float4(1, 0, 1, 1); //sphere3 color
static float shininess = 400;
static float MIN_DIST = 1.0;
static float MAX_DIST = 1000.0;
static float EPSILON = 0.0001;
#define vec3 float3
#define vec4 float4
#define mat4  float4x4

static float4 lightColour = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float3 lightPosition = float3(0.0f, 3.0f, 0.0f);
#define NOBJECTS 4
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
cbuffer Camera : register(b2)
{
    float3 CamPos;
    float padding;
}
//struct Sphere
//{
//    float3 centre;
//    float rad2; // radius* radius
//    float4 color;
//    float Kd, Ks, Kr, shininess;
//};
//static Sphere object[NOBJECTS] =
//{
////sphere 1
//    { 0.0, 0.0, 0.0, 1.0, sphereColor_1, 0.3, 0.5, 0.7, shininess },
////sphere 2
//    { 2.0, -1.0, 0.0, 0.25, sphereColor_2, 0.5, 0.7, 0.4, shininess },
////sphere 3
//    { -2.0, -2.0, 1.0, 2, sphereColor_3, 0.5, 0.3, 0.3, shininess }
//};
static const int CubeOffset = 3;
static int cubAmount = 1;
static int sphereAmount = 3;
struct Cube
{
    float3 mi;
    float3 ma;
};
struct Sphere
{
    float3 centre;
    float rad; // radius* radius
};
static Sphere sphere[3] =
{
    { 1, -2, 1, 2 },
    { 2, -1, 1, 2 },
    { 0, 2, 1, 2 }
};

static Cube cube[1] =
{
    { -1, -1,-1 ,1, 1, 1 }
};

struct ShinyObj
{
    float4 color;
    float Kd, Ks, Kr, shininess;
};
static ShinyObj objects[NOBJECTS] =
{
//sphere 1
    { sphereColor_1, 0.3, 0.5, 0.7, shininess },
//sphe
    { sphereColor_2, 0.5, 0.7, 0.4, shininess },
//sphe
    { sphereColor_3, 0.5, 0.3, 0.3, shininess },
    //
    { sphereColor_3, 0.5, 0.3, 0.3, shininess }
};
float noise(float3 pos)
{
    float h = dot(pos, pos);
    return h = frac(sin(h));
}


struct VS_QUAD
{
    float4 Position : SV_POSITION;
    float2 canvasXY : TEXCOORD0;
};


struct Ray
{
    float3 o; // origin
    float3 d; // direction
};
//struct Cube
//{
//    float3 centre;
//    float xSize;
//    float ySize;
//    float4 color;
//    float Kd, Ks, Kr, shininess;
//};
//static Cube cub[1]=
//{
//    {0,0,0,0.5,2, sphereColor_1, 0.5, 0.3, 0.3, shininess}
//};

float3 get_normal(const int face_hit)
{
    switch (face_hit)
    {
        case 0:
            return (float3(-1, 0, 0)); // -x face
        case 1:
            return (float3(0, -1, 0)); // -y face
        case 2:
            return (float3(0, 0, -1)); // -z face
        case 3:
            return (float3(1, 0, 0)); // +x face
        case 4:
            return (float3(0, 1, 0)); // +y face
        case 5:
            return (float3(0, 0, 1)); // +z face
    }
    return float3(0, 0, 0);

}
float Cubehit(Ray ray, Cube cube, out bool hit, out float3 normal)
{
    float t = -1;
    float3 p0 = cube.mi;
    float3 p1 = cube.ma;
    float3 o = ray.o;
    float3 d = ray.d;
    float3 t_min;
    float3 t_max;

    double a = 1.0 / d.x;
    if (a >= 0)
    {
        t_min.x = (p0.x - o.x) * a;
        t_max.x = (p1.x - o.x) * a;
    }
    else
    {
        t_min.x = (p1.x - o.x) * a;
        t_max.x = (p0.x - o.x) * a;
    }
    double b = 1.0 / d.y;
    if (b >= 0)
    {
        t_min.y = (p0.y - o.y) * b;
        t_max.y = (p1.y - o.y) * b;
    }
    else
    {
        t_min.y = (p1.y - o.y) * b;
        t_max.y = (p0.y - o.y) * b;
    }
    double c = 1.0 / d.z;
    if (c >= 0)
    {
        t_min.z = (p0.z - o.z) * c;
        t_max.z = (p1.z - o.z) * c;
    }
    else
    {
        t_min.z = (p1.z - o.z) * c;
        t_max.z = (p0.z - o.z) * c;
    }
    double t0, t1;
    int face_in, face_out;
    // finding largest
    if (t_min.x > t_min.y)
    {
        t0 = t_min.x;
        face_in = (a >= 0.0) ? 0 : 3;
    }
    else
    {
        t0 = t_min.y;
        face_in = (b >= 0.0) ? 1 : 4;
    }
    if (t_min.z > t0)
    {
        t0 = t_min.z;
        face_in = (c >= 0.0) ? 2 : 5;
    }
    // find smallest
    if (t_max.x < t_max.y)
    {
        t1 = t_max.x;
        face_out = (a >= 0.0) ? 3 : 0;
    }
    else
    {
        t1 = t_max.y;
        face_out = (b >= 0.0) ? 4 : 1;
    }
    if (t_max.z < t1)
    {
        t1 = t_max.z;
        face_out = (c >= 0.0) ? 5 : 2;
    }
    if (t0 < t1 && t1 > EPSILON)
    {
        if (t0 > EPSILON)
        {
            t = t0;
            normal = get_normal(face_in);
        }
        else
        {
            t = t1;
            normal = get_normal(face_out);
        }
        //local_hit_point = ray.o + t*ray.d;
        hit = true;
        return t;
    }
    else
    {
        hit = false;
        return t;
    }
}


float SphereIntersect(Sphere s, Ray ray, out bool hit)
{
    float t;
    float3 v = s.centre - ray.o;
    float A = dot(v, ray.d);
    float B = dot(v, v) - A * A;
    float R = sqrt(s.rad);
    if (B > R * R)
    {
        hit = false;
        t = farPlane;
    }
    else
    {
        float disc = sqrt(R * R - B);
        t = A - disc;
        if (t < 0.0)
        {
            hit = false;
        }
        else
            hit = true;
    }
    return t;
}
bool AnyHit(Ray ray, out float3 n)
{
    bool anyhit = false;
    for (int i = 0; i < sphereAmount; i++)
    {
        bool hit;
        float t = SphereIntersect(sphere[i], ray, hit);
        if (hit)
        {
            anyhit = true;
        }
    }
    for (int j = 0; j < cubAmount; j++)
    {
        bool hit;
        float t = Cubehit(ray, cube[j], hit, n);
        if (hit)
        {
            anyhit = true;
        }
    }
    return anyhit;
}

float3 SphereNormal(Sphere s, float3 pos)
{
    return normalize(pos - s.centre);
}
float3 NearestHit(Ray ray, out int hitobj, out bool anyhit, out float3 norm)
{
    float mint = farPlane;
    hitobj = -1;
    anyhit = false;
    for (int i = 0; i < sphereAmount; i++)
    {
        bool hit = false;
        float t = SphereIntersect(sphere[i], ray, hit);
        if (hit)
        {
            if (t < mint)
            {
                hitobj = i;
                mint = t;
                anyhit = true;
            }
        }
    }
    for (int j = 0; j < 1; j++)
    {
        bool hit = false;
        float3 cn;
        float t = Cubehit(ray, cube[j], hit, cn);
        if (hit)
        {
            if (t < mint)
            {
                hitobj = j + CubeOffset;
                mint = t;
                anyhit = true;
                norm = cn;
            }
        }
    }
    
    return ray.o + ray.d * mint;
}


float4 Phong(float3 n, float3 l, float3 v, float shininess, float4
diffuseColor, float4 specularColor)
{
    float NdotL = dot(n, l);
    float diff = saturate(NdotL);
    float3 r = reflect(l, n);
    float spec = pow(saturate(dot(v, r)), shininess) * (NdotL > 0.0);
    return diff * diffuseColor + spec * specularColor;
}

float4 Shade(float3 hitPos, float3 normal,
float3 viewDir, int hitobj, float lightIntensity, Ray ray)
{
  
    float3 lightDir = normalize(LightPos - hitPos);
  
    float4 diff = objects[hitobj].color * objects[hitobj].Kd;
    float4 spec = objects[hitobj].color * objects[hitobj].Ks;
    if (hitobj == 0)
    {
        //diff *= noise(hitPos);
        diff *= frac(frac(frac(fmod(hitPos.y, hitPos.x) / 3) * 100));
        //diff *= (1 + frac(sin(hitPos.y * 10)) / 2);
    }
    return LightColor * lightIntensity * Phong(normal, lightDir, viewDir, objects[hitobj].shininess, diff, spec);

}
float4 RayTracing(Ray ray)
{
    int hitobj;
    bool hit = false;
    float3 n;
    float4 c = (float4) 0;
    float lightInensity = 10.0;
//calculate the nearest hit
    float3 localHit = NearestHit(ray, hitobj, hit, n);
    for (int depth = 1; depth < 5; depth++)
    {
        if (hit)
        {
            if (hitobj < 3)
                n = SphereNormal(sphere[hitobj], localHit);
            c += Shade(localHit, n, ray.d, hitobj, lightInensity, ray);
// shoot refleced ray
            lightInensity *= objects[hitobj].Kr;
            ray.o = localHit;
            ray.d = reflect(ray.d, n);
            localHit = NearestHit(ray, hitobj, hit, n);
        }
        
    }
    return c;
}


struct outputPS
{
    float4 colour : SV_TARGET;
    float depth : SV_DEPTH;
};

// A pass-through function for the (interpolated) color data.
outputPS main(VS_QUAD input) : SV_Target
{

    outputPS output;

    float3 PixelPos = float3(input.canvasXY, -5);


    Ray eyeray;
    eyeray.o = mul(float4(float3(0.0f, 0.0f, 0.0f), 1.0f), inverse);
    eyeray.d = normalize(mul(float4(PixelPos, 0.0f), inverse));

    float4 distanceAndColour = RayTracing(eyeray);

    if (distanceAndColour.x > MAX_DIST - EPSILON)
    {
        discard;
    }

    float3 surfacePoint = CamPos + distanceAndColour.x * eyeray.d;

    float4 pv = mul(float4(surfacePoint, 1.0f), view);
    pv = mul(pv, projection);
    output.depth = pv.z / pv.w;

    output.colour = float4(distanceAndColour.yzw, 1.0f);

    output.colour = float4(lerp(output.colour.xyz, float3(1.0f, 0.97255f, 0.86275f), 1.0 - exp(-0.0005 * distanceAndColour.x * distanceAndColour.x * distanceAndColour.x)), 1.0f);

    return output;
    
}


//float4 main(VS_QUAD input) : SV_Target
//{
//// specify primary ray:
//    Ray eyeray;
//    eyeray.o = mul(float4(float3(0.0f, 0.0f, 0.0f), 1.0f), inverse);
//// set ray direction in view space
//    float dist2Imageplane =0.1;
//    float3 viewDir = float3(input.canvasXY, -dist2Imageplane);
//    viewDir = normalize(viewDir);
//    float4x4 viewTrans = transpose(inverse);
//    eyeray.d = normalize(mul(float4(viewDir, 0.0f), inverse));
//    return RayTracing(eyeray);
//}