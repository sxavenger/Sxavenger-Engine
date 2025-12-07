#include "InputHelper.h"

////////////////////////////////////////////////////////////////////////////////////////////
// InputHelper class methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector2f InputHelper::GetDirection(const KeyboardInput* input, const std::array<KeyId, 4>& id) {

	if (!input->IsEnableAcquire()) { //!< 入力が取得できない
		return {};
	}

	Vector2f direction = {};

	if (input->IsPress(id[0])) { //!< up
		direction.y += 1.0f;
	}

	if (input->IsPress(id[1])) { //!< down
		direction.y -= 1.0f;
	}

	if (input->IsPress(id[2])) { //!< left
		direction.x -= 1.0f;
	}

	if (input->IsPress(id[3])) { //!< right
		direction.x += 1.0f;
	}

	return direction;
}

Vector2f InputHelper::GetDirection(const GamepadInput* input, GamepadStickId id, float deadzone = 0.0f) {
	if (!input->IsConnect()) { //!< コントローラーが接続されていない
		return {};
	}

	Vector2f direction = input->GetStickNormalized(id);

	if (direction.Length() < deadzone) { //!< デッドゾーン内
		return {};
	}

	return direction;
}

Vector2f InputHelper::GetDirectionNormalize(const Vector2f& direction) {

	float length = direction.Length();
	length = std::clamp(length, 0.0f, 1.0f);

	return direction.Normalize() * length;
	
}
