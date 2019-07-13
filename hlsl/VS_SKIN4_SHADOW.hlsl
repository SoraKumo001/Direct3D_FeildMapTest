#include "default.hlsli"

struct VS_SKIN_INPUT
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
	float BlendWeights[3] : BLENDWEIGHT;
	float BlendIndices[4] : BLENDINDICES;
};

float4 main( VS_SKIN_INPUT i ) : SV_POSITION
{
	float4 Pos;
	float3 Nor;

	float LastWeight = 1.0f - i.BlendWeights[0] - i.BlendWeights[1] - i.BlendWeights[2];
	Pos = mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[0]]) * i.BlendWeights[0];
	Nor  = mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[0]]) * i.BlendWeights[0];
	Pos += mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[1]]) * i.BlendWeights[1];
	Nor += mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[1]]) * i.BlendWeights[1];
	Pos += mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[2]]) * i.BlendWeights[2];
	Nor += mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[2]]) * i.BlendWeights[2];
	Pos += mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[3]]) * LastWeight;
	Nor += mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[3]]) * LastWeight;



	float LN = dot(Nor,LightDir);
	float scale = (LN<0.0f)?ShadowLong[0]:ShadowLong[1];
	Pos.xyz += normalize(LightDir)*scale;

	float3 sPos = (float3)mul(Pos,View);
	sPos.xyz *= sPos.xyz;
	Pos.z -= (sPos.x + sPos.y+sPos.z)/100000;


	Pos = mul( Pos, ViewProj );

	return Pos;
}


