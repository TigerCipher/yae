#define NUM_LIGHTS 4

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

struct PointLight
{
    float4 diffuseColor;
    float constantFactor;
    float linearFactor;
    float quadraticFactor;
    float padding;
};

cbuffer PointLightBuffer
{
    PointLight pointLights[NUM_LIGHTS];
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

float4 CalculatePointLight(PointLight light, float3 lightPos, float3 normal, float3 viewDir, float4 textureColor)
{
    float3 lightDir = lightPos;
    float diff = max(dot(normal, lightDir), 0.0f);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), specularPower);
    
    float dist = length(lightDir);
    float atten = 1.0f / (light.constantFactor + light.linearFactor * dist + light.quadraticFactor * dist * dist);

    float4 ambient = ambientColor * textureColor * atten;
    float4 dif = light.diffuseColor * diff * textureColor * atten;
    float4 specColor = ambientColor * spec * textureColor * atten;

    return ambient + dif + specColor;
}


float4 LightPixelShader(PixelInputType input) : SV_TARGET
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
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (pointLights[i].diffuseColor.a > 0.0f)
        {
            color += CalculatePointLight(pointLights[i], input.lightPos[i], input.normal, viewDir, textureColor);
        }
    }

    //color += diffuse_color + specular;
    //color = saturate(color);
    //color *= textureColor;

    return color;
}