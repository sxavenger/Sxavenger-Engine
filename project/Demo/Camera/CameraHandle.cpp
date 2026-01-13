#include "CameraHandle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>

SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// CameraHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

CameraHandle::CameraHandle() {
	keyboard_ = SxxEngine::System::GetInput()->GetKeyboardInput();
	mouse_    = SxxEngine::System::GetInput()->GetMouseInput();

	gamepad0_ = SxxEngine::System::GetInput()->GetGamepadInput(0);
}

void CameraHandle::Update() {
	direction_ = {};

	Vector2f keyboardDirection = GetKeyboardDirection();
	Vector2f mouseDirection    = /*GetMouseDirection();*/ {};
	Vector2f gamepadDirection  = GetGamepadDirection();

	float keyboardLength = keyboardDirection.Length();
	float mouseLength    = mouseDirection.Length();
	float gamepadLength  = gamepadDirection.Length();

	//!< 最も大きい入力を採用
	if (keyboardLength >= mouseLength && keyboardLength >= gamepadLength) {
		direction_ = keyboardDirection;

	} else if (mouseLength >= keyboardLength && mouseLength >= gamepadLength) {
		direction_ = mouseDirection; //!< TODO: 正規化

	} else if (gamepadLength >= keyboardLength && gamepadLength >= mouseLength) {
		direction_ = gamepadDirection;
	}
}

Vector2f CameraHandle::GetKeyboardDirection() {
	return keyboard_->GetDirection(aroundKeys_);
}

Vector2f CameraHandle::GetMouseDirection() {
	if (!mouse_->IsPress(aroundMouseButton_)) {
		return {};
	}

	// マウスの制御
	mouse_->SetPosition(static_cast<Vector2i>(Configuration::GetConfig().resolution) / 2);
	mouse_->SetShowCursor(false);

	Vector2f delta = mouse_->GetDeltaPosition();
	return delta * sensitivity_;
	// TODO: 正規化
}

Vector2f CameraHandle::GetGamepadDirection() {
	Vector2f inclination = gamepad0_->GetStickNormalized(aroundStick_);

	//!< deadzoneの決定
	float length = std::abs(inclination.x) + std::abs(inclination.y) / 2.0f;

	if (length < deadzone_) {
		return {};
	}

	//!< deadzoneを考慮したスケール計算
	float scale = (length - deadzone_) / (1.0f - deadzone_);

	Vector2f direction = inclination.Normalize();
	return direction * scale;
}
