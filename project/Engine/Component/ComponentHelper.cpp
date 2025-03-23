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
