//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MonoBehavior.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
Texture2D<float4> gDiffuse : register(t0);
SamplerState gSampler      : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	// diffuseの取得
	float4 diffuse = gDiffuse.Sample(gSampler, input.texcoord);

	// lightingの計算
	float3 toCamera = normalize(gCamera.position.xyz - input.worldPos);
	
	float NdotC = dot(input.normal, toCamera);
	float factor = pow(NdotC * 0.5f + 0.5f, 2.0f);
	
	output.color.rgb = diffuse.rgb * factor;
	output.color.a   = diffuse.a;
	
	CheckDiscard(output.color);
	return output;
	
}