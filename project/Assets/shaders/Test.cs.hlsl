//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Packages/shaders/Library/ACES.hlsli"
#include "../../Packages/shaders/Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* texture
AppendStructuredBuffer<uint> gAppend   : register(u0);
ConsumeStructuredBuffer<uint> gConsume : register(u1);

RWStructuredBuffer<uint2> gCounter : register(u2);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	gAppend.Append(dispatchThreadId.x);
	InterlockedAdd(gCounter[0].y, 1);
	
}
