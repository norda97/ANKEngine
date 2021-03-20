struct PixelOutput
{
	float4 albedo	: SV_Target0;
};

struct VS_Output
{
	float4 worldPos	: SV_POSITION;
	float4 localPos	: POSITION;
};

Texture2D equirectangularTex	: register(t0);
SamplerState samplerr			: register(s0);

static const float2 invAtan = float2(0.1591, 0.3183);
float2 sampleSphericalMap(float3 pos)
{
	float2 uv = float2(atan2(pos.z, pos.x), asin(pos.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

PixelOutput PSMain(VS_Output input) : SV_TARGET
{
	PixelOutput output;

	float2 uv = sampleSphericalMap(normalize(input.localPos.xyz));
	float3 color = equirectangularTex.Sample(samplerr, uv).rgb;

	// Gamma correction
	color /= (color + float3(1.0f, 1.0f, 1.0f));
	const float div = 1.0f / 2.2f;
	color = pow(color, float3(div, div, div));

	output.albedo = float4(color, 1.0f);

	return output;
}