#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/Address.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// intermediate structures
////////////////////////////////////////////////////////////////////////////////////////////

struct DecalPSInput {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float4 position : SV_Position;
	float clip      : SV_ClipDistance;
	
};

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b0, space2);
static const float4x4 kViewProj = gCamera.GetViewProj();

ConstantBuffer<TransformComponent> gTransform : register(b1, space2);

cbuffer Dimension : register(b2, space2) {
	uint2 dimension;
};

ConstantBuffer<Address> AddressBuffer : register(b3, space2);
