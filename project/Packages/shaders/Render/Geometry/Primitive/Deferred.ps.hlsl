//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPrimitive.hlsli"

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

	MaterialLib::TextureSampler parameter;
	parameter.Set(input.texcoord, gSampler);

	output.SetAlbedo(float3(1.0f, 1.0f, 1.0f));
	output.SetNormal(input.normal);
	output.SetPosition(input.worldPos);

	return output;
}
