#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Camera3d.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Ray structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Ray {
	float3 origin;
	float3 direction;
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

Ray GenerateRay(float2 uv, Camera3d camera) {
	
	uv.y = -uv.y; //!< yŽ²”½“]
	
	// ‹tŽË‰e•ÏŠ·
	float4 clip = float4(uv, 0.0f, 1.0f);
	float4 view = mul(clip, camera.projInverseMatrix);
	view /= view.w;
	
	Ray ray = (Ray)0;
	
	// ƒJƒƒ‰‹óŠÔ‚Å‚Ìray‚ðƒ[ƒ‹ƒh‹óŠÔ‚É•ÏŠ·
	ray.direction = normalize(mul(float4(view.xyz, 0.0f), camera.worldMatrix).xyz); 
	ray.origin    = camera.position;
	
	return ray;
}

