#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/Math.hlsli"
#include "../../Library/Hammersley.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// defines
////////////////////////////////////////////////////////////////////////////////////////////

#define _NUM_THREADS_X 16
#define _NUM_THREADS_Y 16

//=========================================================================================
// buffers
//=========================================================================================

//* input
TextureCube<float4> gEnvironment : register(t10); //!< 環境マップ
SamplerState gEnvironmentSampler : register(s10); //!< 環境マップSampler

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 GetDirection(float2 uv, uint face) {
	switch (face) {
		case 0:
			return normalize(float3(1.0f, -uv.y, -uv.x)); //!< +x

		case 1:
			return normalize(float3(-1.0f, -uv.y, uv.x)); //!< -x

		case 2:
			return normalize(float3(uv.x, 1.0f, uv.y)); //!< +y

		case 3:
			return normalize(float3(uv.x, -1.0f, -uv.y)); //!< -y

		case 4:
			return normalize(float3(uv.x, -uv.y, 1.0f)); //!< +z

		case 5:
			return normalize(float3(-uv.x, -uv.y, -1.0f)); //!< -z
	}

	return float3(0.0f, 0.0f, 0.0f); //!< error.
}

/*
# reference
 https://jcgt.org/published/0006/01/01/
*/
void TangentSpace(float3 n, out float3 t, out float3 b) {
	float s = (n.z >= 0.0f) ? 1.0f : -1.0f;
	float a = -1.0f / (s + n.z);
	float bv = n.x * n.y * a;

	t = float3(1.0f + s * n.x * n.x * a, s * bv, -s * n.x);
	b = float3(bv, s + n.y * n.y * a, -n.y);
}
