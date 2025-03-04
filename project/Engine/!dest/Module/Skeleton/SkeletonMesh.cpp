#include "SkeletonMesh.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletonMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkeletonMesh::Create(const std::shared_ptr<AssetModel>& model) {
	model->WaitComplete();

	// 引数の保存
	model_ = model;

	CreateSkeleton();
	CreateSkinCluster();
	CreateSkinnedVertex();
}

void SkeletonMesh::CreateBottomLevelAS(const DirectXThreadContext* context) {
	if (isCreateBottomLevelAS_) {
		return;
	}

	bottomLevelASs_.resize(model_->GetMeshSize());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {

		D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
		desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		desc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		desc.Triangles.VertexBuffer.StartAddress  = skinnedVertex_[i]->GetGPUVirtualAddress();
		desc.Triangles.VertexBuffer.StrideInBytes = skinnedVertex_[i]->GetStride();
		desc.Triangles.VertexCount                = skinnedVertex_[i]->GetSize();
		desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
		desc.Triangles.IndexBuffer                = model_->GetMesh(i).input.GetIndex()->GetGPUVirtualAddress();
		desc.Triangles.IndexCount                 = model_->GetMesh(i).input.GetIndex()->GetIndexCount();
		desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

		bottomLevelASs_[i].Build(SxavengerSystem::GetDxDevice(), context->GetDxCommand(), desc);
	}

	isCreateBottomLevelAS_ = true;
}

void SkeletonMesh::UpdateAnimation(const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop) {
	skeleton_.Update(animation, time, isLoop);
	Skinning();
}

//void SkeletonMesh::UpdateAnimation(const AnimationGroup& animationGroup, DeltaTimePoint<TimeUnit::s> time, bool isLoop) {
//	skeleton_.Update(animationGroup, time, isLoop);
//	Skinning();
//}

void SkeletonMesh::UpdateTransitionAnimation(
	const Animation& before, TimePointf<TimeUnit::second> beforeTime, bool isLoopBefore,
	const Animation& after, TimePointf<TimeUnit::second> afterTime, bool isLoopAfter,
	float t) {

	skeleton_.TransitionAnimation(
		before, beforeTime, isLoopBefore,
		after, afterTime, isLoopAfter,
		t
	);

	Skinning();
}

void SkeletonMesh::UpdateBottomLevelAS(const DirectXThreadContext* context) {
	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		bottomLevelASs_[i].Update(context->GetDxCommand());
	}
}

void SkeletonMesh::SetIABuffer(const DirectXThreadContext* context, uint32_t meshIndex) const {

	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = skinnedVertex_[meshIndex]->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = model_->GetMesh(meshIndex).input.GetIndex()->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

void SkeletonMesh::DrawCall(const DirectXThreadContext* context, uint32_t meshIndex, uint32_t instanceCount) const {
	model_->DrawCall(context, meshIndex, instanceCount);
}

void SkeletonMesh::CreateSkeleton() {
	skeleton_.Create(model_->GetRoot());
}

void SkeletonMesh::CreateSkinCluster() {

	skinClusters_.resize(model_->GetMeshSize());

	for (uint32_t i = 0; i < skinClusters_.size(); ++i) {

		const uint32_t kVertexSize = model_->GetMesh(i).input.GetVertex()->GetSize();

		SkinCluster& skinCluster = skinClusters_[i];

		//* info
		skinCluster.info = std::make_unique<DimensionBuffer<uint32_t>>();
		skinCluster.info->Create(SxavengerSystem::GetDxDevice(), 1);
		(*skinCluster.info)[0] = kVertexSize;

		//* influence
		skinCluster.influence = std::make_unique<DimensionBuffer<VertexInfluence>>();
		skinCluster.influence->Create(SxavengerSystem::GetDxDevice(), kVertexSize);

		std::fill(skinCluster.influence->GetMappedData().begin(), skinCluster.influence->GetMappedData().end(), VertexInfluence{});
		//std::memset(skinCluster.influence->GetData(), 0, skinCluster.influence->GetStructureSize() * skinClusterElement.influenceResource->GetIndexSize()); //!< 0で埋めておく

		//* palette
		skinCluster.palette = std::make_unique<DimensionBuffer<TransformationMatrix>>();
		skinCluster.palette->Create(SxavengerSystem::GetDxDevice(), static_cast<uint32_t>(skeleton_.joints.size()));

		//* inverse pose bind matrix
		skinCluster.inverseBindPoseMatrices.resize(skeleton_.joints.size());
		std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), Matrix4x4::Identity);


		for (const auto& jointWeight : model_->GetMesh(i).jointWeights) {
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
					if (currentInfluence.weights[ij] == 0.0f) {
						currentInfluence.weights[ij]      = vertexWeight.weight;
						currentInfluence.jointIndices[ij] = jointIndex;
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
		skinnedVertex_[i]->Create(SxavengerSystem::GetDxDevice(), model_->GetMesh(i).input.GetVertex()->GetSize());
	}
}

void SkeletonMesh::Skinning() {

	SxavengerModule::SetSkinningPipeline(SxavengerSystem::GetMainThreadContext());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skinClusters_[i].UpdatePalette(skeleton_);

		//* skinning
		// TODO: skinning
		BindBufferDesc bind = {};
		bind.SetAddress("gInputVertex",  model_->GetMesh(i).input.GetVertex()->GetGPUVirtualAddress());
		bind.SetAddress("gPalette",      skinClusters_[i].palette->GetGPUVirtualAddress());
		bind.SetAddress("gInfluence",    skinClusters_[i].influence->GetGPUVirtualAddress());
		bind.SetAddress("gInfo",         skinClusters_[i].info->GetGPUVirtualAddress());
		bind.SetAddress("gOutputVertex", skinnedVertex_[i]->GetGPUVirtualAddress());

		SxavengerModule::DispatchSkinningPipeline(SxavengerSystem::GetMainThreadContext(), bind, (*skinClusters_[i].info)[0]);
	}
}
