
cbuffer sceneVariables: register(b0)
{
	float4x4 VP;
	float time;
};

float4 VSMain(float4 pos : POSITION) : SV_POSITION
{
	pos += float4(0.0, sin(time), 0.0, 0.0);

	return mul(VP, pos);
}

float4 PSMain() : SV_TARGET
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}