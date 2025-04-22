//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../GeometryPass.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryDeferredOutput main(GeometryPSInput input) {
	
	GeometryDeferredOutput output = (GeometryDeferredOutput)0;

	float3x3 tbn = float3x3(input.tangent, input.bitangent, input.normal);

	MaterialLib::TextureSampler parameter;
	parameter.Set(input.texcoord, gSampler);

	float transparency = gMaterials[input.instanceId].transparency.GetTransparency(parameter);
	if (transparency <= 0.05f) {
		discard;
	}

	output.SetAlbedo(gMaterials[input.instanceId].albedo.GetAlbedo(parameter));
	output.SetNormal(gMaterials[input.instanceId].normal.GetNormal(input.normal, parameter, tbn));
	output.SetPosition(input.worldPos);

	output.SetMaterial( //!< ORM
		gMaterials[input.instanceId].properties.roughness.GetValue(parameter, 1),
		gMaterials[input.instanceId].properties.metallic.GetValue(parameter, 2),
		gMaterials[input.instanceId].properties.specular.GetValue(parameter)
	);

	return output;
}

