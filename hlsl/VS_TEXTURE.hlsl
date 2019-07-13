#include "default.hlsli"

PS_INPUT main( VS_TEXTURE i )
{
	PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( i.Pos, World );
    output.Pos = mul( output.Pos, ViewProj );
    output.Col = 1.0;
	output.Tex = i.Tex;
	return output;
}
