
struct PS_Input
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 PSMain(PS_Input input) : SV_TARGET
{
	return float4(1.0, 1.0, 1.0, 1.0);
}