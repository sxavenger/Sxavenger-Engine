#include "ComponentHelper.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "ComponentStorage.h"
#include "Transform/TransformComponent.h"
#include "Transform/RectTransformComponent.h"
#include "Camera/CameraComponent.h"
#include "Armature/ArmatureComponent.h"
#include "MeshRenderer/MeshRendererComponent.h"
#include "MeshRenderer/SkinnedMeshRendererComponent.h"
#include "CanvasRenderer/SpriteRendererComponent.h"
#include "CanvasRenderer/TextRendererComponent.h"
#include "Light/Punctual/DirectionalLightComponent.h"
#include "Light/Punctual/PointLightComponent.h"
#include "Light/Punctual/SpotLightComponent.h"
#include "Light/Environment/SkyLightComponent.h"
#include "Collider/ColliderComponent.h"
#include "Audio/Audio3dListenerComponent.h"
#include "Audio/Audio3dSourceComponent.h"

//* entity
#include "../Entity/EntityBehaviourStorage.h"

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentHelper class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComponentHelper::BeginFrame() {
	sComponentStorage->ForEach<CameraComponent>([](CameraComponent* component) {
		component->SwapBuffer();
	});
}

void ComponentHelper::UpdateTransform() {
	sEntityBehaviourStorage->ForEachRoot([](EntityBehaviour* behaviour) {
		if (auto component = behaviour->GetComponent<TransformComponent>()) {
			component->UpdateMatrix();
		}

		if (auto component = behaviour->GetComponent<CameraComponent>()) {
			component->UpdateView();
			component->UpdateProj();
		}

		if (auto component = behaviour->GetComponent<RectTransformComponent>()) {
			component->UpdateMatrix();
		}
	});
}

void ComponentHelper::UpdateSkinning() {
	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([](SkinnedMeshRendererComponent* renderer) {
		renderer->Skinning();
	});
}

void ComponentHelper::UpdateParticle() {
	/*sComponentStorage->ForEachActive<EmitterComponent>([](EmitterComponent* component) {
		component->Update();
	});

	sComponentStorage->ForEachActive<ParticleComponent>([](ParticleComponent* component) {
		component->Update();
	});*/
}

void ComponentHelper::UpdateAudio3d() {

	std::optional<X3DAUDIO_LISTENER> listener = std::nullopt;

	sComponentStorage->ForEachActive<Audio3dListenerComponent>([&](Audio3dListenerComponent* component) {
		listener = component->GetListener(); //!< 最後に見つけたlistenerを使う
	});

	if (!listener.has_value()) {
		return;
	}

	sComponentStorage->ForEachActive<Audio3dSourceComponent>([&](Audio3dSourceComponent* component) {
		component->Update(listener.value());
	});
}

void ComponentHelper::RegisterComponents() {
	sComponentStorage->RegisterFactory<TransformComponent>();
	sComponentStorage->RegisterFactory<RectTransformComponent>();
	sComponentStorage->RegisterFactory<CameraComponent>();
	sComponentStorage->RegisterFactory<ArmatureComponent>();
	sComponentStorage->RegisterFactory<MeshRendererComponent>();
	sComponentStorage->RegisterFactory<SkinnedMeshRendererComponent>();
	sComponentStorage->RegisterFactory<SpriteRendererComponent>();
	sComponentStorage->RegisterFactory<TextRendererComponent>();
	sComponentStorage->RegisterFactory<DirectionalLightComponent>();
	sComponentStorage->RegisterFactory<PointLightComponent>();
	sComponentStorage->RegisterFactory<SpotLightComponent>();
	//sComponentStorage->RegisterFactory<ParticleComponent>();
	sComponentStorage->RegisterFactory<SkyLightComponent>();
	sComponentStorage->RegisterFactory<ColliderComponent>();
}

CameraComponent* ComponentHelper::GetCameraComponent(CameraComponent::Tag tag) {

	CameraComponent* camera = nullptr;

	sComponentStorage->ForEachActive<CameraComponent>([&](CameraComponent* component) {
		if (component->GetTag() == tag) {
			camera = component;
		}
	});

	return camera;
}
