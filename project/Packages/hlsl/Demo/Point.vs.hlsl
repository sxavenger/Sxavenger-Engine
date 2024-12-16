//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Shape.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

ConstantBuffer<UVTransformationMatrix> gUVTransform : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VertexGSInput main(VertexInput input, uint instanceId : SV_InstanceID) {

	VertexGSInput output = (VertexGSInput)0;

	output.position  = input.position;
	output.normal    = input.normal;
	output.texcoord  = gUVTransform.Transform(input.texcoord);
	output.intanceId = instanceId;

	return output;

}