static const float PI = 3.14159265f;

Texture2D worldTex		: register(t0);
Texture2D albedoTex		: register(t1);
Texture2D normalTex		: register(t2);

SamplerState samplerr	: register(s0);

cbuffer LightVariables : register(b0)
{
	float4 lightPosIntensity;
};

struct VS_Output
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};

float4 PSMain(VS_Output input) : SV_TARGET
{
	float3 position = worldTex.Sample(samplerr, input.texCoord).xyz;
	float3 albedo = albedoTex.Sample(samplerr, input.texCoord).xyz;
	float3 normal = normalize(normalTex.Sample(samplerr, input.texCoord).xyz);

	float3 distVec = lightPosIntensity.xyz - position;
	float distance = length(distVec);
	float3 lightDir = normalize(distVec);

	//float attenuation = 1.0f / (distance);
	float ambient = 0.4;
	float diffuse = max(dot(normal, lightDir), 0.0f);

	//float4 output = float4(albedo * (diffuse + ambient), 1.0f);
	float4 output = float4(albedo * (min(diffuse + ambient, 1.0)), 1.0f);

	return output;
}