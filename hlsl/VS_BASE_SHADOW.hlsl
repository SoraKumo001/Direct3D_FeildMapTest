#include "default.hlsli"

float4 main( VS_SHADOW i ) :SV_POSITION
{
	float4 Pos;
    Pos = mul( i.Pos, World );

	float3 sPos = (float3)mul(Pos,View);
	sPos.xyz *= sPos.xyz;
	Pos.z -= (sPos.x + sPos.y+sPos.z)/100000;

    float3 Nor = normalize(mul( i.Nor, (float3x3)World ));
	float LN = dot(Nor,LightDir);
	float scale = (LN<0.0)?ShadowLong[0]:ShadowLong[1];
	Pos.xyz = Pos.xyz + normalize(LightDir)*scale;
    
    Pos = mul( Pos, ViewProj );

	return Pos;
}

