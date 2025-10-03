//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//struct Parameter {
//	uint2 size; //!< texture size
//	uint2 tile; //!< tile一つ分のサイズ
//};
//ConstantBuffer<Parameter> gParameter : register(b0);

//* textures
Texture2D<float4> gInput    : register(t0);
RWTexture3D<float3> gOutput : register(u0);

//* parameter
cbuffer Parameter : register(b0) {
	uint2 texSize; //!< texture size
	uint2 texTile; //!< tile一つ分のサイズ
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)] //!< input texture size dispatch.
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (any(index >= texSize)) {
		return;
	}

	float3 color = gInput[index].rgb;
	
	const uint2 kTileCount = texSize /texTile;
	uint2 tileIndex        = index / texTile;

	// 3D LUT texture coordinate.z
	uint3 dimension = uint3(
		index.x % texTile.x,
		index.y % texTile.y,
		tileIndex.x + tileIndex.y * kTileCount.x
	);

	gOutput[dimension] = color;
	
	
}
