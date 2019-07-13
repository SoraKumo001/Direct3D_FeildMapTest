#include "default.hlsli"
PS_INPUT main(VS_VECTOR i)
{
	PS_INPUT output = (PS_INPUT)0;


	//���S�ʒu�̊m��
	output.Pos = mul(float4(0, 0, 0, 1), World);

	//�^�����̉�
	output.Pos = computeSphere(output.Pos, View);
	output.Pos.xyz += float3(i.Pos.x, -i.Pos.y, i.Pos.z);
	output.Pos = mul(round(output.Pos), Projection);
	output.Col = i.Col;

	//���ꂽ��t�F�[�h
	float PDF = 2000;
	if (output.Pos.z > PDF)
		output.Col.a *= (1.0 - (output.Pos.z - PDF) / PDF * 2.0);
	//output.Pos.z -= 5.0;
	return output;
}