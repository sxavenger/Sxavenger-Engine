#pragma once

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのversion指定

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include <dinput.h>
#include <Xinput.h>

//* id
#include "InputId.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/Window/Window.h>

//* c++
#include <cstdint>
#include <array>
#include <memory>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
//!< dinput
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//!< xinput
#pragma comment(lib, "Xinput.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

//! [pair]
//! first:  現在frameのInput状態
//! second: 前frameのInput状態
template <typename T>
using InputState = std::pair<T, T>;

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class
////////////////////////////////////////////////////////////////////////////////////////////
class KeyboardInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	KeyboardInput() = default;
	~KeyboardInput() { Term(); }

	void Init(IDirectInput8* dInput);

	void Term();

	void Update();

	//* keyboard input option *//

	bool IsPress(KeyId id) const;

	bool IsTrigger(KeyId id) const;

	bool IsRelease(KeyId id) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> keyboardDevice_;

	DWORD flags_ = NULL;

	HWND currentHwnd_ = NULL;

	//* member *//

	static const uint32_t kKeyNum_ = 256;
	InputState<std::array<BYTE, kKeyNum_>> keys_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void SetCooperativeLevel(const Window* window);

};

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class
////////////////////////////////////////////////////////////////////////////////////////////
class MouseInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MouseInput()  = default;
	~MouseInput() { Term(); }

	void Init(IDirectInput8* dInput);

	void Term();

	void Update();

	//* mouse position option *//

	Vector2i GetPosition() const;
	Vector2i GetPosition(const Window* window) const;

	Vector2i GetDeltaPosition() const;

	void SetPosition(const Vector2i& position) const;

	//* cursor option *//

	void ShowCousor(bool isShow) const;

	//* mouse input option *//

	bool IsPress(MouseId id) const;

	bool IsTrigger(MouseId id) const;

	bool IsRelease(MouseId id) const;

	//* mouse wheel *//

	float GetDeltaWheel() const;

	bool IsWheelUp() const;

	bool IsWheelDown() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> mouseDevice_;

	DWORD flags_ = NULL;

	HWND currentHwnd_ = nullptr;

	//* member *//

	InputState<DIMOUSESTATE2> mouse_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void SetCooperativeLevel(const Window* window);

};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadInput class
////////////////////////////////////////////////////////////////////////////////////////////
class GamepadInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GamepadInput() = default;
	~GamepadInput() { Term(); }

	void Init(uint8_t number);

	void Term();

	void Update();

	//* gamepad state option *//

	bool IsConnect() const { return isConnect_; }

	//* gamepad input option *//

	bool IsPress(GamepadButtonId id) const;
	bool IsPress(GamepadTriggerId id) const;

	bool IsTrigger(GamepadButtonId id) const;
	bool IsTrigger(GamepadTriggerId id) const;

	bool IsRelease(GamepadButtonId id) const;
	bool IsRelease(GamepadTriggerId id) const;

	//* gamepad stick option *//

	Vector2i GetStick(GamepadStickId id) const;
	Vector2f GetStickNormalized(GamepadStickId id) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* member *//

	uint8_t number_ = NULL;

	//* state *//

	InputState<XINPUT_STATE> state_;

	bool isConnect_ = false;

};


////////////////////////////////////////////////////////////////////////////////////////////
// Input class
////////////////////////////////////////////////////////////////////////////////////////////
class Input {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Input() = default;
	~Input() { Term(); }

	void Init(const Window* mainWindow);

	void Term();

	void Update();

	//* key input option *//

	bool IsPressKey(KeyId id);

	bool IsTriggerKey(KeyId id);

	bool IsReleaseKey(KeyId id);

	//* getter *//

	const KeyboardInput* GetKeyboardInput() const { return keyboard_.get(); }

	const MouseInput* GetMouseInput() const { return mouse_.get(); }

	const GamepadInput* GetGamepadInput(uint8_t number) const { return gamepads_[number].get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directInput *//

	ComPtr<IDirectInput8> directInput_;

	//* dinput *//

	std::unique_ptr<KeyboardInput> keyboard_;
	std::unique_ptr<MouseInput>    mouse_;

	//* xinput *//

	std::array<std::unique_ptr<GamepadInput>, XUSER_MAX_COUNT> gamepads_;

};

