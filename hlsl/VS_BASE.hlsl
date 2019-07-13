#include "default.hlsli"

PS_INPUT main( VS_INPUT i )
{
	PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( i.Pos, World );
    float3 Nor = normalize(mul( i.Nor, (float3x3)World ));

	//疑似球体化
	output.Pos = computeSphere(output.Pos, View);
    output.Pos = mul( output.Pos, Projection );
	//ライティング計算
	float3 light = dot(Nor,normalize(-LightDir));
	light = light*LightPri+(1.0-LightPri);
	output.Tex = i.Tex;
    output.Col.xyz = light*i.Col.xyz;
	output.Col.w = i.Col.w;
	

	if(output.Pos.z < PDN)
		output.Col.w *= (1.0-(PDN-output.Pos.z)/PDN*0.7);
	else if(output.Pos.z > PDF)
		output.Col.w *= (1.0-(output.Pos.z-PDF)/PDF*2.0);


	return output;
}
