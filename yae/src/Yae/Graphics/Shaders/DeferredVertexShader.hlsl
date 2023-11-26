
cbuffer MatrixData : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

struct VertexInput
{
    float3 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : POSITIONWS;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

    matrix wvp = mul(mul(worldMatrix, viewMatrix), projectionMatrix);
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix).xyz;
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
    output.uv = input.uv;

    output.tangent = mul(input.tangent, (float3x3) worldMatrix);
    output.tangent = normalize(output.tangent);

    output.binormal = mul(input.binormal, (float3x3) worldMatrix);
    output.binormal = normalize(output.binormal);

    return output;
}