Texture2D positionGB : register(t0);
Texture2D normalGB : register(t1);
Texture2D diffuseGB : register(t2);
SamplerState Sampler : register(s0);


cbuffer LightData : register(b0)
{
    float3 ambientColor;
    float3 lightColor;
    float3 lightDirection;
}

cbuffer CameraData : register(b1)
{
    float3 cameraPos;
}

struct PixelInput
{
    float4 position : SV_POSITION;
};

float4 main(PixelInput input) : SV_TARGET
{
    int3 sampleIndices = int3(input.position.xy, 0);
    float3 normal = normalGB.Load(sampleIndices).xyz;
    float3 pos = positionGB.Load(sampleIndices).xyz;
    float4 diff = diffuseGB.Load(sampleIndices);

    float3 lightDir = normalize(-lightDirection);
    float amnt = max(dot(normal, lightDir), 0.0f);
    float3 reflectDir = reflect(-lightDir, normal);
    float3 viewDir = normalize(cameraPos - pos);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);

    float4 color = float4(ambientColor, 1.0f) * diff;
    color += float4(lightColor, 1.0f) * amnt * diff;
    color += float4(1, 1, 1, 1) * spec * diff;


    return color;
}