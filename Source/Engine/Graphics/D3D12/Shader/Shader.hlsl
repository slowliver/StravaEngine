struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float2 uv : TEXCOORD;
};

Texture2D TestTexture : register(t0);
//SamplerState TestSamplerState : register(s0);

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color;
	result.uv = position.xy;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float3 color = TestTexture.Load(int3(input.uv.x * 128, input.uv.y * 128, 0)).xyz;
    return float4(color, 1.0f);
}

float4 VSMainFull(uint id : SV_VertexID) : SV_POSITION
{
	float2 uv = float2((id << 1) & 2, id & 2);
	return float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
}

float4 PSMainFull(float4 position : SV_POSITION) : SV_TARGET
{
	return float4(position.xy, 0, 0);
}