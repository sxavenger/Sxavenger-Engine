//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

StructuredBuffer<TransformComponent> gTransform : register(t0);
ConstantBuffer<Transform2dComponent> gTransform2d : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(GeometryVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position  = mul(gTransform[instanceId].Transform(input.position), kViewProj);
	output.texcoord  = gTransform2d.Transform(input.texcoord);
	output.normal    = normalize(gTransform[instanceId].TransformNormal(input.normal));
	output.worldPos  = gTransform[instanceId].Transform(input.position).xyz;
	output.tangent   = gTransform[instanceId].TransformNormal(input.tangent);
	output.bitangent = gTransform[instanceId].TransformNormal(input.bitangent);
	
	return output;
}
