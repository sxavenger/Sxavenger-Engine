//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gTexture : register(u0);

struct Parameter {
	float4 color;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint width, height;
	gTexture.GetDimensions(width, height);

	if (any(dispatchThreadId.xy >= uint2(width, height))) {
		return;
	}

	gTexture[dispatchThreadId.xy] = gParameter.color;
	
}
