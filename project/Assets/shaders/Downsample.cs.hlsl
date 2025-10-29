//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);
Texture2D<float4> gInput : register(t0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetDimension() {
	uint width, height;
	gOutput.GetDimensions(width, height);
	return uint2(width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index      = dispatchThreadId.xy;
	uint2 dimensions = GetDimension();

	if (any(index >= dimensions)) {
		return;
	}

	float2 uv = (float2)index / (float2)dimensions;

	gOutput[index] = gInput.SampleLevel(gSampler, uv, 0);
	
}
