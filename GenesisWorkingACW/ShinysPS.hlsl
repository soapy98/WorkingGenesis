cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

cbuffer InverseViewConstantBuffer : register(b1)
{
    matrix inverseView;
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

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 canvasXY : TEXCOORD0;
};

static float MIN_DIST = 1.0;
static float MAX_DIST = 100.0;
static float EPSILON = 0.0001;

static float4 lightColour = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float3 lightPosition[2] = { float3(-10.0f, -10.0f, -10.0f), float3(10.0f, 10.0f, 10.0f) };

static float4 sphereColor_1 = float4(1, 1, 0, 1); //sphere1 color
static float4 sphereColor_2 = float4(0, 1, 0, 1); //sphere2 color
static float4 sphereColor_3 = float4(1, 0, 1, 1); //sphere3 color
static float shininess = 400;
#define vec3 float3
#define vec4 float4
#define mat4  float4x4

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
    { -0.5, 2, 1, 0.1 },
    { 0, 2, 1, 0.1 },
    { 0.5, 2, 1, 0.1 }
};

static Cube cube[1] =
{
    {
        -1, -0.2, 0,
      .5, 1.5, 2
    }
};

struct ShinyObj
{
    float4 color;
    float Kd, Ks, Kr, shininess;
};
static ShinyObj objects[4] =
{
//sphere 1
    { sphereColor_1, 0.3, 0.5, 0.7, shininess },
//sphe
    { sphereColor_2, 0.5, 0.7, 0.4, shininess },
//sphe
    { sphereColor_3, 0.5, 0.3, 0.3, shininess },
    //
    { sphereColor_3, 0.5, 0.3, 0.6, shininess }
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
    return float3(1, 0, 0);

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
        t = MAX_DIST;
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
        {
            hit = true;
        }
    }

    return t;
}

float3 SphereNormal(Sphere s, float3 pos)
{
    return normalize(pos - s.centre);
}



bool AnyHit(Ray ray)
{
    bool hit;
    float t;

    bool anyhit = false;
    for (int i = 0; i < 3; i++)
    {
        t = SphereIntersect(sphere[i], ray, hit);
        if (hit)
        {
            anyhit = true;
        }
    }

    return anyhit;
}

float3 NearestHit(Ray ray, out int hitobj, out bool anyhit, out float mint, out float3 norm)
{
    bool hit = false;
    mint = MAX_DIST;
    hitobj = -1;
    anyhit = false;
    float t;

    for (int i = 0; i < 3; i++)
    {
        t = SphereIntersect(sphere[i], ray, hit);
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

float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor)
{
    float NdotL = dot(n, l);
    float diff = saturate(NdotL);
    float3 r = reflect(l, n);
    float spec = pow(saturate(dot(v, r)), shininess) * (NdotL > 0.0);

    return diff * diffuseColor + spec * specularColor;
}


float4 Shade(float3 hitPos, float3 normal, Ray ray, int hitobj, float lightIntensity)
{
    float4 col;
    for (int i = 0; i < 2; i++)
    {
        float3 lightDir = normalize(lightPosition[i] - hitPos);
        float4 diff = (float4) 0.0f;
        float4 spec = (float4) 0.0f;
        float shininess = 0.0f;

        diff = objects[hitobj].color * objects[hitobj].Kd;
        spec = objects[hitobj].color * objects[hitobj].Ks;
        shininess = objects[hitobj].shininess;
        
        col += lightColour * lightIntensity * Phong(normal, lightDir, ray.d, shininess, diff, spec);
    }

    return col;
	

}

float4 RayTracing(Ray ray)
{
    int hitobj;
    bool hit = false;
    float3 n;
    float4 c = (float4) 0;
    float lightIntensity = 1.0;

    float mint = 0.0f;

	//Calculate nearest hit
    float3 i = NearestHit(ray, hitobj, hit, mint, n);

    for (int depth = 1; depth < 5; depth++)
    {
        if (hit)
        {
            if (hitobj < 3)
            {
                n = SphereNormal(sphere[hitobj], i);
            }

            c += Shade(i, n, ray, hitobj, lightIntensity);

			//Shoot reflect ray
            lightIntensity *= objects[hitobj].Kr;
            ray.o = i;
            ray.d = reflect(ray.d, n);
            float mint2 = 0.0f;
            i = NearestHit(ray, hitobj, hit, mint2, n);
        }
		
    }

    return float4(mint, c.xyz);
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

    float3 PixelPos = float3(input.canvasXY, -MIN_DIST);

    Ray eyeray;
    eyeray.o = mul(float4(float3(0.0f, 0.0f, 0.0f), 1.0f), inverseView);
    eyeray.d = normalize(mul(float4(PixelPos, 0.0f), inverseView));

    float4 distanceAndColour = RayTracing(eyeray);

    if (distanceAndColour.x > MAX_DIST - EPSILON)
    {
        discard;
		//output.colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
		//return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float3 surfacePoint = cameraPosition + distanceAndColour.x * eyeray.d;

    float4 pv = mul(float4(surfacePoint, 1.0f), view);
    pv = mul(pv, projection);
    output.depth = pv.z / pv.w;

    output.colour = float4(distanceAndColour.yzw, 1.0f);

    output.colour = float4(lerp(output.colour.xyz, float3(1.0f, 0.97255f, 0.86275f), 1.0 - exp(-0.0005 * distanceAndColour.x * distanceAndColour.x * distanceAndColour.x)), 1.0f);

    return output;
}