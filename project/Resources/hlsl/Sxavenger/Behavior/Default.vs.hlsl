//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Behavior.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

StructuredBuffer<TransformationMatrix> gTransform   : register(t0);
ConstantBuffer<UVTransformationMatrix> gUVTransform : register(b0); //!< 特になければconstantとして扱う.

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(VertexInput input, uint instanceId : SV_InstanceID) {
	
	PSInput output = (PSInput)0;
	
	output.position = mul(input.position, mul(gTransform[instanceId].mat, viewProj));
	output.texcoord = mul(float4(input.texcoord, 0.0f, 1.0f), gUVTransform.mat).xy;
	output.normal   = normalize(mul(input.normal, (float3x3)gTransform[instanceId].matInverseTransopse));
	output.worldPos = mul(input.position, gTransform[instanceId].mat).xyz;
	
	return output;
	
}


