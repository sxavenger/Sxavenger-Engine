#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Matrix4x4.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Weight structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};