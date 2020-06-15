
struct InstanceVariables
{
	float3 pos						: POSITION;
	float4x4 transform				: INSTANCEPOS;
	float4 color					: COLOR;
};

struct VS_Output
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
};

cbuffer SceneVariables : register(b0)
{
	float4x4 VP;
	float time;
};

VS_Output VSMain(InstanceVariables input)
{
	VS_Output output;

	output.pos = mul(VP, mul(input.transform, float4(input.pos, 1.0f)));
	output.color = input.color;

	return output;
}