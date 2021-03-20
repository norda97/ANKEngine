struct PixelOutput
{
	float4 albedo	: SV_Target0;
	float4 position : SV_Target1;
	float4 normal	: SV_Target2;
	float4 MRAO		: SV_Target3; // Metallic, Roughness and AO
};

cbuffer materialProperties  : register(b0)
{
	float4 albedo;
	float roughness;
	float metallicness;
	float specular;
};

Texture2D albedoTex			: register(t0);
Texture2D metallicTex		: register(t1);
Texture2D roughnessTex		: register(t2);
Texture2D ambientTex		: register(t3);
Texture2D normalTex			: register(t4);
SamplerState samplerr		: register(s0);

struct VS_Output
{
	float4 pos			: SV_POSITION;
	float4 worldPos		: WORLD_POSITON;
	float3x3 TBN		: TBN_MATRIX;
	float2 texCoord		: TEXCOORD;
};

PixelOutput PSMain(VS_Output input)
{
	PixelOutput output;

	output.albedo = albedoTex.Sample(samplerr, input.texCoord) * albedo;

	const float gammaFactor = 2.2;
	output.albedo.rgb = pow(output.albedo.rgb, float3(gammaFactor, gammaFactor, gammaFactor));


	float r = roughnessTex.Sample(samplerr, input.texCoord).r * roughness;
	float m = metallicTex.Sample(samplerr, input.texCoord).r * metallicness;
	float ao = ambientTex.Sample(samplerr, input.texCoord).r;
	output.MRAO = float4(m, r, ao, 1.0);
	//output.MRAO.g = roughnessTex.Sample(samplerr, input.texCoord).g;
	//output.MRAO.b = ambientTex.Sample(samplerr, input.texCoord).b;
	output.position = input.worldPos;

	//float3 N = normalize(input.normal.xyz);
	//float3 T = normalize(input.tangent.xyz);
	//float3 B = normalize(cross(N,T));
	//float3x3 TBN = float3x3(T, B, N);

	float3 normalMap = normalTex.Sample(samplerr, input.texCoord).xyz;

	normalMap = normalize(normalMap * 2.0 - 1.0);

	output.normal = float4(mul(normalMap, input.TBN), 1.0);
	//output.normal = float4(normalMap, 1.0);

	return output;
}