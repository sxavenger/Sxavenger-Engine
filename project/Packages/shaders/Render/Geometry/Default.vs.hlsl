//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

StructuredBuffer<TransformComponent> gTransforms : register(t0);
//ConstantBuffer<Transform2dComponent> gTransform2d : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(GeometryVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position = mul(gTransforms[instanceId].Transform(input.position), kViewProj);
	output.worldPos = gTransforms[instanceId].Transform(input.position).xyz;
	
	//output.texcoord  = gTransform2d.Transform(input.texcoord);
	output.texcoord = input.texcoord; //!< This is a temporary fix for the texcoord transformation.
	
	output.normal    = normalize(gTransforms[instanceId].TransformNormal(input.normal));
	output.tangent   = gTransforms[instanceId].TransformNormal(input.tangent);
	output.bitangent = gTransforms[instanceId].TransformNormal(input.bitangent);

	output.instanceId = instanceId;
	
	return output;
}
