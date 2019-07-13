#include "default.hlsli"

float4 main( PS_INPUT i ) : SV_Target
{
	if( i.Col.a < 0.05 )
		discard;
	return i.Col;
}
