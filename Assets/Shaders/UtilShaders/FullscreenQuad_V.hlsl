struct VertexData
{
	float4 pos						: POSITION;
	float2 texCoord					: TEXCOORD;
};

struct VS_Output
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};

VS_Output VSMain(VertexData input)
{
	VS_Output output;

	output.pos = input.pos;
	output.texCoord = input.texCoord;

	return output;
}