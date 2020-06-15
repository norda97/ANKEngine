
struct PS_Input
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 PSMain(PS_Input input) : SV_TARGET
{
	return input.color;
}