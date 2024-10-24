#include "PlayerBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>


////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init() {
	SetName("player bullet");
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
}

void PlayerBullet::SetAttributeImGui() {

	ImGui::Text("energy parameter");
	ImGui::DragFloat("max energy",      &maxEnergy_, 0.01f, 0.0f, 128.0f);
	ImGui::SliderFloat("energy",        &energy_, 0.0f, maxEnergy_);
	ImGui::DragFloat("energy decay",    &energyDecay_, 0.01f, 0.0f, 64.0f);
	ImGui::DragFloat("energy recovery", &energyRecovery_, 0.01f, 0.0f, 64.0f);

}

void PlayerBullet::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {

	if (!isShot_) {
		return;
	}

	SxavengerGame::DrawLine(
		position_, position_ + direction_ * 12.0f, ToColor4f(0x00FAFAFF)
	);

}
