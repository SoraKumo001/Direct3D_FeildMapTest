#include "default.hlsli"
PS_INPUT main(VS_VECTOR i)
{
	PS_INPUT output = (PS_INPUT)0;


	//中心位置の確定
	output.Pos = mul(float4(0, 0, 0, 1), World);

	//疑似球体化
	output.Pos = computeSphere(output.Pos, View);
	output.Pos.xyz += float3(i.Pos.x, -i.Pos.y, i.Pos.z);
	output.Pos = mul(round(output.Pos), Projection);
	output.Col = i.Col;

	//離れたらフェード
	float PDF = 2000;
	if (output.Pos.z > PDF)
		output.Col.a *= (1.0 - (output.Pos.z - PDF) / PDF * 2.0);
	//output.Pos.z -= 5.0;
	return output;
}