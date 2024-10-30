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
#include "ComPtr.h"

// geometry
#include <Lib/Geometry/Vector2.h>

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

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class
////////////////////////////////////////////////////////////////////////////////////////////
class KeyboardInput {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	//* KeyData *//
	static const uint32_t kKeyNum = 256;
	using KeyData = std::array<BYTE, kKeyNum>;

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

	//* position *//

	const Vector2i& GetMousePos() const;

	Vector2i GetDeltaMousePos() const;

	//* button *//

	bool IsPressMouse(uint8_t buttonNum) const;

	//* wheel *//

	int32_t GetDeltaWheel() const;

	bool IsWheelUp() const;

	bool IsWheelDown() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> mouseDevice_;

	//* external *//

	const HWND* hWnd_;

	//* member *//

	InputState<DIMOUSESTATE> mouse_;
	Vector2i mousePos_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CalculteMousePos();

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

	//! @brief 接続されているかの確認
	bool IsConnect() const { return isConnect_; }

	//* GamePad input methods *//
	
	//! 仮でxInputGamepadをuint32_t 
	bool IsPressButton(uint32_t xInputGamepad) const;

	bool IsTriggerButton(uint32_t xInputGamepad) const;

	bool IsReleaseButton(uint32_t xInputGamepad) const;

	Vector2i GetLStick() const;
	Vector2f GetLStickNormalize() const;

	Vector2i GetRStick() const;
	Vector2f GetRStickNormalize() const;

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

	void Init(const HINSTANCE& hInst, const HWND& hWnd);

	void Update();

	//* keyboard inputs *//

	bool IsPressKey(uint8_t dik) const;

	bool IsTriggerKey(uint8_t dik) const;

	bool IsReleaseKey(uint8_t dik) const;

	const KeyboardInput* GetKeyboardInput() const { return keyboardInput_.get(); }

	//* mouse inputs *//

	const Vector2i& GetMousePos() const;

	Vector2i GetDeltaMousePos() const;

	bool IsPressMouse(uint8_t buttonNum) const;

	int32_t GetDeltaWheel() const;

	bool IsWheelUp() const;

	bool IsWheelDown() const;

	const MouseInput* GetMouseInput() const { return mouseInput_.get(); }

	//* GamePad inputs *//

	bool IsConnectGamepad(uint32_t gamepadNum) const;

	bool IsPressButton(uint32_t gamepadNum, uint32_t xInputGamepad) const;

	bool IsTriggerButton(uint32_t gamepadNum, uint32_t xInputGamepad) const;

	bool IsReleaseButton(uint32_t gamepadNum, uint32_t xInputGamepad) const;

	const GamepadInput* GetGamepadInput(uint32_t gamepadNum) const { return gamepadInputs_[gamepadNum].get(); }

	static const uint32_t GetGamepadNum() { return kGamepadNum_; }

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