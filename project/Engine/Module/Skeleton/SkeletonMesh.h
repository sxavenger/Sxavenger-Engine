#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* animation
#include "Skeleton.h"
#include "SkinCluster.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/Asset/ModelAnimator/Model/Model.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletonMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonMesh {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkeletonMesh()  = default;
	~SkeletonMesh() = default;

	void Create(const std::shared_ptr<Model>& model);

	//* animation option *//

	void UpdateAnimation(const Animation& animation, DeltaTimePoint<TimeUnit::s> time, bool isLoop = true);
	//void UpdateAnimation(const AnimationGroup& animationGroup, DeltaTimePoint<TimeUnit::s> time, bool isLoop = true);

	void UpdateTransitionAnimationAToB(
		const Animation& animationA, DeltaTimePoint<TimeUnit::s> timeA,
		const Animation& animationB, DeltaTimePoint<TimeUnit::s> timeB,
		float t
	);

	//* meshes option *//

	void SetIABuffer(uint32_t meshIndex) const;

	void DrawCall(uint32_t meshIndex, uint32_t instanceCount = 1) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	std::shared_ptr<const Model> model_;

	//* member *//

	Skeleton skeleton_;

	std::vector<SkinCluster>                                                               skinClusters_;
	std::vector<std::unique_ptr<DxObject::VertexUnorderedDimensionBuffer<MeshVertexData>>> skinnedVertex_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateSkeleton();
	void CreateSkinCluster();
	void CreateSkinnedVertex();

	void Skinning();

};
