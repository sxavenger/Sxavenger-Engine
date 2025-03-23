#include "SkinnedMeshRendererComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InputSkinnedMesh structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRendererComponent::InputSkinnedMesh::Create(const DirectXThreadContext* context, const Model::AssimpMesh* mesh) {
	CreateVetex(mesh);
	CreateBottomLevelAS(context, mesh);
	isCreateMesh = true;
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::UpdateBottomLevelAS(const DirectXThreadContext* context) {
	Assert(isCreateMesh, "mesh is not created.");
	bottomLevelAS.Update(context->GetDxCommand());
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::CreateVetex(const Model::AssimpMesh* mesh) {
	vertex = std::make_unique<VertexUnorderedDimensionBuffer<MeshVertexData>>();
	vertex->Create(SxavengerSystem::GetDxDevice(), mesh->input.GetVertex()->GetSize());
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::CreateBottomLevelAS(const DirectXThreadContext* context, const Model::AssimpMesh* mesh) {
	D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
	desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	desc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	desc.Triangles.VertexBuffer.StartAddress  = vertex->GetGPUVirtualAddress();
	desc.Triangles.VertexBuffer.StrideInBytes = vertex->GetStride();
	desc.Triangles.VertexCount                = vertex->GetSize();
	desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.Triangles.IndexBuffer                = mesh->input.GetIndex()->GetGPUVirtualAddress();
	desc.Triangles.IndexCount                 = mesh->input.GetIndex()->GetIndexCount();
	desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

	bottomLevelAS.Build(SxavengerSystem::GetDxDevice(), context->GetDxCommand(), desc);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRendererComponent::CreateMesh(const Model::AssimpMesh* mesh) {
	// reference先のmeshを保持
	referenceMesh_ = mesh;

	mesh_.Create(SxavengerSystem::GetMainThreadContext(), referenceMesh_);
	CreateCluster();
}

void SkinnedMeshRendererComponent::SkinUpdate() {

	//* cluster update *//
	cluster_.UpdatePalette(GetArmatureComponent()->GetSkeleton());

	//* compute shader skinning *//
	SxavengerContent::SetSkinningPipeline(SxavengerSystem::GetMainThreadContext());

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gInputVertex",  referenceMesh_->input.GetVertex()->GetGPUVirtualAddress());
	parameter.SetAddress("gPalette",      cluster_.palette->GetGPUVirtualAddress());
	parameter.SetAddress("gInfluence",    cluster_.influence->GetGPUVirtualAddress());
	parameter.SetAddress("gInfo",         cluster_.info->GetGPUVirtualAddress());
	parameter.SetAddress("gOutputVertex", mesh_.vertex->GetGPUVirtualAddress());

	SxavengerContent::DispatchSkinning(SxavengerSystem::GetMainThreadContext(), parameter, cluster_.info->At(0));
}

void SkinnedMeshRendererComponent::BindIABuffer(const DirectXThreadContext* context) const {
	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = mesh_.vertex->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = referenceMesh_->input.GetIndex()->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

void SkinnedMeshRendererComponent::DrawCall(const DirectXThreadContext* context, uint32_t instanceCount) const {
	context->GetCommandList()->DrawIndexedInstanced(referenceMesh_->input.GetIndex()->GetIndexCount(), instanceCount, 0, 0, 0);
}

const TransformComponent* SkinnedMeshRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

void SkinnedMeshRendererComponent::CreateCluster() {
	const uint32_t kVertexSize = referenceMesh_->input.GetVertex()->GetSize();

	//* info
	cluster_.info = std::make_unique<DimensionBuffer<uint32_t>>();
	cluster_.info->Create(SxavengerSystem::GetDxDevice(), 1);
	cluster_.info->At(0) = kVertexSize;

	//* influence
	cluster_.influence = std::make_unique<DimensionBuffer<VertexInfluence>>();
	cluster_.influence->Create(SxavengerSystem::GetDxDevice(), kVertexSize);
	cluster_.influence->Fill(VertexInfluence{});

	const auto& skeleton      = GetArmatureComponent()->GetSkeleton();
	const uint32_t kJointSize = static_cast<uint32_t>(skeleton.GetJointSize());

	//* palette
	cluster_.palette = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	cluster_.palette->Create(SxavengerSystem::GetDxDevice(), static_cast<uint32_t>(kJointSize));

	//* inverse pose bind matrix
	cluster_.inverseBindPoseMatrices.resize(kJointSize);
	std::generate(cluster_.inverseBindPoseMatrices.begin(), cluster_.inverseBindPoseMatrices.end(), Matrix4x4::Identity);

	for (const auto& [name, jointWeight] : referenceMesh_->jointWeights) {
		if (!skeleton.jointMap.contains(name)) {
			continue; //!< この名前のJointは存在しない
		}

		uint32_t jointIndex = skeleton.jointMap.at(name);
		cluster_.inverseBindPoseMatrices[jointIndex] = jointWeight.inverseBindPoseMatrix;

		for (const auto& vertexWeight : jointWeight.vertexWeights) {
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

const ArmatureComponent* SkinnedMeshRendererComponent::GetArmatureComponent() const {
	//!< 同一階層のArmatureComponentを取得
	auto child = BaseComponent::GetBehaviour()->RequireParent()->FindChild(ArmatureComponent::kArmatureName);

	//!< ArmatureComponentを取得
	return child->RequireComponent<ArmatureComponent>();
}
