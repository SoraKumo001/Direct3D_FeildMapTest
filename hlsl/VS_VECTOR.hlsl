#include "default.hlsli"

PS_INPUT main( VS_VECTOR i )
{
	PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( i.Pos, World );
    output.Pos = mul( output.Pos, ViewProj );

    float3 Nor = normalize(mul( i.Nor, (float3x3)World ));
    output.Col = i.Col;
	return output;
}