#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* animation
#include "Skeleton.h"
#include "SkinCluster.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/Asset/Model/AssetModel.h>

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

	void Create(const std::shared_ptr<AssetModel>& model);

	//* animation option *//

	void UpdateAnimation(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop = true);
	//void UpdateAnimation(const AnimationGroup& animationGroup, DeltaTimePoint<TimeUnit::s> time, bool isLoop = true);

	void UpdateTransitionAnimation(
		const Animation& before, TimePointf<TimeUnit::second> beforeTime, bool isLoopBefore,
		const Animation& after,  TimePointf<TimeUnit::second> afterTime,  bool isLoopAfter,
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

	std::shared_ptr<Model> model_;

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
