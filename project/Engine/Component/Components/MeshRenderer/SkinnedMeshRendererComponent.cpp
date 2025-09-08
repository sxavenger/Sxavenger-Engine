#include "SkinnedMeshRendererComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Preview/Asset/UAssetStorage.h>
#include <Engine/Preview/Content/UContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InputSkinnedMesh structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRendererComponent::InputSkinnedMesh::Create(const DirectXQueueContext* context, const std::shared_ptr<UAssetMesh>& mesh) {
	CreateVetex(mesh);
	CreateBottomLevelAS(context, mesh);
	isCreateMesh = true;
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::UpdateBottomLevelAS(const DirectXQueueContext* context) {
	Exception::Assert(isCreateMesh, "mesh is not created.");
	bottomLevelAS.Update(context->GetDxCommand());
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::CreateVetex(const std::shared_ptr<UAssetMesh>& mesh) {
	vertex = std::make_unique<VertexUnorderedDimensionBuffer<MeshVertexData>>();
	vertex->Create(SxavengerSystem::GetDxDevice(), mesh->GetInputVertex()->GetSize());
}

void SkinnedMeshRendererComponent::InputSkinnedMesh::CreateBottomLevelAS(const DirectXQueueContext* context, const std::shared_ptr<UAssetMesh>& mesh) {
	D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
	desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	desc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	desc.Triangles.VertexBuffer.StartAddress  = vertex->GetGPUVirtualAddress();
	desc.Triangles.VertexBuffer.StrideInBytes = vertex->GetStride();
	desc.Triangles.VertexCount                = vertex->GetSize();
	desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.Triangles.IndexBuffer                = mesh->GetInputIndex()->GetGPUVirtualAddress();
	desc.Triangles.IndexCount                 = mesh->GetInputIndex()->GetIndexCount();
	desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

	bottomLevelAS.Build(SxavengerSystem::GetDxDevice(), context->GetDxCommand(), desc);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshRendererComponent::CreateMesh(const Uuid& referenceMesh) {
	// 参照先のmeshを保持
	referenceMesh_ = referenceMesh;

	mesh_.Create(SxavengerSystem::GetDirectQueueContext(), referenceMesh_.Get());
	CreateCluster();
}

void SkinnedMeshRendererComponent::CreateMesh(const std::shared_ptr<UAssetMesh>& referenceMesh) {
	// 参照先のmeshを保持
	referenceMesh_ = referenceMesh;

	mesh_.Create(SxavengerSystem::GetDirectQueueContext(), referenceMesh_.Get());
	CreateCluster();
}

void SkinnedMeshRendererComponent::Skinning() {
	//* cluster update *//
	cluster_.UpdatePalette(GetArmatureComponent()->GetSkeleton());

	//* compute shader skinning *//
	SxavengerContent::SetSkinningPipeline(SxavengerSystem::GetDirectQueueContext());

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gInputVertex",  referenceMesh_.Require()->GetInputVertex()->GetGPUVirtualAddress());
	parameter.SetAddress("gPalette",      cluster_.palette->GetGPUVirtualAddress());
	parameter.SetAddress("gInfluence",    cluster_.influence->GetGPUVirtualAddress());
	parameter.SetAddress("gInfo",         cluster_.info->GetGPUVirtualAddress());
	parameter.SetAddress("gOutputVertex", mesh_.vertex->GetGPUVirtualAddress());

	SxavengerContent::DispatchSkinning(SxavengerSystem::GetDirectQueueContext(), parameter, cluster_.info->At(0));
}

void SkinnedMeshRendererComponent::Update(const DirectXQueueContext* context) {
	mesh_.UpdateBottomLevelAS(context);
}

void SkinnedMeshRendererComponent::BindIABuffer(const DirectXQueueContext* context) const {
	auto commandList = context->GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = mesh_.vertex->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = referenceMesh_.Require()->GetInputIndex()->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

void SkinnedMeshRendererComponent::DrawCall(const DirectXQueueContext* context, uint32_t instanceCount) const {
	context->GetCommandList()->DrawIndexedInstanced(referenceMesh_.Require()->GetInputIndex()->GetIndexCount(), instanceCount, 0, 0, 0);
}

std::shared_ptr<UAssetMaterial> SkinnedMeshRendererComponent::GetMaterial() const {
	return material_.Require();
}

const TransformComponent* SkinnedMeshRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json SkinnedMeshRendererComponent::PerseToJson() const {
	json data = json::object();

	data["referenceMesh"] = referenceMesh_.Serialize();
	data["material"]      = material_.Serialize();
	data["mask"]          = mask_.Get();

	return data;
}

void SkinnedMeshRendererComponent::InputJson(const json& data) {

	Uuid referenceMesh = Uuid::Deserialize(data["referenceMesh"].get<std::string>());
	Uuid material = Uuid::Deserialize(data["material"].get<std::string>());

	// referenceMesh, materialのuuidが存在しない場合は, tableから読み込み

	if (!sUAssetStorage->Contains<UAssetMesh>(referenceMesh)) {
		const auto& filepath = sUAssetStorage->GetFilepath(referenceMesh);
		sUContentStorage->Import<UContentModel>(filepath);
	}

	if (!sUAssetStorage->Contains<UAssetMesh>(material)) {
		const auto& filepath = sUAssetStorage->GetFilepath(material);
		sUContentStorage->Import<UContentModel>(filepath);
	}

	CreateMesh(referenceMesh);
	material_ = material;

	mask_ = static_cast<MeshInstanceMask>(data["mask"].get<uint8_t>());
	
}

void SkinnedMeshRendererComponent::CreateCluster() {
	const uint32_t kVertexSize = referenceMesh_.Require()->GetInputVertex()->GetSize();

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

	for (const auto& [name, jointWeight] : referenceMesh_.Require()->GetJointWeights()) {
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
