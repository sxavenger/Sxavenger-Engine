//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ReflectionHitgroupCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================
//* local buffer

//!< t0, t1 はReflectionHitgroupCommon.hlsliで定義済み
Texture2D<float4> gAlbedo  : register(t2);
SamplerState      gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainGeometryClosesthit(inout Payload payload, Attribute attribute) {
	// HACK: recrusion処理やや透明の場合は, anyhitで処理を行う
	//<- anyhitのオーバーヘッドの考慮
	
	Vertex vtx = GetVertex(attribute);

	float4 color = gAlbedo.SampleLevel(gSampler, vtx.texcoord, 0.0f);
	
	payload.color = color.rgb;
}

