#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "../../Content/Mesh.hlsli"
#include "../../Content/Material.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct CacheFragmentInput {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 position  : SV_Position;
	float2 texcoord  : TEXCOORD0;
	float3 normal    : NORMAL0;
	float3 worldPos  : POSITION0;
	float3 tangent   : TANGENT0;
	float3 bitangent : BITANGENT0;
	
};

struct CacheFragmentOutput {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 albedo   : SV_Target0;
	float4 normal   : SV_Target1;
	float4 position : SV_Target2;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetAlbedo(float3 color) {
		albedo = float4(color, 1.0f);
	}

	void SetNormal(float3 n) {
		float3 map = (n + 1.0f) * 0.5f; //!< [-1, 1] -> [0, 1]
		normal = float4(map, 1.0f);
	}

	void SetPosition(float3 pos) {
		position = float4(pos, 1.0f);
	}
	
};
