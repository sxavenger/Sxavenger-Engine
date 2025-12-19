#include "SkinCluster.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinCluster structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinCluster::UpdatePalette(const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		StreamLogger::AssertA(jointIndex < inverseBindPoseMatrices.size(), "index out of range.");

		Matrix4x4 mat = inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		(*palette)[static_cast<uint32_t>(jointIndex)].Transfer(mat);
	}
}

