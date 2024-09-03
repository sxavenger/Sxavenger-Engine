#pragma once

/*
[naming convention]
- ConstantBuffer<...>     g... : register
- StructuredBuffer<...>   g... : register
- RWStructuredBuffer<...> g... : register
- Texture2D<...>          g... : register
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"
#include "../Lighting.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input, Output structure
////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput { //!< VertexData structure
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

struct VSOutput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
	float3 worldPos : POSITION0;
};

struct PSOutput {
	float4 color : SV_Target0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// other buffer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformationMatrix { //!< Transform.h /.cpp
	//* members *//
	float4x4 world;
	float4x4 worldInverseTranspose;
};

struct ObjectMaterial { //!< Material.h / .cpp
	//* members *//
	
	float4 color;
	float4x4 uvTransform;
	
	//* methods *//
	
	float4 GetAlbed(float2 texcoord, Texture2D<float4> albed, SamplerState state) {
		float2 transformedUV = mul(float4(texcoord.xy, 0.0f, 1.0f), uvTransform).xy;
		return albed.Sample(state, transformedUV);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckPSOutput(PSOutput output) {
	if (output.color.a == 0.0f) {
		discard;
	}
}
