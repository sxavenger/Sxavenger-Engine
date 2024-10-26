//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DebugObject.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
struct Material {
	float4 color;
};
ConstantBuffer<Material> gMaterial : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	output.color = gMaterial.color;
	
	float3 direction = normalize(gCamera.position.xyz - input.worldPos);
	
	float NdotD = dot(input.normal, direction);
	NdotD = pow((NdotD + 1.0f) * 0.5f, 2.0f);
	
	output.color.rgb = output.color.rbg * NdotD;
	
	return output;
}