Texture2D textureSRV : register(t0);
Texture2D textureSRVBump : register(t1);
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

    //float3 n = input.normal;
    //float3 t = normalize(input.tangent - n * dot(input.tangent, n));
    //float3 bitan = cross(t, n);
    //float3x3 tanframe = float3x3(normalize(input.tangent), normalize(bitan), normalize(input.normal));
    float4 bump = textureSRVBump.Sample(Sampler, input.uv);
    //bump = normalize(bump * 2.0f - 1.0f);
    //float3 normWS = mul(bump, tanframe);
    bump = bump * 2.0f - 1.0f;
    float3 bumpNormal = (bump.x * input.tangent) + (bump.y * input.binormal) + (bump.z * input.normal);
    bumpNormal = normalize(bumpNormal);

    float3 lightDir = -lightDirection;
    float lightIntensity = saturate(dot(bumpNormal, lightDir));

    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(bumpNormal, 1.0f);
    output.diffuse = saturate(diffuse * tintColor);
    //output.diffuse = saturate(lightIntensity * diffuse * tintColor);
    //output.diffuse = saturate(ambientColor * lightIntensity * diffuse * tintColor);

    return output;
}