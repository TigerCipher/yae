
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

cbuffer LightPositionBuffer
{
    float4 lightPosition;
};

struct VertexInputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 viewDirection : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    float4 worldPos = mul(input.position, worldMatrix);
    output.viewDirection = cameraPosition.xyz - worldPos.xyz;
    output.viewDirection = normalize(output.viewDirection);

    output.lightPos = lightPosition.xyz - worldPos.xyz;
    output.lightPos = normalize(output.lightPos);

    return output;
}