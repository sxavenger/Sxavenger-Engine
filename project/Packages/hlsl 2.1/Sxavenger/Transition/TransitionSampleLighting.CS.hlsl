//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../SystematicProcess.hlsli"
#include "../../Light.hlsli"
#include "../../Camera3d.hlsli"

//=========================================================================================
// Input
//=========================================================================================

ConstantBuffer<Camera3d> gCamera : register(b0);

//=========================================================================================
// Output
//=========================================================================================

RWTexture2D<float4> gXclipse : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUMTHREADS_X, _NUMTHREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 currentId = dispatchThreadId.xy;
	uint2 size      = gConfig.size;

	// 画像サイズ以上の場合, 書き込みをしない
	if (any(currentId >= size)) {
		return;
	}

	float4 albedo   = GetAlbedo(currentId);
	float3 normal   = GetNormal(currentId);
	float4 position = GetPosition(currentId);
	 
	if (albedo.a == 0.0f) {
		gXclipse[currentId] = (float4)0;
		return;
	}

	float3 lightDirection = normalize(float3(-1.0f, -1.0f, 0.0f));

	float NdotL = dot(normal, -lightDirection);
	float d = pow((NdotL + 1.0f) * 0.5f, 2.0f);
	
	float4 color = (float4)0.0f;
	color.rgb = albedo.rgb * d;
	color.a = albedo.a;
	
	gXclipse[currentId] = color;
}
