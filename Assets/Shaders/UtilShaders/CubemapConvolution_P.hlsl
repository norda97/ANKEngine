struct PixelOutput
{
	float4 albedo	: SV_Target0;
};

struct VS_Output
{
	float4 worldPos	: SV_POSITION;
	float4 localPos	: POSITION;
};

TextureCube cubeTex				: register(t0);
SamplerState samplerr			: register(s0);

static const float2 invAtan = float2(0.1591, 0.3183);
static const float PI = 3.14159265f;

PixelOutput PSMain(VS_Output input) : SV_TARGET
{
	PixelOutput output;

	float3 normal = normalize(input.localPos.xyz);
	float3 irradiance = float3(0.f, 0.f, 0.f);
	
	float3 up = float3(0.f, 1.f, 0.f);
	float3 right = cross(up, normal);
	up = cross(normal, right);

	float step = 0.025;
	float sampleCount = 0;

	for (float phi = 0; phi < 2.0f * PI; phi += step)
	{
		for (float theta = 0; theta < 0.5f * PI; theta += step)
		{
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			float3 contribution = cubeTex.Sample(samplerr, sampleVec).rgb;

			irradiance += contribution * cos(theta) * sin(theta);
			sampleCount++;
		}
	}

	irradiance = PI * irradiance * (1.0f / float(sampleCount));
	//color /= (color + float3(1.0f, 1.0f, 1.0f));
	//const float div = 1.0f / 2.2f;
	//color = pow(color, float3(div, div, div));

	output.albedo = float4(irradiance, 1.0f);

	return output;
}