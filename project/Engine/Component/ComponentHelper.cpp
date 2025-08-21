#include "ComponentHelper.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "Components/ComponentStorage.h"
#include "Components/Transform/TransformComponent.h"
#include "Components/Camera/CameraComponent.h"
#include "Components/Armature/ArmatureComponent.h"
#include "Components/MeshRenderer/MeshRendererComponent.h"
#include "Components/MeshRenderer/SkinnedMeshRendererComponent.h"
#include "Components/Light/Punctual/DirectionalLightComponent.h"
#include "Components/Light/Punctual/PointLightComponent.h"
//#include "Components/Light/Rect/RectLightComponent.h" -> FIXME
#include "Components/Particle/ParticleComponent.h"
#include "Components/Light/Environment/SkyLightComponent.h"
#include "Components/Collider/ColliderComponent.h"

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentHelper class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComponentHelper::UpdateTransform() {
	sMonoBehaviourContainer->ForEach(std::execution::seq, [](MonoBehaviour* behaviour) {
		if (auto component = behaviour->GetComponent<TransformComponent>()) {
			component->UpdateMatrix();
		}

		if (auto component = behaviour->GetComponent<CameraComponent>()) {
			component->UpdateView();
		}
	});
}

void ComponentHelper::UpdateSkinning() {
	sComponentStorage->ForEach<SkinnedMeshRendererComponent>([](SkinnedMeshRendererComponent* renderer) {
		renderer->Skinning();
	});
}

