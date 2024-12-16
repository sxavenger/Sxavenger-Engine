#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Define Configs
////////////////////////////////////////////////////////////////////////////////////////////

#define _AMPLIFICATION_NUMTHREAD 32
#define _MAX_VERTICES            64
#define _MAX_INDICES             128
//!< Engine/Game/InputMesh.h

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {
	uint meshletIndices[_AMPLIFICATION_NUMTHREAD];
	uint instanceIndices[_AMPLIFICATION_NUMTHREAD];
};

////////////////////////////////////////////////////////////////////////////////////////////
// Meshlet info structure
////////////////////////////////////////////////////////////////////////////////////////////

struct Meshlet {
	uint vertexCount;
	uint vertexOffset;
	uint primitiveCount;
	uint primitiveOffset;
};

struct CullData {
	float4 boundingSphere;
	uint   normalCone;
	float  apexOffset;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh info structure
////////////////////////////////////////////////////////////////////////////////////////////

struct MeshInfo {
	uint meshletCount;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Common methods
////////////////////////////////////////////////////////////////////////////////////////////

uint3 UnpackPrimitiveIndex(uint packedIndex) {
	return uint3(
		packedIndex & 0x3FF,
		(packedIndex >> 10) & 0x3FF,
		(packedIndex >> 20) & 0x3FF
	);
}

bool IsConeDegenerate(uint normalCone) {
	return (normalCone >> 24) == 0xff;
}

float4 UnpackCone(uint packed) {
	float4 v;
	v.x = float((packed >> 0)  & 0xFF);
	v.y = float((packed >> 8)  & 0xFF);
	v.z = float((packed >> 16) & 0xFF);
	v.w = float((packed >> 24) & 0xFF);

	v /= 255.0f;
	v.xyz = v.xyz * 2.0f - 1.0f;

	return v;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Frustum
////////////////////////////////////////////////////////////////////////////////////////////

//=========================================================================================
// FrustumPlanes structure
//=========================================================================================
struct FrustumPlanes {
	float4 planes[6];
};

namespace FrustumPlaneType {
	static const uint LEFT   = 0,
	                  RIGHT  = 1,
	                  BOTTOM = 2,
	                  TOP    = 3,
	                  NEAR   = 4,
	                  FAR    = 5;
}


//=========================================================================================
// Frustum methods
//=========================================================================================

FrustumPlanes CreateFrustumPlanes(float4x4 view, float4x4 proj) {
	FrustumPlanes result;
	
	float4x4 clipMatrix = transpose(mul(view, proj));
	
	result.planes[FrustumPlaneType::LEFT]   = clipMatrix[3] + clipMatrix[0];
	result.planes[FrustumPlaneType::RIGHT]  = clipMatrix[3] - clipMatrix[0];
	result.planes[FrustumPlaneType::BOTTOM] = clipMatrix[3] + clipMatrix[1];
	result.planes[FrustumPlaneType::TOP]    = clipMatrix[3] - clipMatrix[1];
	result.planes[FrustumPlaneType::NEAR]   = clipMatrix[2];
	result.planes[FrustumPlaneType::FAR]    = clipMatrix[3] - clipMatrix[2];
	
	for (uint i = 0; i < 6; ++i) {
		result.planes[i] /= length(result.planes[i].xyz);
	}
	
	return result;
}

bool IsVisible(FrustumPlanes planes, CullData c, float4x4 world, float4 viewPos) {
	
	float3 scale   = float3(length(world[0]), length(world[1]), length(world[2]));
	float maxScale = max(scale.x, max(scale.y, scale.z));
	
	float4 center = mul(float4(c.boundingSphere.xyz, 1.0f), world);
	float radius = c.boundingSphere.w * maxScale;
	
	// frustum culling
	for (uint i = 0; i < 6; ++i) {
		if (dot(center.xyz, planes.planes[i].xyz) + planes.planes[i].w <= -radius) {
			return false;
		}
	}
	
	// cone culling
	if (IsConeDegenerate(c.normalCone)) {
		return true;
	}
	
	float4 normalCone = UnpackCone(c.normalCone);
	
	float3 axis = normalize(mul(float4(normalCone.xyz, 0), world)).xyz;
	
	float3 apex = center.xyz - axis * c.apexOffset * scale;
	float3 view = normalize(viewPos.xyz - apex);
	
	if (dot(view, -axis) > normalCone.w) {
		return false;
	}
	
	return true;
}

/* ex. buffers
StructuredBuffer<MSInput> sVertices   : register(t0);
StructuredBuffer<uint> sIndices       : register(t1);
StructuredBuffer<Meshlet> sMeshlets   : register(t2);
StructuredBuffer<uint> sPrimitives    : register(t3);
StructuredBuffer<CullData> sCullData  : register(t4);

ConstantBuffer<MeshInfo> gMeshInfo              : register(b0);
ConstantBuffer<Camera> gCamera                  : register(b1);
ConstantBuffer<TransformationMatrix> gTransform : register(b2);

static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);
*/