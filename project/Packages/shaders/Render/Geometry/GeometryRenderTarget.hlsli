#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// common defines
////////////////////////////////////////////////////////////////////////////////////////////

#define _NOT_USED_1 1

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color, float threshold = 0.0f) {
	if (color.a <= threshold) {
		discard;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// render target for geometry pass
////////////////////////////////////////////////////////////////////////////////////////////

struct GeometryForwardOutput {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 color : SV_Target0;
	
	//=========================================================================================
	// public methods
	//=========================================================================================
	
	void SetColor(float4 _color) {
		color = _color;
	}
	
};

struct GeometryDeferredOutput { //!< FSceneTextures::GBuffers

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 normal      : SV_Target0;
	float4 materialARM : SV_Target1;
	float4 albedo      : SV_Target2;
	float4 position    : SV_Target3;
	
	//=========================================================================================
	// public methods
	//=========================================================================================
	
	void SetNormal(float3 n) {
		float3 map = (n + 1.0f) * 0.5f; //!< [-1, 1] -> [0, 1]
		normal = float4(map, _NOT_USED_1);
	}
	
	void SetMaterial(float ao, float roughness, float metallic) {
		materialARM.r = ao;
		materialARM.g = max(roughness, 0.02f);
		materialARM.b = metallic;
		materialARM.a = _NOT_USED_1;
	}
	
	void SetAlbedo(float3 _albedo) {
		albedo.rgb = ACES::IDT_sRGB_AP1(_albedo);
		albedo.a   = _NOT_USED_1;
	}
	
	void SetPosition(float3 pos) {
		position = float4(pos, _NOT_USED_1);
	}
	
};

