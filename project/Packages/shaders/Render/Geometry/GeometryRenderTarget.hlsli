#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// common defines
////////////////////////////////////////////////////////////////////////////////////////////

#define _NOT_USED_1 1

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color) {
	if (color.a <= 0.0f) {
		discard;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// render target for geometry pass
////////////////////////////////////////////////////////////////////////////////////////////

struct GeometryForwardOutput {

	//* member *//
	
	float4 color : SV_Target0;
	
	//* method *//
	
	void SetColor(float4 c) {
		color = c;
		CheckDiscard(c);
	}
	
};

struct GeometryDeferredOutput { //!< FSceneTextures::GBuffers

	//* member *//
	
	float4 normal      : SV_Target0;
	float4 material_ao : SV_Target1;
	float4 albedo      : SV_Target2;
	float4 position    : SV_Target3;
	
	//* method *//
	
	void SetNormal(float3 n) {
		float3 map = n * 0.5f + 0.5f; //!< [-1, 1] -> [0, 1]
		normal = float4(map, _NOT_USED_1);
	}
	
	void SetMaterial(float metallic, float specular, float roughness) {
		material_ao.r = metallic;
		material_ao.g = specular;
		material_ao.b = roughness;
	}
	
	void SetAlbedo(float3 _albedo) {
		albedo.rgb = _albedo;
		albedo.a   = _NOT_USED_1;
	}
	
	void SetAO(float ao) {
		material_ao.a = ao;
	}
	
	void SetPosition(float3 pos) {
		position = float4(pos, _NOT_USED_1);
	}
	
};



