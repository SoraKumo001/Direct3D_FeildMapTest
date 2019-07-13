#include "default.hlsli"

PS_INPUT main( VS_INPUT i )
{
	PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( i.Pos, World );
    float3 Nor = normalize(mul( i.Nor, (float3x3)World ));

	//�^�����̉�
	float3 sPos = (float3)mul(output.Pos,View);
	sPos.xyz *= sPos.xyz;
	output.Pos.z -= (sPos.x + sPos.y+sPos.z)/100000;
	//���_�m��
    output.Pos = mul( output.Pos, ViewProj );
	//���C�e�B���O�v�Z
	float3 light = dot(Nor,normalize(-LightDir));
	light = light*LightPri+(1.0-LightPri);
	output.Tex = i.Tex;
    output.Col= 1.0;
	
	return output;
}
