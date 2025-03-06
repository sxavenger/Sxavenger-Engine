#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Camera.hlsli"
#include "../../Light.hlsli"
#include "../Component/TransformComponent.hlsli"
#include "DeferredBuffers.hlsli"
#include "../BRDFLib.hlsli"

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

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = mul(gCamera.view, gCamera.proj);

//* t10 ~ t13 is defined in DeferredBuffer.hlsli
StructuredBuffer<TransformComponent> gTransform : register(t10); //!< Light transform buffer

RaytracingAccelerationStructure gScene : register(t11);

////////////////////////////////////////////////////////////////////////////////////////////
// InlineShadow structure
////////////////////////////////////////////////////////////////////////////////////////////
struct InlineShadow {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float strength;
	uint  flag;

	//=========================================================================================
	// public variables
	//=========================================================================================

	float TraceShadow(RayDesc desc) {
		RayQuery<0> q;

		q.TraceRayInline(
			gScene,
			flag,
			0xFF,
			desc
		);

		if (q.Proceed()) {
			return 1.0f - strength;
		}

		return 1.0f;
	}
	
};
