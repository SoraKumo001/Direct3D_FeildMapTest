#include "default.hlsli"
cbuffer ConstantBuffer3
{
	float4 Scale;
}
PS_INPUT main( VS_TEXTURE i )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = i.Pos;
	output.Pos.xy *= Scale.xy;
    output.Pos = mul( output.Pos, World );
    output.Pos = mul( output.Pos, ViewProj );
    output.Col = 1.0;
	output.Tex = i.Tex*Scale.xy + Scale.zw;
	return output;
}
