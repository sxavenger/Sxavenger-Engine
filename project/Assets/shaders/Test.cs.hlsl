//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);

Texture2D<float4> gDummy : register(t0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetDimension() {
	uint2 size;
	gOutput.GetDimensions(size.x, size.y);
	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (any(index >= GetDimension())) {
		return;
	}

	float2 uv = float2(index) / float2(GetDimension());

	gOutput[index] = gDummy.SampleLevel(gSampler, uv, 0);
	
}
