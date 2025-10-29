#include "CollectibleItems.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollectibleItems class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollectibleItems::Load() {
}

void CollectibleItems::Awake() {
	MonoBehaviour::SetName("collectible items");

	for (size_t i = 0; i < cubes_.size(); ++i) {
		std::unique_ptr<CollectibleCube> cube = std::make_unique<CollectibleCube>();
		cubes_[i] = cube.get();
		MonoBehaviour::AddChild(std::move(cube));

		cubes_[i]->Init();
	}

}

void CollectibleItems::Start() {
	for (size_t i = 0; i < cubes_.size(); ++i) {
		auto transform = cubes_[i]->GetComponent<TransformComponent>();
		transform->GetTransform().translate = positions_.Get()[i];
	}
}

void CollectibleItems::Update() {
	for (const auto& cube : cubes_) {
		cube->Update();
	}
}

void CollectibleItems::Inspectable() {
	if (ImGui::Button("Save Positions")) {
		for (size_t i = 0; i < cubes_.size(); ++i) {
			auto transform = cubes_[i]->GetComponent<TransformComponent>();
			positions_.Get()[i] = transform->GetTransform().translate;
		}

		positions_.Save();
	}
}

bool CollectibleItems::IsCollected() const {
	bool isActive = false;

	for (const auto& cube : cubes_) {
		isActive |= cube->IsActive();
	};

	return !isActive;
}
