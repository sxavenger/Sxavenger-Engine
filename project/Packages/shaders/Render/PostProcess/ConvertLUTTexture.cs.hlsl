//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	uint2 size; //!< texture size
	uint2 tile; //!< tile一つ分のサイズ
};
ConstantBuffer<Parameter> gParameter : register(b0);
Texture2D<float4> gInput             : register(t0);

RWTexture3D<float3> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)] //!< input texture size dispatch.
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (any(index >= gParameter.size)) {
		return;
	}

	float3 color = gInput[index].rgb;
	
	const uint2 kTileCount = gParameter.size / gParameter.tile;
	uint2 tileIndex        = index / gParameter.tile;

	// 3D LUT texture coordinate.z
	uint3 dimension = uint3(
		index.x % gParameter.tile.x,
		index.y % gParameter.tile.y,
		tileIndex.x + tileIndex.y * kTileCount.x
	);

	gOutput[dimension] = color;
	
	
}
