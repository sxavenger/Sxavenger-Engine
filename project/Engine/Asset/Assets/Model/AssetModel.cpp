#include "AssetModel.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../../AssetStorage.h"
#include "../../Observer/AssetObserver.h"

//* engine
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetModel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetModel::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {

	uint32_t option = Model::GetDefaultAssimpOption();

	if (param_.has_value()) { //!< parameter override
		option = std::any_cast<uint32_t>(param_);
		 
	}

	model_.Load(BaseAsset::GetFilepath(), option);
	CreateMaterials();
}

void AssetModel::ShowInspector() {
}

void AssetModel::UpdateMaterials() {
	for (size_t i = 0; i < materials_.size(); ++i) {
		UpdateMaterial(model_.GetMaterials()[i], materials_[i].get());
	}
}

std::unique_ptr<Material> AssetModel::CreateMaterial(const Model::AssimpMaterial& material) {
	std::unique_ptr<Material> result = std::make_unique<Material>();
	result->CreateBuffer();

	UpdateMaterial(material, result.get());

	return result;
}

void AssetModel::CreateMaterials() {
	const auto& materias = model_.GetMaterials();
	materials_.resize(materias.size());

	for (size_t i = 0; i < materias.size(); ++i) {
		materials_[i] = CreateMaterial(materias[i]);
	}
}

void AssetModel::UpdateMaterial(const Model::AssimpMaterial& material, Material* result) const {
	// todo: load中textureを変更

	auto& parameter = result->GetBuffer();

	// diffuse
	if (material.GetTexture(Model::AssimpMaterial::TextureType::Diffuse).has_value()) {
		auto observer = sAssetStorage->TryImport<AssetTexture>(material.GetTexture(Model::AssimpMaterial::TextureType::Diffuse).value());
		auto ptr      = observer.Acquire();

		parameter.albedo.SetTexture(ptr->GetDescriptorSRV().GetIndex());
		parameter.transparency.SetTexture(ptr->GetDescriptorSRV().GetIndex());

	} else {
		parameter.albedo.SetValue(material.color);
		parameter.transparency.SetValue(1.0f);
	}

	// bump
	if (material.GetTexture(Model::AssimpMaterial::TextureType::Bump).has_value()) {
		auto observer = sAssetStorage->TryImport<AssetTexture>(material.GetTexture(Model::AssimpMaterial::TextureType::Bump).value(), Texture::Option{ Texture::Encoding::Intensity });
		auto ptr      = observer.Acquire();
		parameter.normal.SetTexture(ptr->GetDescriptorSRV().GetIndex());
		
	}

	// roughness
	if (material.GetTexture(Model::AssimpMaterial::TextureType::Roughness).has_value()) {
		auto observer = sAssetStorage->TryImport<AssetTexture>(material.GetTexture(Model::AssimpMaterial::TextureType::Roughness).value(), Texture::Option{ Texture::Encoding::Intensity });
		auto ptr      = observer.Acquire();
		parameter.properties.roughness.SetTexture(ptr->GetDescriptorSRV().GetIndex());

	} else {
		parameter.properties.roughness.SetValue(material.roughness);
	}

	// metallic
	if (material.GetTexture(Model::AssimpMaterial::TextureType::Metallic).has_value()) {
		auto observer = sAssetStorage->TryImport<AssetTexture>(material.GetTexture(Model::AssimpMaterial::TextureType::Metallic).value(), Texture::Option{ Texture::Encoding::Intensity });
		auto ptr      = observer.Acquire();
		parameter.properties.metallic.SetTexture(ptr->GetDescriptorSRV().GetIndex());

	} else {
		parameter.properties.metallic.SetValue(material.metallic);
	}

	// ambient occlusion
	if (material.GetTexture(Model::AssimpMaterial::TextureType::AmbientOcclusion).has_value()) {
		auto observer = sAssetStorage->TryImport<AssetTexture>(material.GetTexture(Model::AssimpMaterial::TextureType::AmbientOcclusion).value(), Texture::Option{ Texture::Encoding::Intensity });
		auto ptr      = observer.Acquire();
		parameter.properties.ao.SetTexture(ptr->GetDescriptorSRV().GetIndex());
	}

}

