#include "SkinCluster.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SkinCluster structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinCluster::UpdatePalette(const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		Assert(jointIndex < inverseBindPoseMatrices.size(), "index out of range.");

		Matrix4x4 mat = inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		(*palette)[static_cast<uint32_t>(jointIndex)].Transfer(mat);
	}
}

