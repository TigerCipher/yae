
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
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : POSITIONWS;
    float2 uv : TEXCOORD;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

    matrix wvp = mul(mul(worldMatrix, viewMatrix), projectionMatrix);
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix).xyz;
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
    output.uv = input.uv;

    return output;
}