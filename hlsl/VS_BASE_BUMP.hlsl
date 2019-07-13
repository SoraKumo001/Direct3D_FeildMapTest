#include "default.hlsli"

struct VS_BUMP
{
    float4  Pos            : POSITION;
    float3  Nor            : NORMAL;
    float2  Tex            : TEXCOORD;
	float3  Tan            : TANGENT;
    float3  Bin            : BINORMAL;

};

PS_BUMP main( VS_BUMP i )
{
	PS_BUMP output = (PS_BUMP)0;
    output.Pos = mul( i.Pos, World );

	//‹^Ž—‹…‘Ì‰»
	float3 sPos = (float3)mul(output.Pos,View);
	sPos.xyz *= sPos.xyz;
	output.Pos.z -= (sPos.x + sPos.y+sPos.z)/100000;
	//’¸“_Šm’è
    output.Pos = mul( output.Pos, ViewProj );

    float3 Nor = normalize(mul( i.Nor, (float3x3)World ));
	output.Tex = i.Tex;
    output.Light = (float3)mul( -LightDir, InvTangentMatrix( i.Tan, i.Bin, Nor ) );

	float a;
	if(output.Pos.z < PDN)
		a = (1.0-(PDN-output.Pos.z)/PDN*0.7);
	else if(output.Pos.z > PDF)
		a = (1.0-(output.Pos.z-PDF)/PDF*2.0);
	output.Col = float4(1,1,1,a);
	return output;
}
