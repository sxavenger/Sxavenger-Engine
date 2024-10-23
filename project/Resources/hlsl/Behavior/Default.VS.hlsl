//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Behavior.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

StructuredBuffer<TransformationMatrix> gTransform : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(VertexInput input, uint instanceId : SV_InstanceID) {
	
	PSInput output = (PSInput)0;
	
	output.position = mul(input.position, mul(gTransform[instanceId].world, viewProj));
	output.texcoord = input.texcoord;
	output.normal   = normalize(mul(input.normal, (float3x3)gTransform[instanceId].worldInverseTranspose));
	output.worldPos = mul(input.position, gTransform[instanceId].world).xyz;
	
	return output;
}


