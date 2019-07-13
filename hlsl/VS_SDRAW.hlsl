#include "default.hlsli"

PS_INPUT_SDRAW main(float4 pos:POSITION )
{
	PS_INPUT_SDRAW o = (PS_INPUT_SDRAW)0;
	o.Pos = pos;
	o.Tex = (pos.xy+1.0)*0.5;
	o.Tex.y = -o.Tex.y;
	return o;
}

