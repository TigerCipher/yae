Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

/*
struct PointLight
{
    float4 diffuseColor;
    float constantFactor;
    float linearFactor;
    float quadraticFactor;
    float padding;
};
*/

struct PointLight
{
    float4 diffuseColor;
    float radius;
    float intensity;
    float falloff;
};

cbuffer PointLightBuffer
{
    PointLight pointLights;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 viewDirection : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

float sqr(float x)
{
    return x * x;
}

float CalculateAttenuation(float dist, float radius, float intensity, float falloff)
{
    float s = dist / radius;
    if(s >= 1.0f)
    {
        return 0.0f;
    }

    float s2 = sqr(s);
    return intensity * sqr(1 - s2) / (1 + falloff * s);
}

float4 CalculatePointLight(PointLight light, float3 lightPos, float3 normal, float3 viewDir, float4 textureColor)
{
    float3 lightDir = normalize(lightPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), specularPower);
    
    float dist = length(lightPos);
    //float atten = 1.0f / (light.constantFactor + light.linearFactor * dist + light.quadraticFactor * dist * dist);
    float atten = CalculateAttenuation(dist, light.radius, light.intensity, light.falloff);

    float4 ambient = ambientColor * textureColor * atten;
    float4 dif = light.diffuseColor * diff * textureColor * atten;
    float4 specColor = ambientColor * spec * textureColor * atten;

    return ambient + dif + specColor;
}


float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

    //float4 color = ambientColor;

    float3 lightDir = normalize(-lightDirection);
    float diff = max(dot(input.normal, lightDir), 0.0f);
    float3 reflectDir = reflect(-lightDir, input.normal);
    float3 viewDir = input.viewDirection;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), specularPower);

    float4 color = ambientColor * textureColor;
    color += diffuseColor * diff * textureColor;
    color += specularColor * spec * textureColor;

    //color = saturate(color);
    //color += specular;

    // Point lights
    if (pointLights.diffuseColor.a > 0.0f)
    {
        color += CalculatePointLight(pointLights, input.lightPos, input.normal, viewDir, textureColor);
    }

    //color += diffuse_color + specular;
    //color = saturate(color);
    //color *= textureColor;

    return color;
}