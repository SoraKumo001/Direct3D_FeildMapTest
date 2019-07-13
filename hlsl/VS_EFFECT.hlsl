#include "default.hlsli"


RasterizerState renderAll
{
	CullMode = NONE;
};

struct VS_EFFECT
{
	float4 Pos : POSITION;
	float Size : SIZE;
	float Rot : ROTATION;
	float2 Tex : TEXCOORD;
	float4 Col : COLOR;
};



PS_INPUT main(VS_EFFECT i)
{
	PS_INPUT output = (PS_INPUT)0;
	float2 ss = (i.Tex.xy - 0.5)*i.Size;

	output.Pos = mul(i.Pos, World);
	//‹^Ž—‹…‘Ì‰»
	output.Pos = computeSphere(output.Pos, View);

	float sin0 = sin(i.Rot);
	float cos0 = cos(i.Rot);

	output.Pos.xy += float2(ss.x*cos0-ss.y*sin0,-(ss.x*sin0+ss.y*cos0));
	output.Pos = mul(output.Pos, Projection);
	output.Col = i.Col;
	output.Tex = i.Tex;
	return output;
}
