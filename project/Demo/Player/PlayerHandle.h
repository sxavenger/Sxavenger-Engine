#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/VectorComparision.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerHandle class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerHandle {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputState enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class InputType : uint8_t {
		Keyboard,
		Gamepad,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MoveState enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class MoveState : uint8_t {
		Idle,
		Walk,
		Dash,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerHandle();
	~PlayerHandle() = default;

	void Update();

	//* getter *//

	MoveState GetMoveState() const { return moveState_; }

	const Vector3f& GetMoveVector() const { return moveVector_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input *//

	const SxxEngine::KeyboardInput* keyboard_ = nullptr;
	const SxxEngine::MouseInput* mouse_       = nullptr;

	const SxxEngine::GamepadInput* gamepad0_  = nullptr;

	//* config *//

	//* keyboard

	std::array<KeyId, 4> moveKeys_ = {
		KeyId::KEY_W,
		KeyId::KEY_S,
		KeyId::KEY_A,
		KeyId::KEY_D
	};
	KeyId dashKey_ = KeyId::KEY_LSHIFT;

	KeyId interactKey_ = KeyId::KEY_F;

	//* gamepad

	GamepadStickId moveStick_   = GamepadStickId::STICK_LEFT;
	GamepadButtonId dashButton_ = GamepadButtonId::BUTTON_X;

	GamepadButtonId interactButton_ = GamepadButtonId::BUTTON_A;

	//* parameter *//

	float deadzone_ = 0.1f;

	//* state *//

	InputType inputType_ = InputType::Keyboard;

	Vector3f moveVector_  = {};
	MoveState moveState_  = MoveState::Idle;

	bool isInteract_ = false;

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f GetKeyboardDirection() const;

	Vector2f GetGamepadDirection() const;

	bool IsDashKeyboard() const;

	bool IsDashGamepad() const;

	void UpdateMove();

	void UpdateInteract();

};
