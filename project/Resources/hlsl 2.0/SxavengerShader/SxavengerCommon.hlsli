#pragma once

/*
[naming convention]
 - ConstantBuffer<...>     g... : register(.b)
 - StructuredBuffer<...>   g... : register(.t)
 - RWStructuredBuffer<...> g... : register(.u)
 - Texture2D<...>          g... : register(.t)
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"
#include "../Light.hlsli"

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
// Transform structure [Transform.h / .cpp]
////////////////////////////////////////////////////////////////////////////////////////////

struct TransformationMatrix { //!< 
	//* members *//
	float4x4 world;
	float4x4 worldInverseTranspose;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Material structure [Material.h / .cpp]
////////////////////////////////////////////////////////////////////////////////////////////

namespace LamertType {
	static const int TYPE_LAMBERT_NONE = 0,
	                 TYPE_LAMBERT      = 1,
	                 TYPE_HALF_LAMBERT = 2;
}

namespace PhongType {
	static const int TYPE_PHONG_NONE  = 0,
	                 TYPE_PHONG       = 1,
	                 TYPE_BLINN_PHONG = 2;
}

struct ObjectMaterial {
	
	//* members *//
	
	float4   color;
	float4x4 uvTransform;
	int      lambertType;
	int      phongType;
	
	//* methods *//
	
	float4 GetColor(float2 texcoord, Texture2D<float4> texture, SamplerState state) {
		float2 transformedUV = mul(float4(texcoord.xy, 0.0f, 1.0f), uvTransform).xy;
		return texture.Sample(state, transformedUV);
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color) {
	if (color.a == 0.0f) {
		discard;
	}
}
