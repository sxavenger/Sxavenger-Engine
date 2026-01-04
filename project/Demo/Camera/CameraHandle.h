#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>

//* lib
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/VectorComparision.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// CameraHandle class
////////////////////////////////////////////////////////////////////////////////////////////
class CameraHandle {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CameraHandle();
	~CameraHandle() = default;

	void Update();

	//* getter *//

	const Vector2f& GetDirection() const { return direction_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input *//

	const SxxEngine::KeyboardInput* keyboard_ = nullptr;
	const SxxEngine::MouseInput* mouse_       = nullptr;

	const SxxEngine::GamepadInput* gamepad0_ = nullptr;

	//* config *//

	//* keyboard

	std::array<KeyId, 4> aroundKeys_ = {
		KeyId::KEY_UP,
		KeyId::KEY_DOWN,
		KeyId::KEY_LEFT,
		KeyId::KEY_RIGHT
	};

	//* mouse

	MouseId aroundMouseButton_ = MouseId::MOUSE_RIGHT;

	//* gamepad

	GamepadStickId aroundStick_ = GamepadStickId::STICK_RIGHT;

	//* parameter *//

	float sensitivity_ = 0.4f;
	float deadzone_    = 0.1f;

	//* variables *//

	Vector2f direction_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f GetKeyboardDirection();

	Vector2f GetMouseDirection();

	Vector2f GetGamepadDirection();

};
