//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

StructuredBuffer<TransformComponent> gTransforms : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(GeometryVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position = mul(gTransforms[instanceId].Transform(input.position), kViewProj);
	output.worldPos = gTransforms[instanceId].Transform(input.position).xyz;
	
	output.texcoord = gMaterials[instanceId].transform.Transformation(input.texcoord);
	
	output.normal    = normalize(gTransforms[instanceId].TransformNormal(input.normal));
	output.tangent   = gTransforms[instanceId].TransformNormal(input.tangent);
	output.bitangent = gTransforms[instanceId].TransformNormal(input.bitangent);

	output.instanceId = instanceId;
	
	return output;
}
