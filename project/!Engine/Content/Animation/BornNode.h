#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// BornNode structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief node構造体
struct BornNode {
public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	QuaternionTransform transform;
	Matrix4x4           localMatrix;

	std::string name;
	std::vector<BornNode> children;

	std::vector<uint32_t> meshIndices;
};
