//struct PixelOutput
//{
//	float4 albedo	: SV_Target0;
//};

struct VS_Output
{
	float4 worldPos	: SV_POSITION;
	float4 localPos	: POSITION;
};

TextureCube cubeTex				: register(t0);
SamplerState samplerr			: register(s0);

float4 PSMain(VS_Output input) : SV_TARGET
{
	//PixelOutput output;

	float3 color = cubeTex.Sample(samplerr, normalize(input.localPos)).rgb;

	//output.albedo = float4(color, 1.0f);

	return float4(color, 1.0f);
}