Texture2D diffuse : register(t0);
SamplerState Sampler : register(s0);


cbuffer Data : register(b0)
{
    float4 fontColor;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 textureColor = diffuse.Sample(Sampler, input.tex);

    if(textureColor.r == 0.0f)
    {
        textureColor.a = 0.0f;
    }

    textureColor *= fontColor;

    return textureColor;
}