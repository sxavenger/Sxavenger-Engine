#include "SkeletonMesh.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletonMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkeletonMesh::Create(const Model* model) {

	// 引数の保存
	model_ = model;

	CreateSkeleton();
	CreateSkinCluster();
	CreateSkinnedVertex();
}

void SkeletonMesh::UpdateAnimation(const Animation& animation, DeltaTimePoint<TimeUnit::s> time, bool isLoop) {
	skeleton_.Update(animation, time, isLoop);
	Skinning();
}

void SkeletonMesh::UpdateAnimation(const AnimationGroup& animationGroup, DeltaTimePoint<TimeUnit::s> time, bool isLoop) {
	skeleton_.Update(animationGroup, time, isLoop);
	Skinning();
}

void SkeletonMesh::UpdateTransitionAnimationAToB(
	const Animation& animationA, DeltaTimePoint<TimeUnit::s> timeA,
	const Animation& animationB, DeltaTimePoint<TimeUnit::s> timeB,
	float t) {

	skeleton_.TransitionAnimation(
		animationA, timeA,
		animationB, timeB,
		t
	);

	Skinning();
}

void SkeletonMesh::SetIABuffer(uint32_t meshIndex) const {

	auto commandList = SxavengerSystem::GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = skinnedVertex_[meshIndex]->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = model_->GetInputMesh(meshIndex).GetIndex()->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

void SkeletonMesh::DrawCall(uint32_t meshIndex, uint32_t instanceCount) const {
	model_->DrawCall(meshIndex, instanceCount);
}

void SkeletonMesh::CreateSkeleton() {
	skeleton_.Create(model_->GetRootNode());
}

void SkeletonMesh::CreateSkinCluster() {

	skinClusters_.resize(model_->GetMeshSize());

	for (uint32_t i = 0; i < skinClusters_.size(); ++i) {

		const uint32_t kVertexSize = model_->GetMeshData(i).mesh.GetVertex()->GetSize();

		SkinCluster& skinCluster = skinClusters_[i];

		//* info
		skinCluster.info = std::make_unique<DimensionBuffer<uint32_t>>();
		skinCluster.info->Create(SxavengerSystem::GetDxDevice(), 1);
		(*skinCluster.info)[0] = kVertexSize;

		//* influence
		skinCluster.influence = std::make_unique<DimensionBuffer<VertexInfluence>>();
		skinCluster.influence->Create(SxavengerSystem::GetDxDevice(), kVertexSize);

		//std::fill();
		//std::memset(skinClusterElement.influenceResource->GetData(), 0, skinClusterElement.influenceResource->GetStructureSize() * skinClusterElement.influenceResource->GetIndexSize()); //!< 0で埋めておく

		//* palette
		skinCluster.palette = std::make_unique<DimensionBuffer<TransformationMatrix>>();
		skinCluster.palette->Create(SxavengerSystem::GetDxDevice(), static_cast<uint32_t>(skeleton_.joints.size()));

		//* inverse pose bind matrix
		skinCluster.inverseBindPoseMatrices.resize(skeleton_.joints.size());
		std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), Matrix4x4::Identity);


		for (const auto& jointWeight : model_->GetMeshData(i).jointWeights) {
			if (!skeleton_.jointMap.contains(jointWeight.first)) {
				continue; //!< この名前のJointは存在しない
			}

			uint32_t jointIndex = skeleton_.jointMap.at(jointWeight.first);
			skinCluster.inverseBindPoseMatrices[jointIndex] = jointWeight.second.inverseBindPoseMatrix;

			for (const auto& vertexWeight : jointWeight.second.vertexWeights) {

				// 該当のindexのinfluence情報の参照
				auto& currentInfluence = (*skinCluster.influence)[vertexWeight.vertexIndex];

				// influenceの空いているところに代入
				for (uint32_t ij = 0; ij < kInfluenceCount; ++ij) {
					if (currentInfluence.weights[i] == 0.0f) {
						currentInfluence.weights[i]      = vertexWeight.weight;
						currentInfluence.jointIndices[i] = jointIndex;
						break;
					}
				}
			}
		}

	}
}

void SkeletonMesh::CreateSkinnedVertex() {

	skinnedVertex_.resize(model_->GetMeshSize());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skinnedVertex_[i] = std::make_unique<VertexUnorderedDimensionBuffer<MeshVertexData>>();
		skinnedVertex_[i]->Create(SxavengerSystem::GetDxDevice(), model_->GetInputMesh(i).GetVertex()->GetSize());
	}
}

void SkeletonMesh::Skinning() {

	SxavengerContent::SetSkinningPipeline(SxavengerSystem::GetMainThreadContext());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skinClusters_[i].UpdatePalette(skeleton_);

		//* skinning
		// TODO: skinning
		BindBufferDesc bind = {};
		bind.SetAddress("gInputVertex",  model_->GetInputMesh(i).GetVertex()->GetGPUVirtualAddress());
		bind.SetAddress("gPalette",      skinClusters_[i].palette->GetGPUVirtualAddress());
		bind.SetAddress("gInfluence",    skinClusters_[i].influence->GetGPUVirtualAddress());
		bind.SetAddress("gInfo",         skinClusters_[i].info->GetGPUVirtualAddress());
		bind.SetAddress("gOutputVertex", skinnedVertex_[i]->GetGPUVirtualAddress());

		SxavengerContent::DispatchSkinning(SxavengerSystem::GetMainThreadContext(), bind, (*skinClusters_[i].info)[0]);
	}
}
