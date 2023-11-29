


struct PixelInput
{
    float4 position : SV_POSITION;
};

PixelInput main(uint id : SV_VertexID)
{
    PixelInput output;

    float2 uv = float2((id << 1) & 2, id & 2);

    output.position = float4(uv, 0, 1);
    output.position.x = output.position.x * 2 - 1;
    output.position.y = output.position.y * -2 + 1;

	return output;
}