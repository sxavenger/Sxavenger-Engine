#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// ViewInfo structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ViewInfo {

	//* member *//
	
	float4x4 view;
	float4x4 world; //!< view inverse
	float4x4 proj;
	float4x4 projInv;
	//float    near;
	//float    far;
	
	//* method *//
	
	float3 GetPosition() {
		return (float3)world[3];
	}
	
	float4x4 GetViewProj() {
		return mul(view, proj);
	}
	
};
