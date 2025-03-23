#include "ComponentHelper.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "Components/ComponentStorage.h"
#include "Components/Transform/TransformComponent.h"
#include "Components/Camera/CameraComponent.h"
#include "Components/Armature/ArmatureComponent.h"
#include "Components/MeshRenderer/SkinnedMeshRendererComponent.h"
#include "Components/Light/DirectionalLightComponent.h"

//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Asset/Observer/AssetObserver.h>

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

void ComponentHelper::ApplyAnimation(MonoBehaviour* behaviour, const Animation& animation, TimePointf<TimeUnit::second> time, bool isLoop) {
	auto child = behaviour->FindChild(ArmatureComponent::kArmatureName);
	child->GetComponent<ArmatureComponent>()->UpdateAnimation(animation, time, isLoop);
}

void ComponentHelper::UpdateSkinning() {
	sComponentStorage->ForEach<SkinnedMeshRendererComponent>([](SkinnedMeshRendererComponent* renderer) {
		renderer->Skinning();
	});
}

std::unique_ptr<MonoBehaviour> ComponentHelper::CreateModelBehaviour(const std::filesystem::path& filepath) {
	AssetObserver<AssetModel> observer = SxavengerAsset::TryImport<AssetModel>(filepath);
	return observer.WaitGet()->CreateStaticMeshBehaviour(filepath.stem().generic_string());
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