void ComponentHelper::UpdateParticle() {
	sComponentStorage->ForEach<ParticleComponent>([](ParticleComponent* particle) {
		particle->Update();
	});
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateMonoBehaviour() {
	return std::make_unique<MonoBehaviour>();
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateTransformBehaviour() {
	std::unique_ptr<MonoBehaviour> root = std::make_unique<MonoBehaviour>();
	root->SetName("transform");
	root->AddComponent<TransformComponent>();
	return root;
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateCameraMonoBehaviour() {
	std::unique_ptr<MonoBehaviour> root = std::make_unique<MonoBehaviour>();
	root->SetName("camera");

	root->AddComponent<TransformComponent>();
	root->AddComponent<CameraComponent>()->SetTag(CameraComponent::Tag::GameCamera);

	return root;
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateDirectionalLightMonoBehaviour() {
	std::unique_ptr<MonoBehaviour> root = std::make_unique<MonoBehaviour>();
	root->SetName("directional light");

	root->AddComponent<TransformComponent>();
	root->AddComponent<DirectionalLightComponent>();

	return root;
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreatePointLightMonoBehaviour() {
	std::unique_ptr<MonoBehaviour> root = std::make_unique<MonoBehaviour>();
	root->SetName("point light");

	root->AddComponent<TransformComponent>();
	root->AddComponent<PointLightComponent>();

	return root;
}

void ComponentHelper::CreateStaticMeshBehaviour(MonoBehaviour* root, const std::shared_ptr<UContentModel>& model) {

	// modelのnodeから階層構造を作成
	const std::function<void(MonoBehaviour*, const BornNode&)> function = [&](MonoBehaviour* parent, const BornNode& node) {
		auto child = std::make_unique<MonoBehaviour>();
		child->SetName(node.name);

		// transform component
		auto transform = child->AddComponent<TransformComponent>();
		transform->GetTransform() = node.transform;

		if (node.meshIndices.size() == 1) {
			// componentが1つの場合, そのままMeshRendererComponentを追加
			const uint32_t meshIndex = node.meshIndices.front();

			auto mesh     = sUAssetStorage->GetAsset<UAssetMesh>(model->GetMeshId(meshIndex));
			auto material = sUAssetStorage->GetAsset<UAssetMaterial>(model->GetMeshToMaterialId(meshIndex));

			auto renderer = child->AddComponent<MeshRendererComponent>();
			renderer->SetMesh(mesh->GetId());
			renderer->SetMaterial(material->GetId());

		} else {
			// componentが一つしか付けられないので苦肉の策
			for (auto& meshIndex : node.meshIndices) {

				auto mesh     = sUAssetStorage->GetAsset<UAssetMesh>(model->GetMeshId(meshIndex));
				auto material = sUAssetStorage->GetAsset<UAssetMaterial>(model->GetMeshToMaterialId(meshIndex));

				auto behaviour = std::make_unique<MonoBehaviour>();
				behaviour->SetName(mesh->GetName());

				//!< transform component の追加
				behaviour->AddComponent<TransformComponent>();

				//!< mesh renderer component の追加
				auto renderer = behaviour->AddComponent<MeshRendererComponent>();
				renderer->SetMesh(mesh->GetId());
				renderer->SetMaterial(material->GetId());

				child->AddChild(std::move(behaviour));
			}
		}

		// 再帰的に登録
		for (const auto& childNode : node.children) {
			function(child.get(), childNode);
		}

		parent->AddChild(std::move(child));
	};

	function(root, model->GetRoot());
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateStaticMeshBehaviour(const std::shared_ptr<UContentModel>& model) {
	model->WaitComplete();

	auto root = std::make_unique<MonoBehaviour>();
	root->SetName(model->GetFilepath().stem().string());

	ComponentHelper::CreateStaticMeshBehaviour(root.get(), model);

	return std::move(root);
}

void ComponentHelper::CreateSkinnedMeshBehaviour(MonoBehaviour* root, const std::shared_ptr<UContentModel>& model) {
	model->WaitComplete();

	{ //!< ArmatureComponent
		auto armature = std::make_unique<MonoBehaviour>();
		armature->SetName(ArmatureComponent::kArmatureName);
		armature->SetRenamable(false);
		armature->AddComponent<ArmatureComponent>()->SetSkeleton(model->GetSkeletonId());
		root->AddChild(std::move(armature));
	}

	// meshの登録
	for (size_t i = 0; i < model->GetMeshCount(); ++i) {

		auto mesh     = sUAssetStorage->GetAsset<UAssetMesh>(model->GetMeshId(i));
		auto material = sUAssetStorage->GetAsset<UAssetMaterial>(model->GetMeshToMaterialId(i));

		auto child = std::make_unique<MonoBehaviour>();
		child->SetName(mesh->GetName());

		// HACK: ptrでの操作ではなく, childを直接操作すること.
		auto ptr = child.get();
		root->AddChild(std::move(child));

		//!< transform component の追加
		ptr->AddComponent<TransformComponent>();

		//!< skinned mesh renderer component の追加
		auto renderer = ptr->AddComponent<SkinnedMeshRendererComponent>();
		renderer->CreateMesh(mesh->GetId());
		renderer->SetMaterial(material->GetId());
	}
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateSkinnedMeshBehaviour(const std::shared_ptr<UContentModel>& model) {
	model->WaitComplete();

	auto root = std::make_unique<MonoBehaviour>();
	root->SetName(model->GetFilepath().stem().string());

	ComponentHelper::CreateSkinnedMeshBehaviour(root.get(), model);

	return std::move(root);
}


void ComponentHelper::ApplyAnimation(MonoBehaviour* behaviour, const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop) {
	auto child = behaviour->FindChild(ArmatureComponent::kArmatureName);
	child->GetComponent<ArmatureComponent>()->UpdateAnimation(animation, time, isLoop);
}

void ComponentHelper::ApplyAnimationTransition(MonoBehaviour* behaviour, const Animation& animationA, TimePointd<TimeUnit::second> timeA, bool isLoopA, const Animation& animationB, TimePointd<TimeUnit::second> timeB, bool isLoopB, float transitionT) {
	auto child = behaviour->FindChild(ArmatureComponent::kArmatureName);
	child->GetComponent<ArmatureComponent>()->TransitionAnimation(animationA, timeA, isLoopA, animationB, timeB, isLoopB, transitionT);
}

void ComponentHelper::RegisterComponents() {
	sComponentStorage->RegisterFactory<TransformComponent>();
	sComponentStorage->RegisterFactory<CameraComponent>();
	sComponentStorage->RegisterFactory<ArmatureComponent>();
	sComponentStorage->RegisterFactory<MeshRendererComponent>();
	sComponentStorage->RegisterFactory<SkinnedMeshRendererComponent>();
	sComponentStorage->RegisterFactory<DirectionalLightComponent>();
	sComponentStorage->RegisterFactory<PointLightComponent>();
	sComponentStorage->RegisterFactory<ParticleComponent>();
	sComponentStorage->RegisterFactory<SkyLightComponent>();
	sComponentStorage->RegisterFactory<ColliderComponent>();
}
