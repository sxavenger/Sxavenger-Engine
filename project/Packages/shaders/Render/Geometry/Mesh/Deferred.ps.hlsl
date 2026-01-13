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

	float3x3 tbn = float3x3(input.tangent, input.bitangent, input.normal);

	MaterialLib::TextureSampler parameter;
	parameter.Set(input.texcoord, gSampler);

	float transparency = gMaterials[input.instanceId].transparency.GetTransparency(parameter);

	clip(transparency - 0.1f); //!< 半透明描画を破棄 [transparency <= 0.1]

	output.SetAlbedo(gMaterials[input.instanceId].albedo.GetAlbedo(parameter));
	output.SetNormal(gMaterials[input.instanceId].normal.GetNormal(input.normal, parameter, tbn));
	output.SetPosition(input.worldPos);

	output.SetMaterial(
		0.0f,
		gMaterials[input.instanceId].properties.roughness.GetValue(parameter, 1),
		gMaterials[input.instanceId].properties.metallic.GetValue(parameter, 2)
	);

	return output;
}

