//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPrimitive.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(PrimitiveVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position = mul(gTransforms[instanceId].Transform(input.position), kViewProj);
	output.worldPos = gTransforms[instanceId].Transform(input.position).xyz;
	
	output.texcoord = input.texcoord;
	
	output.normal    = normalize(gTransforms[instanceId].TransformNormal(input.normal));

	output.instanceId = instanceId;
	
	return output;
	
}
