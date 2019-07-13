#include "default.hlsli"
cbuffer ConstantBuffer3
{
	float4 Scale;
}
PS_INPUT main(VS_TEXTURE i)
{
	PS_INPUT output = (PS_INPUT)0;

	//���S�ʒu�̊m��
	output.Pos = mul(float4(0, 0, 0, 1), World);
	output.Pos = computeSphere(output.Pos, View);

	output.Pos.xyz += float3(i.Pos.x, -i.Pos.y, i.Pos.z);
	output.Pos = mul(output.Pos, Projection);
	output.Col = 1.0;
	output.Tex = i.Tex*Scale.xy + Scale.zw;

	//���ꂽ��t�F�[�h
	float PDF = 1500;
	if (output.Pos.z > PDF)
		output.Col.a *= (1.0 - (output.Pos.z - PDF) / PDF * 0.5);


	return output;
}