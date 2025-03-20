#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* animation
#include "Skeleton.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Transform/Transform.h>

//* c++
#include <cstdint>
#include <array>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Influence structure
////////////////////////////////////////////////////////////////////////////////////////////

static const uint32_t kInfluenceCount = 4;

struct VertexInfluence {
	std::array<float,   kInfluenceCount> weights;
	std::array<int32_t, kInfluenceCount> jointIndices;
};

////////////////////////////////////////////////////////////////////////////////////////////
// SkinCluster structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SkinCluster {

	//=========================================================================================
	// public methods
	//=========================================================================================

	void UpdatePalette(const Skeleton& skeleton);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<Matrix4x4> inverseBindPoseMatrices;

	std::unique_ptr<DxObject::DimensionBuffer<VertexInfluence>>      influence;
	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> palette;
	std::unique_ptr<DxObject::DimensionBuffer<uint32_t>>             info;

};
