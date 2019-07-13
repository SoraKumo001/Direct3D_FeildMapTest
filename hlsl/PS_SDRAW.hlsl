#include "default.hlsli"



float4 main( PS_INPUT_SDRAW i ) : SV_Target
{
	float2 uv = i.Tex;
	float4 c = texture0.Sample(samLinear,uv);
	float f = 0.5/512.0;
	float4 c1 = texture0.Sample(samLinear,float2(uv.x-f,uv.y-f));
	float4 c2 = texture0.Sample(samLinear,float2(uv.x+f,uv.y-f));
	float4 c3 = texture0.Sample(samLinear,float2(uv.x+f,uv.y-f));
	float4 c4 = texture0.Sample(samLinear,float2(uv.x+f,uv.y+f));
	f *= 4.0;
	float4 c5 = texture0.Sample(samLinear,float2(uv.x-f,uv.y-f));
	float4 c6 = texture0.Sample(samLinear,float2(uv.x+f,uv.y-f));
	float4 c7 = texture0.Sample(samLinear,float2(uv.x+f,uv.y-f));
	float4 c8 = texture0.Sample(samLinear,float2(uv.x+f,uv.y+f));
	c.a = (c.a+(c1.a+c2.a+c3.a+c4.a)+c5.a+c6.a+c7.a+c8.a)/40;
	return c;
}
