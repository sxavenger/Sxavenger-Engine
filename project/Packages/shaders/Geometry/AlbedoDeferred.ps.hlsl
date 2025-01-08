//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

// todo: material

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryDeferredOutput main(GeometryPSInput input) {
	
	GeometryDeferredOutput output = (GeometryDeferredOutput)0;

	output.SetAlbedo(gAlbedo.Sample(gSampler, input.texcoord).rgb);
	output.SetNormal(input.normal);
	output.SetPosition(input.worldPos);
	
	return output;
}
