#include "GameCamera.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameCamera class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameCamera::Init() {
	APivotCameraActor::Init();

	gamepad_ = SxavengerSystem::GetInput()->GetGamepadInput(0);
}

void GameCamera::Term() {
}

void GameCamera::Update() {
	UpdateGamepad();
}

void GameCamera::UpdateGamepad() {
	if (!gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	// stickの入力を取得
	Vector2f direction = {};

	static const float deadzone = 0.1f;
	//!< value: 0.0f ~ 1.0f の間のdeadzoneを設定

	const Vector2f stick = gamepad_->GetStickNormalized(GamepadStickId::STICK_RIGHT);

	const float active = std::abs(stick.x) + std::abs(stick.y);

	if (active >= deadzone) { //!< deadzoneと実際の入力値を比較
		direction = {
			-stick.x,
			stick.y
		};
	}

	APivotCameraActor::GetParameter().angle += direction * rotationDelta_;
}
