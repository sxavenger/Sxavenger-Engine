//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../SystematicProcess.hlsli"
#include "../../Light.hlsli"
#include "../../Camera3d.hlsli"

//#define _LAMBERT

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

#ifdef _LAMBERT
	float3 lightDirection = normalize(float3(0.0f, 1.0f, 0.0f));

	//float NdotL = dot(normal, -lightDirection);
	//float d = pow((NdotL + 1.0f) * 0.5f, 2.0f);
	
	float d = HalfLambertReflection(normal, lightDirection);
	
	float4 color = (float4)0.0f;
	color.rgb = albedo.rgb * d;
	color.a = albedo.a;
	
	gXclipse[currentId] = color;
	
#else
	
	float3 l = -normalize(float3(-1.0f, -1.0f, 0.0f));
	float3 v = normalize(gCamera.position - position.xyz);
	float3 n = normal;
	
	float3 c_saface = albedo.rgb;
	
	float3 c_cool      = float3(0.0f, 0.0f, 0.55f) + 0.25f * c_saface;
	float3 c_warm      = float3(0.3f, 0.3f, 0.0f) + 0.25f * c_saface;
	float3 c_highlight = float3(1.0f, 1.0f, 1.0f);
	
	float t  = (dot(n, l) + 1.0f) / 2.0f;
	float3 r = 2.0f * dot(n, l) * n - l;
	float s = saturate(100.0f * dot(r, v) - 97.0f);
	
	float3 c_shadered = s * c_highlight + (1.0f - s) * (t * c_warm + (1.0f - t) * c_cool);

	gXclipse[currentId] = float4(c_shadered, 1.0f);
  
#endif
}
