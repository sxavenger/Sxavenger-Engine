#include "EmissiveCubes.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/ComponentHelper.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EmissiveCubes class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EmissiveCubes::Load() {
	model_ = sUContentStorage->Import<UContentModel>("assets/models/primitive/cube_skeleton.obj");
}

void EmissiveCubes::Awake() {

	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();

	MonoBehaviour::SetName("emissive cubes");

	for (size_t i = 0; i < cubes_.size(); ++i) {
		cubes_[i] = std::make_unique<MonoBehaviour>();
		cubes_[i]->SetName("emissive cube");
	
		ComponentHelper::CreateStaticMeshBehaviour(cubes_[i].get(), model_.Get());

		ComponentHelper::DetachBehaviourMaterial(cubes_[i].get());
		ComponentHelper::ModifyBehaviourMaterial(cubes_[i].get(), [this, i](UAssetMaterial* material) {
			material->SetMode(UAssetMaterial::Mode::Emissive);
			material->GetBuffer().albedo.SetValue(colors_[i]);
			//material->GetBuffer().transparency.SetValue(0.8f);
		});

		cubes_[i]->AddComponent<TransformComponent>();
		cubes_[i]->AddComponent<ColliderComponent>();

		MonoBehaviour::AddChild(cubes_[i].get());
	}
}

void EmissiveCubes::Start() {
	for (size_t i = 0; i < cubes_.size(); ++i) {
		auto transform = cubes_[i]->GetComponent<TransformComponent>();
		transform->scale = { 0.4f, 0.4f, 0.4f };
		transform->translate = {
			5.0f * std::cos(kTau * static_cast<float>(i) / static_cast<float>(cubes_.size())),
			0.4f,
			5.0f * std::sin(kTau * static_cast<float>(i) / static_cast<float>(cubes_.size()))
		};

		auto collider = cubes_[i]->GetComponent<ColliderComponent>();
		collider->SetColliderBoundingSphere({ .radius = 0.5f });
		collider->SetTag("cube");
	}
}

void EmissiveCubes::Update() {
	if (camera_ == nullptr) {
		return;
	}

	if (keyboard_->IsPress(KeyId::KEY_F)) { //!< インタラクト
		std::optional<std::pair<size_t, float>> nearest = std::nullopt;

		static const float range = 4.0f;

		// 最も近いキューブを探す
		for (size_t i = 0; i < cubes_.size(); ++i) {

			auto transform = cubes_[i]->GetComponent<TransformComponent>();
			float distance = Vector3f::Distance(transform->GetPosition(), camera_->GetPosition());

			if (distance > range) {
				continue; //!< 遠すぎる
			}

			if (nearest.has_value()) {
				if (nearest->second > distance) {
					nearest = std::make_pair(i, distance);
				}
			} else {
				nearest = std::make_pair(i, distance);
			}

		}

		if (nearest.has_value()) {
			auto transform = cubes_[nearest->first]->GetComponent<TransformComponent>();
			transform->translate = camera_->GetPosition() + camera_->GetForward() * range / 2.0f;
		}
	}

}
