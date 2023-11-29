Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer Data : register(b0)
{
    float4 tintColor;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    const float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

    return saturate(textureColor * tintColor);
}