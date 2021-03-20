struct VertexData
{
	float4 pos	: POSITION;
};

cbuffer SceneVariables : register(b0)
{
	float4x4 vp;
};

struct VS_Output
{
	float4 worldPos	: SV_POSITION;
	float4 localPos	: POSITION;
};

VS_Output VSMain(VertexData input)
{
	VS_Output output;

	output.worldPos = mul(vp, normalize(input.pos)).xyww;
	output.localPos = input.pos;

	return output;
}