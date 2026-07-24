//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryMesh.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(MeshVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position = mul(gTransform.Transform(input.position), kViewProj);
	output.worldPos = gTransform.Transform(input.position).xyz;
	
	output.texcoord = gMaterials[instanceId].transformation.Transform(input.texcoord);
	
	output.normal    = normalize(gTransform.TransformNormal(input.normal));
	output.tangent   = normalize(gTransform.TransformNormal(input.tangent));
	output.bitangent = normalize(gTransform.TransformNormal(input.bitangent));

	output.instanceId = instanceId;
	output.clip       = output.position.z; //!< near分のclip
	
	return output;
}
