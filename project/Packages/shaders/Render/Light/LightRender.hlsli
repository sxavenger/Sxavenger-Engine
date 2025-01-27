#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Transform.hlsli"
#include "../../Camera.hlsli"
#include "../../Light.hlsli"
#include "DeferredBuffer.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput2d {
	float2 position : POSITION0;
};

struct VSInput3d {
	float4 position : POSITION0;
};

struct PSInput {
	float4 position : SV_Position;
	uint instanceId : INSTANCE;
};

struct PSOutput {
	float4 color : SV_Target0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Surface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Surface {

	//* member *//
	
	float3 position;
	float3 albedo;
	float3 normal;
	
	//* methods *//
	
	void GetSurface(uint2 index) {
		if (GetDepth(index) == 1.0f) {
			discard; //!< object is not exist.
		}
		
		position = GetPosition(index);
		albedo   = GetAlbedo(index);
		normal   = GetNormal(index);
	}
	
};

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = mul(gCamera.view, gCamera.proj);

//* t10 ~ t13 is defined in DeferredBuffer.hlsli
StructuredBuffer<TransformationMatrix> gTransform : register(t14); //!< Light transform buffer

RaytracingAccelerationStructure gScene : register(t15);
