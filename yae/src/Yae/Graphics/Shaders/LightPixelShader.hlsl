Texture2D positionGB : register(t0);
Texture2D normalGB : register(t1);
Texture2D diffuseGB : register(t2);
SamplerState Sampler : register(s0);

cbuffer Data : register(b0)
{
    float3 cameraPos;
    float3 lightColor;
    float3 lightPos;
    float radius;
    float intensity;
    float falloff;
}

struct PixelInput
{
    float4 position : SV_POSITION;
};


float sqr(float x)
{
    return x * x;
}

float CalculateAttenuation(float dist)
{
    float s = dist / radius;
    if (s >= 1.0f)
    {
        return 0.0f;
    }

    float s2 = sqr(s);
    return intensity * sqr(1 - s2) / (1 + falloff * s);
}

float4 CalculatePointLight(float3 normal, float3 viewDir, float4 textureColor, float3 pos)
{
    float3 lightDir = normalize(pos);
    float diff = max(dot(normal, lightDir), 0.0f);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    
    float dist = length(pos);
    //float atten = 1.0f / (light.constantFactor + light.linearFactor * dist + light.quadraticFactor * dist * dist);
    float atten = CalculateAttenuation(dist);

    float4 ambient = float4(0.15f, 0.15f, 0.15f, 1.0f) * textureColor * atten;
    float4 dif = float4(lightColor, 1.0f) * diff * textureColor * atten;
    float4 specColor = float4(0.15f, 0.15f, 0.15f, 1.0f) * spec * textureColor * atten;

    return ambient + dif + specColor;
}

float4 main(PixelInput input) : SV_TARGET
{
    int3 sampleIndices = int3(input.position.xy, 0);
    float3 normal = normalGB.Load(sampleIndices).xyz;
    float3 position = positionGB.Load(sampleIndices).xyz;
    float4 diffuse = diffuseGB.Load(sampleIndices);

    float4 color = CalculatePointLight(normal, normalize(cameraPos - position), diffuse, lightPos - position);

    return color;
}