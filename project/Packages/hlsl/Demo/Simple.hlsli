#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Transform.hlsli"
#include "../Camera3d.hlsli"
#include "../VertexStructure.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PSInput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

struct PSOutput {
	float4 color : SV_Target0;
};