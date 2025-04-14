#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../VertexStructure.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// GeometryVertex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GeometryVertex {
	float4 position  : POSITION0;
	float2 texcoord  : TEXCOORD0;
	float3 normal    : NORMAL0;
	float3 tangent   : TANGENT0;
	float3 bitangent : BITANGENT0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveVertex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PrimitiveVertex {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};
