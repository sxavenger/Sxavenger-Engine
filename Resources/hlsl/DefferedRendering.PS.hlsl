//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefferedRendering.hlsli"

//=========================================================================================
// Output
//=========================================================================================
struct DefferedPSOutput {
	float4 color : SV_Target0;
};

//=========================================================================================
// Buffer
//=========================================================================================
/* deffered Textures */
Texture2D<float4> gAlbedo   : register(t0);
Texture2D<float4> gDepth    : register(t1);
Texture2D<float4> gNormal   : register(t2);
Texture2D<float4> gPosition : register(t3);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
DefferedPSOutput main(DefferedVSOutput input) {
	
	DefferedPSOutput output;
	
	float4 albed = Deffered::GetTextureColor(input.texcoord, gAlbedo, gSampler);
	
	if (all(albed == 0.0f)) { //!< albedColorが0.0fの場合
		discard;
	}
	
	output.color = albed;
	
	return output;
}