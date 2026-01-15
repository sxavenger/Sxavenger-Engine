#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// common defines
////////////////////////////////////////////////////////////////////////////////////////////

#define NOT_USED_1 1

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color, float threshold = 0.0f) {
	if (color.a <= threshold) {
		discard;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Render Target structures / Forward
////////////////////////////////////////////////////////////////////////////////////////////

struct GeometryForwardTransparentOutput {
	//!< Transparent Forward Render Target [Weighted Blended OIT]

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 accumulate : SV_Target0;
	float revealage   : SV_Target1;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetColor(float3 albedo, float transparency, float weight) {
		accumulate = float4(albedo * transparency, transparency) * weight;
		revealage  = transparency;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Render Target structures / Deferred
////////////////////////////////////////////////////////////////////////////////////////////

struct GeometryDeferredOutput {
	//!< Deferred Render Target

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 albedo      : SV_Target0;
	float4 normal      : SV_Target1;
	float4 materialARM : SV_Target2;
	float4 position    : SV_Target3;
	
	//=========================================================================================
	// public methods
	//=========================================================================================
	
	void SetNormal(float3 n) {
		float3 map = (n + 1.0f) * 0.5f; //!< [-1, 1] -> [0, 1]
		normal = float4(map, NOT_USED_1);
	}
	
	void SetMaterial(float ao, float roughness, float metallic) {
		materialARM.r = ao;
		materialARM.g = max(roughness, 0.02f);
		materialARM.b = metallic;
		materialARM.a = NOT_USED_1;
	}
	
	void SetAlbedo(float3 _albedo /*AP1*/) {
		albedo.rgb = _albedo;
		albedo.a   = NOT_USED_1;
	}
	
	void SetPosition(float3 pos) {
		position = float4(pos, NOT_USED_1);
	}
	
};

