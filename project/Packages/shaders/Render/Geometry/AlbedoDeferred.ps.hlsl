//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

StructuredBuffer<MaterialComponent> gMaterials : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryDeferredOutput main(GeometryPSInput input) {
	
	GeometryDeferredOutput output = (GeometryDeferredOutput)0;

	float3x3 tbn = float3x3(input.tangent, input.bitangent, input.normal);

	Material::TextureParameter parameter;
	parameter.Set(input.texcoord, gSampler);

	output.SetAlbedo(gMaterials[input.instanceId].albedo.GetAlbedo(parameter));
	output.SetNormal(gMaterials[input.instanceId].normal.GetNormal(input.normal, parameter, tbn));
	output.SetPosition(input.worldPos);
	
	output.SetMaterial(
		gMaterials[input.instanceId].properties.metallic.GetValue(parameter),
		gMaterials[input.instanceId].properties.specular.GetValue(parameter),
		gMaterials[input.instanceId].properties.roughness.GetValue(parameter)
	);

	return output;
}
