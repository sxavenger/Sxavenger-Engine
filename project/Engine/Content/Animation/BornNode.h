#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Transform/Transform.h>

//* c++
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// BornNode structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BornNode {
	QuaternionTransform transform;
	Matrix4x4           localMatrix;

	std::string name;
	std::vector<BornNode> children;
};