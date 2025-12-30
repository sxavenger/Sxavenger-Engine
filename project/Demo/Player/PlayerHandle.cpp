#include "PlayerHandle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

PlayerHandle::PlayerHandle() {
	keyboard_ = SxxEngine::System::GetInput()->GetKeyboardInput();
	mouse_    = SxxEngine::System::GetInput()->GetMouseInput();
	gamepad0_ = SxxEngine::System::GetInput()->GetGamepadInput(0);
}

void PlayerHandle::Update() {

	Vector2f keyboardDirection = GetKeyboardDirection();
	Vector2f gamepadDirection  = GetGamepadDirection();

	float keyboardLength = keyboardDirection.Length();
	float gamepadLength  = gamepadDirection.Length();

	Vector2f direction = {};

	//!< 大きい方の入力を採用 (両方同じ場合はkeyboard優先)
	if (keyboardLength >= gamepadLength) {
		direction  = keyboardDirection;
		inputType_ = InputType::Keyboard;

	} else {
		direction  = gamepadDirection;
		inputType_ = InputType::Gamepad;
	}

	if (All(direction == kOrigin2<float>)) {
		moveVector_ = kOrigin3<float>;
		moveState_  = MoveState::Idle;
		return;
	}

	moveVector_ = { direction.x, 0.0f, direction.y };
	moveState_  = MoveState::Walk;

	if (IsDashKeyboard() || IsDashGamepad()) {
		moveState_ = MoveState::Dash;
	}

}

Vector2f PlayerHandle::GetKeyboardDirection() const {
	return keyboard_->GetDirection(moveKeys_);
}

Vector2f PlayerHandle::GetGamepadDirection() const {
	Vector2f inclination = gamepad0_->GetStickNormalized(moveStick_);

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

bool PlayerHandle::IsDashKeyboard() const {
	return keyboard_->IsPress(dashKey_);
}

bool PlayerHandle::IsDashGamepad() const {
	return gamepad0_->IsPress(dashButton_);
}
