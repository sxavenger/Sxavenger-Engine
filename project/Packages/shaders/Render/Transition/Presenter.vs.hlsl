//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Presenter.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kVertexCount = 3;

static const float4 kPositions[kVertexCount] = {
	float4(-1.0f, 1.0f, 0.0f, 1.0f),
	float4(3.0f, 1.0f, 0.0f, 1.0f),
	float4(-1.0f, -3.0f, 0.0f, 1.0f)
};

static const float2 kTexcoords[kVertexCount] = {
	float2(0.0f, 0.0f),
	float2(2.0f, 0.0f),
	float2(0.0f, 2.0f)
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(uint vertexId : SV_VertexID) {

	PSInput output = (PSInput)0;
	
	output.position = kPositions[vertexId];
	output.texcoord = kTexcoords[vertexId];
	
	return output;
}
