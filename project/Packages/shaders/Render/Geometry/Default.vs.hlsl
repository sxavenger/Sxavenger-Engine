//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

StructuredBuffer<TransformationMatrix> gTransform : register(t0);
// todo: UVTransform

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(GeometryVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position  = mul(gTransform[instanceId].Transform(input.position), kViewProj);
	output.texcoord  = input.texcoord; // todo: UVTransform.Transform(input.texcoord);
	output.normal    = normalize(gTransform[instanceId].TransformNormal(input.normal));
	output.worldPos  = gTransform[instanceId].Transform(input.position).xyz;
	output.tangent   = gTransform[instanceId].TransformNormal(input.tangent);
	output.bitangent = gTransform[instanceId].TransformNormal(input.bitangent);
	
	return output;
}
