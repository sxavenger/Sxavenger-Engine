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

////////////////////////////////////////////////////////////////////////////////////////////
// CollectibleItems class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollectibleItems::Load() {
	model_ = sUContentStorage->Import<UContentModel>("assets/models/primitive/cube_skeleton.obj");
}

void CollectibleItems::Awake() {
	MonoBehaviour::SetName("collectible items");

	for (size_t i = 0; i < cubes_.size(); ++i) {
		cubes_[i] = std::make_unique<MonoBehaviour>();
		cubes_[i]->SetName("collectible item");

		ComponentHelper::CreateStaticMeshBehaviour(cubes_[i].get(), model_.Get());

		ComponentHelper::DetachBehaviourMaterial(cubes_[i].get());
		ComponentHelper::ModifyBehaviourMaterial(cubes_[i].get(), [this, i](UAssetMaterial* material) {
			material->SetMode(UAssetMaterial::Mode::Opaque);
			material->GetBuffer().albedo.SetValue(colors_[i]);
		});

		cubes_[i]->AddComponent<TransformComponent>();

		auto collider = cubes_[i]->AddComponent<ColliderComponent>();
		collider->SetColliderBoundingSphere();
		collider->SetTag("item");

		auto process = cubes_[i]->AddComponent<PostProcessLayerComponent>();
		process->AddPostProcess<PostProcessChromaticAberration>();
		process->AddPostProcess<PostProcessRadialBlur>();

		process->SetTag(PostProcessLayerComponent::Tag::Volume);

		MonoBehaviour::AddChild(cubes_[i].get());
	}

}

void CollectibleItems::Start() {
	for (size_t i = 0; i < cubes_.size(); ++i) {
		auto transform = cubes_[i]->GetComponent<TransformComponent>();

		// TODO: 外部ファイルから読み込み
		transform->scale = { 0.4f, 0.4f, 0.4f };
		transform->translate = {
			5.0f * std::cos(kTau * static_cast<float>(i) / static_cast<float>(cubes_.size())),
			0.4f,
			5.0f * std::sin(kTau * static_cast<float>(i) / static_cast<float>(cubes_.size()))
		};
	}
}

void CollectibleItems::Update() {
}
