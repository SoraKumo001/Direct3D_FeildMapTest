static const float3 LightDir = {0.1,-0.5,-0.5};
static const float2 ShadowLong = {30.0,1000.0};
static const int MAX_MATRICES = 24;
static const float LightPri = 0.5;
static const float PDN = 500.0;
static const float PDF = 5000.0;

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINER;
	AddressU = Wrap;
	AddressV = Wrap;
};
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD;
};
struct VS_VECTOR
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR;
};
struct VS_SHADOW
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
};
struct VS_TEXTURE
{
	float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_SHADOW
{
	float4 Pos : SV_POSITION;
};
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT_SDRAW
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

struct PS_BUMP
{
    float4 Pos		: SV_POSITION;
    float2 Tex		: TEXCOORD;
    float3 Light		: LIGHT;
	float4 Col		: COLOR;
};

cbuffer World
{
	matrix World;
};

cbuffer Color
{
	float4 Color;
};
cbuffer View
{
	matrix View;
};
cbuffer Projection
{
	matrix Projection;
};

cbuffer ViewProj
{
	matrix ViewProj;
};

cbuffer ConstantWorldBone
{
	matrix WorldMatrixArray[MAX_MATRICES];
};
float3x3 InvTangentMatrix(float3 tangent,float3 binormal,float3 normal )
{
	float3x3 mat = 
	{
		tangent ,
		binormal,
		normal  
	};
   return transpose( mat );   // “]’u
}

float4 computeSphere(float4 pos, matrix view)
{
	float4 wPos = mul(pos, view);
	wPos *= wPos;
	pos.z -= (wPos.x + wPos.y + wPos.z) / 100000.0;
	pos = mul(pos, View);
	return pos;
}