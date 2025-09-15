#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/Lighting.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

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

cbuffer Dimension : register(b10) {
	uint2 size;
};

ConstantBuffer<CameraComponent> gCamera : register(b11);
static const float4x4 kViewProj = gCamera.GetViewProj();

StructuredBuffer<TransformComponent> gTransforms : register(t10);

RaytracingAccelerationStructure gScene : register(t11);

Texture2D<float4> gAlbedo   : register(t10, space1);
Texture2D<float4> gNormal   : register(t11, space1);
Texture2D<float4> gPosition : register(t12, space1);
Texture2D<float4> gMaterial : register(t13, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// Surface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Surface
{

	//* member *//
	
	float3 position;
	float3 albedo;
	float3 normal;

	float ao;
	float roughness;
	float metallic;
	
	//* methods *//

	float3 GetAlbedo(uint2 index) {
		return gAlbedo.Load(uint3(index, 0)).rgb;
	}

	float3 GetNormal(uint2 index) {
		float3 normal = gNormal.Load(uint3(index, 0)).rgb;
		return normalize(normal * 2.0f - 1.0f);
	}

	float3 GetPosition(uint2 index) {
		return gPosition.Load(uint3(index, 0)).rgb;
	}

	float3 GetMaterial(uint2 index) {
		return gMaterial.Load(uint3(index, 0)).rgb;
	}

	void GetSurface(uint2 index) {
		
		position  = GetPosition(index);
		albedo    = GetAlbedo(index);
		normal    = GetNormal(index);
		
		float3 material = GetMaterial(index);
		ao        = material.r;
		roughness = material.g;
		metallic  = material.b;
	
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// static const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;
static const float kTMax = 10000.0f;
