static const float PI = 3.14159265f;

Texture2D albedoTex			: register(t0);
Texture2D worldTex			: register(t1);
Texture2D normalTex			: register(t2);
Texture2D MRAOTex			: register(t3); // Metallic, Roughness and AO

TextureCube irradianceTex	: register(t4); // Irradiance Map
TextureCube prefilterTex	: register(t5); // Prefilter Map
Texture2D	brdfLUT			: register(t6); // BRDF Look up table

SamplerState samplerLinear	: register(s0);
SamplerState samplerPoint	: register(s1);

cbuffer SceneVariables : register(b0)
{
	float3 cameraPos;
};

cbuffer LightVariables : register(b1)
{
	float4 lightPosInt;
};

struct VS_Output
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};

float3 fresnelSchlick(float cosTheta, float3 F0);
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);

float4 PSMain(VS_Output input) : SV_TARGET
{
	float3 worldPos = worldTex.Sample(samplerLinear, input.texCoord).xyz;
	float3 albedo = albedoTex.Sample(samplerLinear, input.texCoord).rgb;
	float3 MRAO = MRAOTex.Sample(samplerLinear, input.texCoord).xyz;
	float metallic = MRAO.x;
	float roughness = MRAO.y;
	float ao = MRAO.z;

	float3 N = normalize(normalTex.Sample(samplerLinear, input.texCoord).xyz);
	float3 V = normalize(cameraPos - worldPos);

	float3 Lo = float3(0.0, 0.0, 0.0);
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);

	// Loop over lights here
	//{
		// Lightning calcs
		float3 L = normalize(lightPosInt.xyz - worldPos);
		float3 H = normalize(V + L);

		float distance = length(lightPosInt.xyz - worldPos);
		float attenuation = 1.0f / (distance * distance);
		float3 lightColor = float3(1.0, 1.0, 1.0) * lightPosInt.w;
		float3 radiance = lightColor * attenuation;

		float3 F = fresnelSchlick(max(dot(V, H), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		// BRDF - Bidirectional reflective distribution function
		float3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		float3 specular = numerator / max(denominator, 0.001);

		float NdotL = max(dot(N, L), 0.0);

		//float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;
	//}

	float3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	// Nullify if metallic surface, due to zero refraction on metallic surfaces
	kD *= 1.0 - metallic;

	Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	float3 irradiance = irradianceTex.Sample(samplerLinear, N).rgb;
	float3 diffuse = irradiance * albedo;

	// IBL - Reflection
	const float MAX_REFLECTION_LOD = 4.0;
	float3 prefilteredColor = prefilterTex.SampleLevel(samplerLinear, N, roughness * MAX_REFLECTION_LOD).rgb;
	float2 envBRDF = brdfLUT.Sample(samplerPoint, float2(max(dot(N,V), 0.0), roughness)).rg;
	specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);

	float3 ambient = (kD * diffuse + specular) * ao;

	float3 color = ambient + Lo;

	// Gamma correction
	color = color / (color + float3(1.0, 1.0, 1.0));
	const float gammaFactor = 1.0 / 2.2;
	color = pow(color, float3(gammaFactor, gammaFactor, gammaFactor));

	float4 output = float4(color, 1.0f);

	return output;
}

// All credits for the functions below go to https://learnopengl.com/PBR/Lighting and their respective inventors
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	float val = 1.0f - max(roughness, 0.025);
	// F0 - Base reflectivity (IOR)
	// 

	return F0 + (max(float3(val, val, val), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// All credits for the functions below go to https://learnopengl.com/PBR/Lighting and their respective inventors
float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}