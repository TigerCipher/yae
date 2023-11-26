Texture2D textureSRV : register(t0);
SamplerState Sampler : register(s0);

cbuffer Data : register(b0)
{
    float4 tintColor;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : POSITIONWS;
    float2 uv : TEXCOORD;
};

struct PixelOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 diffuse : SV_Target2;
};

PixelOutput main(PixelInput input)
{
    PixelOutput output;

    const float4 diffuse = textureSRV.Sample(Sampler, input.uv);

    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(input.normal, 1.0f);
    output.diffuse = saturate(diffuse * tintColor);

    return output;
}