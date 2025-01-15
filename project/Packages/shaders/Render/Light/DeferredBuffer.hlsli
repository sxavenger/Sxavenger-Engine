#pragma once

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gNormal   : register(t10);
Texture2D<float4> gPosition : register(t11);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 GetNormal(float2 uv) {
	float3 normal = gNormal[uv].rgb;
	return normalize(normal * 2.0f - 1.0f);
}

float3 GetPosition(float2 uv) {
	return gPosition[uv].rgb;
}
