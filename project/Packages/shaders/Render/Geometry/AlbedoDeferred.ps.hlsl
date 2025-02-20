//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

ConstantBuffer<MaterialComponent> gMaterial : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryDeferredOutput main(GeometryPSInput input) {
	
	GeometryDeferredOutput output = (GeometryDeferredOutput)0;

	float3x3 tbn = float3x3(input.tangent, input.bitangent, input.normal);

	Material::TextureParameter parameter;
	parameter.Set(input.texcoord, gSampler);

	output.SetAlbedo(gMaterial.albedo.GetAlbedo(parameter));
	output.SetNormal(gMaterial.normal.GetNormal(input.normal, parameter, tbn));
	output.SetPosition(input.worldPos);
	
	output.SetMaterial(
		gMaterial.properties.metallic.GetValue(parameter),
		gMaterial.properties.specular.GetValue(parameter),
		gMaterial.properties.roughness.GetValue(parameter)
	);

	return output;
}
