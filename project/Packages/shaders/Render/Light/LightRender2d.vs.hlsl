//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

static const float4 kPositions[3] = {
	float4(-1.0f, 1.0f, 0.0f, 1.0f),
	float4(3.0f, 1.0f, 0.0f, 1.0f),
	float4(-1.0f, -3.0f, 0.0f, 1.0f)
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(uint vertexId : SV_VertexID, uint instanceId : SV_InstanceID) {

	PSInput output = (PSInput)0;
	
	output.position   = kPositions[vertexId];
	output.instanceId = instanceId;
	
	return output;
}
