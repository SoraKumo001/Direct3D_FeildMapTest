#include "default.hlsli"

float4 main( PS_INPUT i ) : SV_Target
{
	float4 c = texture0.Sample(samLinear,i.Tex) * i.Col * Color;
	if( c.a < 0.05 )
		discard;
	return c;
}
