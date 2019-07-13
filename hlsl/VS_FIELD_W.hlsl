#include "default.hlsli"
cbuffer ConstantBuffer3 : register(b3)
{
	float4 Noize;
}

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
struct PS_INPUT2
{
    float4 Pos		: SV_POSITION;
    float  Distance	: DISTANCE;
    float4 Col		: COLOR;
    float2 Tex		: TEXCOORD;
    float2 Tex2		: TEXCOORD2;
    float3 Light		: LIGHT;
};

PS_INPUT2 main( VS_INPUT_FIELD input )
{
	PS_INPUT2 output = (PS_INPUT2)0;
    output.Pos = mul( input.Pos, World );
	float4 sPos = mul(output.Pos,View);
	sPos.xyz *= sPos.xyz;
	output.Distance = (sPos.x+sPos.y+sPos.z);
	output.Pos.z -= output.Distance/100000;
    
    output.Pos = mul( output.Pos, ViewProj );
	output.Tex = input.Tex;
	output.Tex2 = input.Tex2+Noize.x;
    
    output.Light = mul( -LightDir, InvTangentMatrix( input.Tan, input.Bin, input.Nor ) );
	return output;
}


