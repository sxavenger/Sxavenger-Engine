//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryMesh.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
void main(GeometryPSInput input) {
	//!< Forward Opaque Depth Pre-pass.

	MaterialLib::TextureSampler parameter
		= MaterialLib::TextureSampler::Create(input.texcoord, gSampler);
	
	float transparency = gMaterials[input.instanceId].GetTransparency(parameter);

	clip(transparency - 1.0f); //!< 不透明描画を破棄 [transparency < 1.0]

	
}
