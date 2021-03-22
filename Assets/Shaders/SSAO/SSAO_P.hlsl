
Texture2D       worldTex				: register(t0);
Texture2D       normalTex				: register(t1);
Texture2D       noiseTex				: register(t2);

SamplerState	samplerPointClamp		: register(s0);
SamplerState	samplerPointWrap		: register(s1);

cbuffer SSAOData : register(b0)
{
	float4x4    projection;
	uint        screenWidth;
	uint        screenHeight;
	uint        padding0;
	uint        padding1;
	float4      samples[64];
};

struct VS_Output
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};


float4 PSMain(VS_Output input) : SV_TARGET
{
	const float2 noiseScale = float2(screenWidth/4.0, screenHeight/4.0);

	float3 fragPos = worldTex.Sample(samplerPointClamp, input.texCoord).xyz;
	float3 normal = normalTex.Sample(samplerPointClamp, input.texCoord).xyz;
	float3 randomVec = noiseTex.Sample(samplerPointWrap, input.texCoord * noiseScale).xyz;
	
	// Calc TBN
	float3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = cross(normal, tangent);
	float3x3 TBN     = float3x3(tangent, bitangent, normal);

	float occlusion = 0.0;
	const uint kernelSize = 64;
	const float radius = 0.5;
	const float bias = 0.025;
	for(uint i = 0; i < kernelSize; ++i)
	{
		// get sample position
		float3 samplePos = mul(TBN, samples[i].xyz); // from tangent to view-space
		samplePos = fragPos + samplePos * radius; 
		
		float4 offset = float4(samplePos, 1.0);
		offset      = mul(projection, offset);    // from view to clip-space
		offset.xyz /= offset.w;               // perspective divide
		offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0 

		float sampleDepth = worldTex.Sample(samplerPointClamp, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;  
	} 

	occlusion = 1.0 - (occlusion / kernelSize);
	return float4(float3(occlusion, occlusion, occlusion), 1.0f); // TODO: This should be R16_F tex
}