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

//-----------------------------------------------------------------------------------------
// TextureBuffers
//-----------------------------------------------------------------------------------------
Texture2D<float4> gTexture : register(t5);
SamplerState gSampler : register(s0);

//=========================================================================================
// ConstantBuffers
//=========================================================================================
ConstantBuffer<Material> gMaterial : register(b4);
ConstantBuffer<Light> gLight       : register(b5);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// textureSampler
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	output.color = gMaterial.color * textureColor;
	
	if (output.color.a == 0.0f) { //!< 透明度0の場合はpixel破棄
		discard;
	}
	
	Material material = gMaterial;
	Light    light    = gLight;
	Camera   camera   = gCamera;
	
	output.color.rgb = ComputeLightColor(output.color, input.normal, input.worldPos.xyz, material, light, camera.position.xyz);
	
	if (gMaterial.isDisplayMeshlet) {
		output.color.rgb = input.color.rgb;
	}
	
	return output;
}