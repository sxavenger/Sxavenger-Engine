#include "SkinnedMeshRendererComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// InputSkinnedMesh structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRendererComponent::InputSkinnedMesh::Create(const DirectXThreadContext* context, const Model::AssimpMesh& mesh) {
	CreateVetex(mesh);
	CreateBottomLevelAS(context, mesh);
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::CreateVetex(const Model::AssimpMesh& mesh) {
	vertex = std::make_unique<VertexUnorderedDimensionBuffer<MeshVertexData>>();
	vertex->Create(SxavengerSystem::GetDxDevice(), mesh.input.GetVertex()->GetSize());
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::CreateBottomLevelAS(const DirectXThreadContext* context, const Model::AssimpMesh& mesh) {
	D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
	desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	desc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	desc.Triangles.VertexBuffer.StartAddress  = vertex->GetGPUVirtualAddress();
	desc.Triangles.VertexBuffer.StrideInBytes = vertex->GetStride();
	desc.Triangles.VertexCount                = vertex->GetSize();
	desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.Triangles.IndexBuffer                = mesh.input.GetIndex()->GetGPUVirtualAddress();
	desc.Triangles.IndexCount                 = mesh.input.GetIndex()->GetIndexCount();
	desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

	bottomLevelAS.Build(SxavengerSystem::GetDxDevice(), context->GetDxCommand(), desc);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRendererComponent::Create(const std::shared_ptr<Model>& model, uint32_t index) {
	//!< hack
	skeleton_ = model->GetSkeleton();

	mesh_.Create(SxavengerSystem::GetMainThreadContext(), model->GetMesh(index));
	CreateCluster(model->GetMesh(index));
}

const TransformComponent* SkinnedMeshRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

void SkinnedMeshRendererComponent::CreateCluster(const Model::AssimpMesh& mesh) {
	const uint32_t kVertexSize = mesh.input.GetVertex()->GetSize();

	//* info
	cluster_.info = std::make_unique<DimensionBuffer<uint32_t>>();
	cluster_.info->Create(SxavengerSystem::GetDxDevice(), 1);
	cluster_.info->At(0) = kVertexSize;

	//* influence
	cluster_.influence = std::make_unique<DimensionBuffer<VertexInfluence>>();
	cluster_.influence->Create(SxavengerSystem::GetDxDevice(), kVertexSize);
	cluster_.influence->Fill(VertexInfluence{});

	//* palette
	cluster_.palette = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	cluster_.palette->Create(SxavengerSystem::GetDxDevice(), static_cast<uint32_t>(skeleton_.GetJointSize()));

	//* inverse pose bind matrix
	cluster_.inverseBindPoseMatrices.resize(skeleton_.GetJointSize());
	std::generate(cluster_.inverseBindPoseMatrices.begin(), cluster_.inverseBindPoseMatrices.end(), Matrix4x4::Identity);

	for (const auto& jointWeight : mesh.jointWeights) {
		if (!skeleton_.jointMap.contains(jointWeight.first)) {
			continue; //!< この名前のJointは存在しない
		}

		uint32_t jointIndex = skeleton_.jointMap.at(jointWeight.first);
		cluster_.inverseBindPoseMatrices[jointIndex] = jointWeight.second.inverseBindPoseMatrix;

		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			// 該当のindexのinfluence情報の参照
			auto& currentInfluence = (*cluster_.influence)[vertexWeight.vertexIndex];

			// weightの格納
			for (size_t i = 0; i < kInfluenceCount; ++i) {
				if (currentInfluence.weights[i] == 0.0f) {
					currentInfluence.weights[i]      = vertexWeight.weight;
					currentInfluence.jointIndices[i] = jointIndex;
					break;
				}
			}
		}
	}
}
