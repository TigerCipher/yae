Texture2D textureSRV : register(t0);
Texture2D textureSRVBlend : register(t1);
SamplerState Sampler : register(s0);


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

    float4 diffuse = textureSRV.Sample(Sampler, input.uv);
    float4 blend = textureSRVBlend.Sample(Sampler, input.uv);

    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(input.normal, 1.0f);
    output.diffuse = saturate(diffuse * blend * 2.0f);

    return output;
}