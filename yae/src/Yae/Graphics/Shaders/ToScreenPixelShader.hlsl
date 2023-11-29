Texture2D positionGB : register(t0);
Texture2D normalGB : register(t1);
Texture2D diffuseGB : register(t2);
SamplerState Sampler : register(s0);


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

    //return float4(normal, 1.0);
    //return float4(pos, 1.0);
    return diff;
}