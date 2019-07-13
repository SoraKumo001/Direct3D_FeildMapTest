#include "default.hlsli"


float4 main( PS_BUMP i ) : SV_Target
{
	float4 color = texture0.Sample(samLinear,i.Tex);
	float3 color2 = (float3)texture1.Sample(samLinear,i.Tex);

	float3 vect = color2 * 2.0 - 1.0;
	vect = normalize(vect);
	
	float3 bright = 1.0-dot(i.Light,vect);
	bright = 1.0 - bright*bright;
	bright = bright*LightPri+(1.0-LightPri);
	float4 c = i.Col * color * float4(bright,0.99);
	//if(i.z < 0)
	//	c.w = i.Pos.z;//(1.0-(PDN-output.Pos.z)/PDN*0.7);
	return c;
}


