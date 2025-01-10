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
	
	output.position  = mul(gTransform[instanceId].Transform(input.position), viewProj);
	output.texcoord  = gUVTransform.Transform(input.texcoord);
	output.normal    = gTransform[instanceId].TransformNormal(input.normal);
	output.worldPos  = gTransform[instanceId].Transform(input.position).xyz;
	output.tangent   = gTransform[instanceId].TransformNormal(input.tangent);
	output.bitangent = gTransform[instanceId].TransformNormal(input.bitangent);
	
	return output;
	
}


