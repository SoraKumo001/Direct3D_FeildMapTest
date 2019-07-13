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



float4 main( PS_INPUT_FIELD i ) : SV_Target
{
	float4 color = texture0.Sample(wrapLinear,i.Tex);
	float3 color2 = (float3)texture1.Sample(wrapLinear,i.Tex2);

	float3 vect = color2 * 2.0 - 1.0;
	vect = normalize(vect);
	
	float3 bright = dot(i.Light,vect);
	bright = bright*LightPri+(1.0-LightPri);
	float4 c = color * float4(bright,1.0);

	return c;
}
