#include "default.hlsli"

struct VS_INPUT_FIELD
{
    float4  Pos            : POSITION;
    float3  Nor            : NORMAL;
    float3  Tan            : TANGENT;
    float3  Bin            : BINORMAL;
    float2  Tex            : TEXCOORD;
    float2  Tex2           : TEXCOORD_;
};

struct PS_INPUT_FIELD
{
    float4 Pos			: SV_POSITION;
    float4 Col			: COLOR;
    float2 Tex			: TEXCOORD;
    float2 Tex2			: TEXCOORD2;
    float3 Light		    : LIGHT;
};



PS_INPUT_FIELD main( VS_INPUT_FIELD input )
{
	PS_INPUT_FIELD output = (PS_INPUT_FIELD)0;
    output.Pos = mul( input.Pos, World );
    
	float3 sPos = (float3)mul(output.Pos,View);
	sPos.xyz *= sPos.xyz;
	output.Pos.z -= (sPos.x + sPos.y+sPos.z)/100000;
    
    output.Pos = mul( output.Pos, ViewProj );
    //output.Pos = mul( output.Pos, Projection );
	output.Tex = input.Tex;
	output.Tex2 = input.Tex2;
    
    output.Light = mul( -LightDir, InvTangentMatrix( input.Tan, input.Bin, input.Nor ) );
	return output;
}
