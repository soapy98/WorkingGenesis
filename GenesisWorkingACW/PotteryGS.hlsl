struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}
//// A constant buffer that stores the three basic column-major matrices for composing geometry.
//cbuffer ModelViewProjectionConstantBuffer : register(b0)
//{
//    matrix model;
//    matrix view;
//    matrix projection;
//};
//cbuffer CameraBuffer : register(b1)
//{
//    float3 cameraPos;
//    float cameraPadding;
//};

//float3 calcNormal(float3 a, float3 b, float3 c)
//{
//    return normalize(cross(b - a, c - a));
//}

//float3 calcTangent(float3 v1, float3 v2, float2 tuVector, float2 tvVector)
//{
//    float3 tangent;
//    float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

//    tangent.x = (tvVector.y * v1.x - tvVector.x * v2.x) * den;
//    tangent.y = (tvVector.y * v1.y - tvVector.x * v2.y) * den;
//    tangent.z = (tvVector.y * v1.z - tvVector.x * v2.z) * den;

//    float length = sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);

//    tangent.x = tangent.x / length;
//    tangent.y = tangent.y / length;
//    tangent.z = tangent.z / length;

//    return tangent;
//}

//float3 calcBinormal(float3 v1, float3 v2, float2 tuVector, float2 tvVector)
//{
//    float3 binormal;
//    float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

//    binormal.x = (tuVector.x * v2.x - tuVector.y * v1.x) * den;
//    binormal.y = (tuVector.x * v2.y - tuVector.y * v1.y) * den;
//    binormal.z = (tuVector.x * v2.z - tuVector.y * v1.z) * den;

//    float length = sqrt(binormal.x * binormal.x + binormal.y * binormal.y + binormal.z * binormal.z);

//    binormal.x = binormal.x / length;
//    binormal.y = binormal.y / length;
//    binormal.z = binormal.z / length;

//    return binormal;
//}
//struct GSinput
//{
//    float4 position : SV_Position;
//    float2 uv : TEXCOORD;
//};
//struct GSOutput
//{
//    float4 positionH : SV_POSITION;
//    float3 positionW : POSITION;
//    float3 normal : NORMAL;
//    float2 uv : TEXCOORD;
//    float3 tangent : TANGENT;
//    float3 binormal : BINORMAL;
//};

//[maxvertexcount(3)]
//void main(triangle GSinput input[3], inout TriangleStream<GSOutput> output)
//{
//    for (uint i = 0; i < 3; i++)
//    {
//        GSOutput element;
//        element.positionH = input[i];
//        element.positionW = input[i];
//        element.uv = (sign(input[i].position) + 1.0) / 2.0;
//        float3 p0 = mul(input[0].position, model);

//        float3 p1 = mul(input[1].position, model);

//        float3 p2 = mul(input[2].position, model);
//        float3 d1 = p1 - p0;
//        float3 d2 = p2 - p0;
//        element.normal = normalize(cross(d1, d2));
//        float3 v1 = input[1].uv - input[0].uv;
//        float3 v2 = input[2].uv - input[0].uv;

//        float2 tuVector, tvVector;
//        tuVector.x = input[1].uv - input[0].uv.x;
//        tvVector.x = input[1].uv.y - input[0].uv.y;
//        tuVector.y = input[2].uv.x - input[0].uv.x;
//        tvVector.y = input[2].uv.y - input[0].uv.y;

//        element.tangent = normalize(calcTangent(v1, v2, tuVector, tvVector));
//        element.binormal = normalize(calcBinormal(v1, v2, tuVector, tvVector));

//        output.Append(element);
//    }
//}
