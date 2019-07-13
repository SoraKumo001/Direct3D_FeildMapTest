#include "default.hlsli"

static const float4 MaterialDiffuse : MATERIALDIFFUSE = {0.8f, 0.8f, 0.8f, 1.0f};
static const float4 MaterialAmbient  = {0.2f, 0.2f, 0.2f, 1.0f};

struct PS_INPUT_FIELD
{
    float4 Pos			: SV_POSITION;
    float4 Col			: COLOR;
    float2 Tex			: TEXCOORD;
    float2 Tex2			: TEXCOORD2;
    float3 Light		    : LIGHT;
};
SamplerState wrapLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
struct PS_INPUT2
{
    float4 Pos			: SV_POSITION;
    float  Distance		: DISTANCE;
    float4 Col			: COLOR;
    float2 Tex			: TEXCOORD;
    float2 Tex2			: TEXCOORD2;
    float3 Light		: LIGHT;
};


float4 main( PS_INPUT2 i ) : SV_Target
{
	float a = 0.1+i.Distance/7000000.0;
	if(a > 1.0)
		a = 1.0;

	float4 color = texture0.Sample(wrapLinear,i.Tex);
	float3 color2 = (float3)texture1.Sample(wrapLinear,i.Tex2);

	float3 vect = color2 * 2.0 - 1.0;
	vect = normalize(vect);
	
	float3 bright = dot(i.Light,vect);
	bright = bright*LightPri+(1.0-LightPri);
	float4 c = color * float4(bright,a);
	return c;
}


