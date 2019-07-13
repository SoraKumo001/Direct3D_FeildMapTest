#include "default.hlsli"

struct VS_SKIN_INPUT
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD;
	float BlendWeights[3] : BLENDWEIGHT;
	float BlendIndices[4] : BLENDINDICES;
};

PS_INPUT main( VS_SKIN_INPUT i )
{
	PS_INPUT output = (PS_INPUT)0;
	float4 Pos;
	float3 Nor;

	//スキンウエイトによると湯点計算
	float LastWeight = 1.0f - i.BlendWeights[0] - i.BlendWeights[1] - i.BlendWeights[2];
	Pos = mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[0]]) * i.BlendWeights[0];
	Nor = mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[0]]) * i.BlendWeights[0];
	Pos += mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[1]]) * i.BlendWeights[1];
	Nor += mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[1]]) * i.BlendWeights[1];
	Pos += mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[2]]) * i.BlendWeights[2];
	Nor += mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[2]]) * i.BlendWeights[2];
	Pos += mul(i.Pos, WorldMatrixArray[(int)i.BlendIndices[3]]) * LastWeight;
	Nor += mul(i.Nor, (float3x3)WorldMatrixArray[(int)i.BlendIndices[3]]) * LastWeight;
	Nor = normalize(Nor);

	//疑似球体化
	float4 sPos = mul(Pos,View);
	sPos *= sPos;
	Pos.z -= (sPos.x + sPos.y+sPos.z)/100000;
	//頂点確定
	output.Pos = mul( Pos, ViewProj );
	//ライティング計算
	float Light = dot(Nor,normalize(-LightDir));
	output.Tex = i.Tex;
    output.Col.xyz = i.Col.xyz * (Light*LightPri+(1.0-LightPri));
	output.Col.w = i.Col.w;
	return output;
}
