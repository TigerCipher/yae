
cbuffer MatrixData : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


PixelInput main( VertexInput input)
{
    PixelInput output;
    float4 pos = float4(input.position, 1.0f);

    output.position = mul(pos, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    return output;
}