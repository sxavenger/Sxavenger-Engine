#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Math/Matrix4x4.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Weight structure
////////////////////////////////////////////////////////////////////////////////////////////

//! @brief 1つのジョイントが1頂点へ与える影響度(ウェイトと頂点index)
struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

//! @brief 1ジョイントのスキニング情報(逆バインドポーズ行列と影響頂点ウェイト群)
struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

SXAVENGER_ENGINE_NAMESPACE_END
