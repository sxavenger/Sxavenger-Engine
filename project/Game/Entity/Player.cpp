#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {

	model_ = SxavengerAsset::ImportModel("asset/model/demo/cube.obj");
	model_->AsyncLoad();
	SxavengerSystem::PushTask(model_);
	model_->WaitCompleted();

	ModelBehavior::model_ = model_.get();
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	SetName("player");

	weapon_ = std::make_unique<Weapon>();
	weapon_->SetParent(this);
	weapon_->Init();
	SetChild(weapon_.get());
}

void Player::Term() {
}

void Player::Update() {
	MoveWithKeyboard();
	AttackWithKeyboard();
}

void Player::SetAttributeImGui() {
	ImGui::DragFloat("speed", &speed_, 0.01f, 0.0f, 128.0f);
}

void Player::MoveWithKeyboard() {

	Vector2f direction = {};

	if (SxavengerSystem::IsPressKey(KeyId::KEY_W)) {
		direction.y += 1.0f;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_S)) {
		direction.y -= 1.0f;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_D)) {
		direction.x += 1.0f;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_A)) {
		direction.x -= 1.0f;
	}

	direction = Normalize(direction);

	velocity_ = Vector3f{ direction.x, 0.0f, direction.y } * speed_;

	TransformComponent::GetTransform().translate
		+= velocity_ * Performance::GetDeltaTime<TimeUnit::s>().time;

	TransformComponent::UpdateMatrix();
}

void Player::AttackWithKeyboard() {
	if (weapon_->IsActive()) {

		attackTime_.SubtractDeltaTime();

		if (attackTime_.time <= 0.0f) {
			weapon_->SetActive(false);
		}

		return;
	}

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_SPACE)) {
		//weapon_->GetTransform() = TransformComponent::GetTransform();
		attackTime_ = { 1.0f };
		weapon_->SetActive(true);
	}

	weapon_->UpdateMatrix();
}
