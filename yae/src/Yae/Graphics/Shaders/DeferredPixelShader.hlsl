Texture2D textureSRV : register(t0);
Texture2D textureSRVBump : register(t1);
SamplerState Sampler : register(s0);

cbuffer Data : register(b0)
{
    float4 tintColor;
    float4 ambientColor;
    float4 dirLightColor;
    float3 lightDirection;
    float padding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : POSITIONWS;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
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
    float4 diff = diffuse * tintColor;
    float4 color = ambientColor * diff;

    float4 bump = textureSRVBump.Sample(Sampler, input.uv);
    bump = bump * 2.0f - 1.0f;
    float3 bumpNormal = (bump.x * input.tangent) + (bump.y * input.binormal) + (bump.z * input.normal);
    bumpNormal = normalize(bumpNormal);

    float3 lightDir = -lightDirection;
    float lightIntensity = saturate(dot(bumpNormal, lightDir));

    color += dirLightColor * lightIntensity * diff;

    if (lightIntensity > 0.0f)
    {
        float3 reflectDir = reflect(normalize(-lightDir), bumpNormal);
        float spec = pow(saturate(dot(input.viewDirection, reflectDir)), 32.0f);
        float4 specularColor = dirLightColor * spec * diff;
        color += specularColor;
    }



    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(bumpNormal, 1.0f);

    //float4 color = diffuse * tintColor;

    //color *= tintColor;
    output.diffuse = saturate(color);
    //output.diffuse = saturate(diffuse * tintColor);
    //output.diffuse = saturate(lightIntensity * diffuse * tintColor);
    //output.diffuse = saturate(ambientColor * lightIntensity * diffuse * tintColor);

    return output;
}