void AssetModel::CreateStaticMeshBehaviour(MonoBehaviour* root) {
	for (auto& mesh : model_.GetMeshes()) {
		auto child = std::make_unique<MonoBehaviour>();
		child->SetName(mesh.name);

		// transform component
		auto transform = child->AddComponent<TransformComponent>();
		transform->CreateBuffer();

		if (model_.GetSkeleton().ContainsJoint(mesh.name)) {
			transform->GetTransform() = model_.GetSkeleton().GetJoint(mesh.name).transform;
		}

		// renderer component
		auto renderer = child->AddComponent<MeshRendererComponent>();
		renderer->SetMesh(&mesh.input);
		renderer->SetMaterial(materials_[mesh.materialIndex.value()].get());

		root->AddChild(std::move(child));
	}
}

std::unique_ptr<MonoBehaviour> AssetModel::CreateStaticMeshBehaviour(const std::string& name) {
	auto root = std::make_unique<MonoBehaviour>();
	root->SetName(name);

	CreateStaticMeshBehaviour(root.get());

	return std::move(root);
}

void AssetModel::CreateStaticNodeMeshBehaviour(MonoBehaviour* root) {
	// node component || mesh rendererの登録
	const std::function<void(MonoBehaviour*, const BornNode&)> function = [&](MonoBehaviour* parent, const BornNode& node) {
		auto child = std::make_unique<MonoBehaviour>();
		child->SetName(node.name);

		// transform component
		auto transform = child->AddComponent<TransformComponent>();
		transform->GetTransform() = node.transform;

		for (auto& meshIndex : node.meshIndices) {
			auto behaviour = std::make_unique<MonoBehaviour>();
			behaviour->SetName(model_.GetMesh(meshIndex).name);

			// componentが一つしか付けられないので苦肉の策
			behaviour->AddComponent<TransformComponent>();

			auto renderer = behaviour->AddComponent<MeshRendererComponent>();
			renderer->SetMesh(&model_.GetMesh(meshIndex).input);
			renderer->SetMaterial(materials_[model_.GetMesh(meshIndex).materialIndex.value()].get());

			child->AddChild(std::move(behaviour));
		}

		// 再帰的に登録
		for (const auto& childNode : node.children) {
			function(child.get(), childNode);
		}

		parent->AddChild(std::move(child));
	};

	function(root, model_.GetRoot());
}

std::unique_ptr<MonoBehaviour> AssetModel::CreateStaticNodeMeshBehaviour(const std::string& name) {
	auto root = std::make_unique<MonoBehaviour>();
	root->SetName(name);

	CreateStaticNodeMeshBehaviour(root.get());

	return std::move(root);
}

void AssetModel::CreateSkinnedMeshBehaviour(MonoBehaviour* root) {
	{ //!< ArmatureComponent
		auto armature = std::make_unique<MonoBehaviour>();
		armature->SetName(ArmatureComponent::kArmatureName);
		armature->SetRenamable(false);
		armature->AddComponent<ArmatureComponent>()->SetSkeleton(model_.GetSkeleton());
		root->AddChild(std::move(armature));
	}

	// meshの登録
	for (auto& mesh : model_.GetMeshes()) {
		auto child = std::make_unique<MonoBehaviour>();
		auto ptr = child.get();
		root->AddChild(std::move(child));

		ptr->SetName(mesh.name);

		// transform component
		auto transform = ptr->AddComponent<TransformComponent>();
		transform->CreateBuffer();

		// renderer component
		auto renderer = ptr->AddComponent<SkinnedMeshRendererComponent>();
		renderer->CreateMesh(&mesh);
		renderer->SetMaterial(materials_[mesh.materialIndex.value()].get());
	}
}

std::unique_ptr<MonoBehaviour> AssetModel::CreateSkinnedMeshBehaviour(const std::string& name) {
	auto root = std::make_unique<MonoBehaviour>();
	root->SetName(name);

	CreateSkinnedMeshBehaviour(root.get());

	return std::move(root);
}
