//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Floor.hlsli"
#include "Lighting.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput {
	float4 color : SV_TARGET0;
};

//-----------------------------------------------------------------------------------------
// TextureBuffers
//-----------------------------------------------------------------------------------------
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// Buffers
////////////////////////////////////////////////////////////////////////////////////////////
struct Material {
	float4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// textureSampler
	output.color = gTexture.Sample(gSampler, input.texcoord);
	output.color *= gMaterial.color;
	
	if (output.color.a == 0.0f) { //!< 透明度0の場合はpixel破棄
		discard;
	}
	
	return output;
	
}