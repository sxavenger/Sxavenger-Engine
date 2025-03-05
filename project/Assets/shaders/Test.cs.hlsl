//=========================================================================================
// buffers
//=========================================================================================

struct Data {
	float4 color;
};
ConstantBuffer<Data> gData : register(b0);

RWTexture2D<float4> gOutput : register(u0);

uint2 GetDimensions() {
	uint2 size;
	gOutput.GetDimensions(size.x, size.y);
	return size;
}
static const uint2 kSize = GetDimensions();

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	if (any(dispatchThreadId.xy >= kSize)) {
		return;
	}

	uint idx = 0;

	for (int i = 0; i < 1000000; ++i) {
		// 時間がかかる処理
		idx = idx * 2 + 1;
	}

	gOutput[dispatchThreadId.xy] = gData.color * idx;
	
}
