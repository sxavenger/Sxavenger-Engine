//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryMesh.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryDeferredOutput main(GeometryPSInput input) {
	
	GeometryDeferredOutput output = (GeometryDeferredOutput)0;

	MaterialLib::TextureSampler parameter
		= MaterialLib::TextureSampler::Create(input.texcoord, gSampler);

	float transparency = gMaterials[input.instanceId].GetTransparency(parameter);

	clip(transparency - 0.1f); //!< 半透明描画を破棄 [transparency <= 0.1]

	output.SetAlbedo(gMaterials[input.instanceId].GetAlbedo(parameter));
	output.SetNormal(gMaterials[input.instanceId].GetNormal(input.normal, input.tangent, input.bitangent, parameter));

	output.SetMaterial(
		gMaterials[input.instanceId].GetRoughness(parameter),
		gMaterials[input.instanceId].GetMetallic(parameter)
	);

	output.SetAddress(AddressBuffer.GetSprit());

	return output;
}

