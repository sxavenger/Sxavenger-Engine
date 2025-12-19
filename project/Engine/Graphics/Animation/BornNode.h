#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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

SXAVENGER_ENGINE_NAMESPACE_END
