
//=========================================================================================
// buffers
//=========================================================================================

RWTexture2DArray<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint width, height, depth;
	gOutput.GetDimensions(width, height, depth);

	if (any(dispatchThreadId >= uint3(width, height, depth))) {
		return; //!< 範囲外は無視
	}

	float4 colors[6] = {
		float4(1.0, 0.0, 0.0, 1.0), // 赤
		float4(0.0, 1.0, 0.0, 1.0), // 緑
		float4(0.0, 0.0, 1.0, 1.0), // 青
		float4(1.0, 1.0, 0.0, 1.0), // 黄
		float4(0.0, 1.0, 1.0, 1.0), // シアン
		float4(1.0, 0.0, 1.0, 1.0) // マゼンタ
	};

	gOutput[dispatchThreadId] = colors[dispatchThreadId.z]; //!< 各面に色を設定
	
}
