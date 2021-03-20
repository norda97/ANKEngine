
Texture2D		tex				: register(t0);
SamplerState	samplerLinear	: register(s0);

struct VS_Output
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};

float4 PSMain(VS_Output input) : SV_TARGET
{
	float3 albedo = tex.Sample(samplerLinear, input.texCoord).rgb;
	return float4(albedo, 1.0f);
}