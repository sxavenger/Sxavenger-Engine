#pragma once

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのversion指定

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <utility>
#include <memory>
#include <array>

// input
#include <dinput.h> //!< keyboard
#include <Xinput.h> //!< gamepad

// ComPtr
#include <ComPtr.h>

// geometry
#include <Vector2.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
// dinput
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// xinput
#pragma comment(lib, "Xinput.lib")

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
//* InputState *//
//! [pair]
//! first:  現在frameのInput状態
//! second: 前frameのInput状態
template <typename T>
using InputState = std::pair<T, T>;

//* KeyData *//
constexpr const uint32_t kKeyNum = 256;
using KeyData = std::array<BYTE, kKeyNum>;

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class
////////////////////////////////////////////////////////////////////////////////////////////
class KeyboardInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	KeyboardInput() = default;

	void Init(IDirectInput8* dInput, const HWND& hWnd);

	void Update();

	//* Keyboard input methods *//

	bool IsPressKey(uint8_t dik) const;

	bool IsTriggerKey(uint8_t dik) const;

	bool IsReleaseKey(uint8_t dik) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> keyboardDevice_;

	//* member *//

	InputState<KeyData> keys_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class
////////////////////////////////////////////////////////////////////////////////////////////
class MouseInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MouseInput() = default;

	void Init(IDirectInput8* dInput, const HWND& hWnd);

	void Update();

	//* Mouse input methods *//

	Vector2i GetMousePos();

	Vector2i GetDeltaMousePos();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> mouseDevice_;

	//* member *//

	InputState<DIMOUSESTATE> mouse_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GamePadInput class
////////////////////////////////////////////////////////////////////////////////////////////
class GamepadInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GamepadInput() = default;

	void Init(uint32_t gamepadNumber);

	void Update();

	//* Connect methods *//

	bool IsConnect() const { return isConnect_; }

	//* GamePad input methods *//
	//* 仮でuint32_t

	bool IsPressButton(uint32_t xInputGamepad) const;

	bool IsTriggerButton(uint32_t xInputGamepad) const;

	bool IsReleaseButton(uint32_t xInputGamepad) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	uint32_t gamepadNumber_;

	//* member *//

	bool isConnect_ = false;

	InputState<XINPUT_STATE> gamepad_;
	
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

	void Init(const HINSTANCE& hInst, const HWND& hWnd);

	void Update();

	//* keyboard inputs *//

	bool IsPressKey(uint8_t dik) const;

	bool IsTriggerKey(uint8_t dik) const;

	bool IsReleaseKey(uint8_t dik) const;

	const KeyboardInput* GetKeyboardInput() const { return keyboardInput_.get(); }

	//* mouse inputs *//

	const MouseInput* GetMouseInput() const { return mouseInput_.get(); }

	//* GamePad inputs *//

	bool IsConnectGamePad(uint32_t gamepadNum) const;

	bool IsPressButton(uint32_t gamepadNum, uint32_t xInputGamepad) const;

	bool IsTriggerButton(uint32_t gamepadNum, uint32_t xInputGamepad) const;

	bool IsReleaseButton(uint32_t gamepadNum, uint32_t xInputGamepad) const;

	const GamepadInput* GetGamepadInput(uint32_t gamepadNum) const { return gamepadInputs_[gamepadNum].get(); }

	//* Singleton *//

	static Input* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directInput *//

	ComPtr<IDirectInput8> directInput_;

	//* KeyboardInput *//

	std::unique_ptr<KeyboardInput> keyboardInput_;

	//* MouseInput *//
	
	std::unique_ptr<MouseInput> mouseInput_;

	//* GamePadInput *//

	static const uint32_t kGamepadNum_ = 1; //!< 使用するGamePadの個数
	std::array<std::unique_ptr<GamepadInput>, kGamepadNum_> gamepadInputs_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDirectInput(const HINSTANCE& hInst);

};