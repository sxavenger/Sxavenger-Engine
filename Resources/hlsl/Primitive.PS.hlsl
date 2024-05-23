//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Primitive.hlsli"
#include "Lighting.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput {
	float4 color : SV_TARGET0;
};

//=========================================================================================
// Buffers
//=========================================================================================
struct Material {
	float4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLigth {
	float4 color;
	float3 direction;
	float intensity;
};
ConstantBuffer<DirectionalLigth> gLight : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	output.color = gMaterial.color;
	output.color.rgb *= HalfLambertReflection(input.normal, gLight.direction);
	
	return output;
	
}
