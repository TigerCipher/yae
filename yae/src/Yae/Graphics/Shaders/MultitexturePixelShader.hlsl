Texture2D textureSRV : register(t0);
Texture2D textureSRVBlend : register(t1);
Texture2D textureSRVBump : register(t2);
SamplerState Sampler : register(s0);

cbuffer Data : register(b0)
{
    float4 tintColor;
    float4 ambientColor;
    float3 lightDirection;
    float padding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : POSITIONWS;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
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
    const float4 blend = textureSRVBlend.Sample(Sampler, input.uv);

    float4 bump = textureSRVBump.Sample(Sampler, input.uv);
    bump = bump * 2.0f - 1.0f;
    float3 bumpNormal = (bump.x * input.tangent) + (bump.y * input.binormal) + (bump.z * input.normal);
    bumpNormal = normalize(bumpNormal);

    float3 lightDir = -lightDirection;
    float lightIntensity = saturate(dot(bumpNormal, lightDir));

    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(bumpNormal, 1.0f);
    float4 col = saturate(diffuse * blend * 2.0f) * tintColor;
    //output.diffuse = saturate(ambientColor * lightIntensity * col);
    output.diffuse = col;

    return output;
}