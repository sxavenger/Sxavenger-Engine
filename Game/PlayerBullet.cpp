#include "PlayerBullet.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/System/Performance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBullet class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init() {
	SetName("player bullet");
}

void PlayerBullet::Term() {
}

void PlayerBullet::Update() {

	if (Sxavenger::IsPressKey(DIK_SPACE)) { //!< debug
		energy_ = std::max(energy_ - energyLossRate_ * Performance::GetDeltaTime(s).time, 0.0f);

	} else {
		energy_ = std::min(energy_ + energyRecoveryRate_ * Performance::GetDeltaTime(s).time, maxEnergy_);
	}

}

void PlayerBullet::SetAttributeImGui() {
	ImGui::Text("energy parameter");
	ImGui::DragFloat("max energy",           &maxEnergy_, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("energy loss rate",     &energyLossRate_, 0.01f, 0.0f, 128.0f);
	ImGui::DragFloat("energy recovery rate", &energyRecoveryRate_, 0.01f, 0.0f, 128.0f);
	ImGui::SliderFloat("energy",             &energy_, 0.0f, maxEnergy_);
}
