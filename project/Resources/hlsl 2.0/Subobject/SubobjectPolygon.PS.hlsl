//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Subobject.hlsli"
#include "SubobjectMesh.hlsli"
#include "SubobjectLighting.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput {
	float4 color : SV_TARGET0;
};

//=========================================================================================
// Material structure buffer
//=========================================================================================
ConstantBuffer<Material> gMaterial : register(b4);
ConstantBuffer<Light> gLight       : register(b5);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	output.color = gMaterial.color;
	
	if (output.color.a == 0.0f) { //!< 透明度0の場合はpixel破棄
		discard;
	}
	
	Material material = gMaterial;
	Light light = gLight;
	Camera camera = gCamera;
	
	output.color.rgb = ComputeLightColor(output.color, input.normal, input.worldPos.xyz, material, light, camera.position.xyz);
	
	if (gMaterial.isDisplayMeshlet) {
		output.color = input.color;
	}
	
	return output;
}