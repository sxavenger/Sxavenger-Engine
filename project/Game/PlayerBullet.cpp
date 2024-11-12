#include "PlayerBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Performance.h>
#include <Engine/Game/SxavengerGame.h>

//* lib
#include <Lib/MyMath.h>

//* Game
#include <Game/Enemy.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init() {
	SetName("player bullet");
	ModelBehavior::renderingFlag_ = kBehaviorRender_Systematic | kBehaviorRender_Adaptive;

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model", "beam.obj");
	ModelBehavior::transform_.transform.scale = { 0.1f, 0.1f, 128.0f };

	Collider::typeId_       = kColliderType_NONE;
	Collider::targetTypeId_ = kColliderType_Enemy;

	Sxavenger::LoadTexture("resources/textures/energy.png");
	Sxavenger::LoadTexture("resources/textures/energyGauge.png");
}

void PlayerBullet::Term() {
}

void PlayerBullet::Update() {

	if (isShot_) {
		// 打ってる間は徐々に減る
		energy_ = std::max(energy_ - energyDecay_ * Performance::GetDeltaTime(s).time, 0.0f);

	} else {
		// 打ってない時は回復
		energy_ = std::min(energy_ + energyRecovery_ * Performance::GetDeltaTime(s).time, maxEnergy_);
	}
}

void PlayerBullet::Shoot(bool isShot, const Vector3f& position, const Vector3f& direction) {

	isShot_    = isShot;
	direction_ = direction;
	position_  = position;

	// エネルギー0なので打てない
	if (energy_ <= 0.0f) {
		isShot_ = false;
	}

	Collider::bounding_ = CollisionBoundings::Ray{
		.diff = direction
	};

	Collider::position_ = position_;

	ModelBehavior::transform_.transform.translate = position_;
	ModelBehavior::transform_.transform.rotate    = ToQuaternion(CalculateEuler(direction_));
	rotate_                                       *= MakeAxisAngle({0.0f, 0.0f, 1.0f}, 0.02f);
	ModelBehavior::transform_.transform.rotate    *= rotate_;
	ModelBehavior::transform_.UpdateMatrix();



	if (isShot_) {
		Collider::typeId_ = kColliderType_PlayerBullet;

	} else {
		Collider::typeId_ = kColliderType_NONE;
	}
}

void PlayerBullet::SetAttributeImGui() {

	ImGui::Text("energy parameter");
	ImGui::DragFloat("max energy",      &maxEnergy_, 0.01f, 0.0f, 128.0f);
	ImGui::SliderFloat("energy",        &energy_, 0.0f, maxEnergy_);
	ImGui::DragFloat("energy decay",    &energyDecay_, 0.01f, 0.0f, 64.0f);
	ImGui::DragFloat("energy recovery", &energyRecovery_, 0.01f, 0.0f, 64.0f);

	static float step = 8.0f;
	ImGui::InputScalarN("margin", ImGuiDataType_Float, &guagePosition_.x, 2, &step);
}

void PlayerBullet::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {

	SxavengerGame::DrawSpriteClip(
		guagePosition_, { 24.0f, std::lerp(0.0f, -280.0f, energy_ / maxEnergy_) },
		{ 0.0f, 0.0f }, { 1.0f, std::lerp(-1.0f, 0.0f, energy_ / maxEnergy_) },
		Sxavenger::GetTextureHandleGPU("resources/textures/energy.png")
	);

	SxavengerGame::DrawSprite(
		guagePosition_, { 24.0f, -280.0f },
		Sxavenger::GetTextureHandleGPU("resources/textures/energyGauge.png")
	);
}

void PlayerBullet::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {

	if (!isShot_) {
		return;
	}

	ModelBehavior::DrawSystematic(camera);
}

void PlayerBullet::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {
	if (score_ != nullptr) {
		auto enemy = dynamic_cast<Enemy*>(other);
		score_->AddScore(enemy->GetPoint(), enemy->GetModel());
	}

}
