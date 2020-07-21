struct VertexData
{
	float4 pos						: POSITION;
	float4 normal					: NORMAL;
	float4 tangent					: TANGENT;
	float2 texCoord					: TEXCOORD;

	float4x4 world					: INSTANCE_WORLD;

	uint id							: SV_InstanceID;
};

cbuffer SceneVariables : register(b0)
{
	float4x4 vp;
};

struct VS_Output
{
	float4 pos				: SV_POSITION;
	float4 worldPos			: WORLD_POSITON;
	float3x3 TBN			: TBN_MATRIX;
	float2 texCoord			: TEXCOORD;
};

VS_Output VSMain(VertexData input)
{
	VS_Output output;
	output.worldPos = mul(input.world, input.pos);
	output.texCoord = input.texCoord;

	output.pos = mul(vp, output.worldPos);

	float3 T = normalize(mul(input.world, float4(input.tangent.xyz, 0.0f)).xyz);
	float3 N = normalize(mul(input.world, float4(input.normal.xyz, 0.0f)).xyz);
	float3 B = normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	output.TBN = TBN;

	return output;
}