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

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
// dinput
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// xinput
#pragma comment(lib, "Xinput.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class
////////////////////////////////////////////////////////////////////////////////////////////
class KeyboardInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	KeyboardInput() = default;

	void Init(const HINSTANCE& hInst, const HWND& hWnd);

	void Update();

	//* Keyboard input methods *//

	bool IsPressKey(uint8_t dik) const;

	bool IsTriggerKey(uint8_t dik) const;

	bool IsReleaseKey(uint8_t dik) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dinput *//

	ComPtr<IDirectInput8>       directInput_;
	ComPtr<IDirectInputDevice8> keyboard_;

	//* member *//

	static const uint32_t kKeyboradButtonNum_ = 256;
	BYTE    keys_[kKeyboradButtonNum_] = {};
	BYTE preKeys_[kKeyboradButtonNum_] = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// GamePadInput class
////////////////////////////////////////////////////////////////////////////////////////////
class GamePadInput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GamePadInput() = default;

	void Init(uint32_t gamePadNumber);

	void Update();

	//* Connect methods *//

	bool IsConnect() const { return isConnect_; }

	//* GamePad input methods *//
	//* 仮でuint32_t

	bool IsPressButton(uint32_t xInputGamePad) const;

	bool IsTriggerButton(uint32_t xInputGamePad) const;

	bool IsReleaseButton(uint32_t xInputGamePad) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	uint32_t gamePadNumber_;

	//* member *//

	bool isConnect_ = false;

	//* [pair]
	//* first:  現在frameのinput状態
	//* second: 1frame前のstate状態
	std::pair<XINPUT_STATE, XINPUT_STATE> state_;
	
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

	KeyboardInput* GetKeyboardInput() const { return keyboardInput_.get(); }

	//* GamePad inputs *//

	bool IsConnectGamePad(uint32_t gamePadNum) const;

	bool IsPressButton(uint32_t gamePadNum, uint32_t xInputGamePad) const;

	bool IsTriggerButton(uint32_t gamePadNum, uint32_t xInputGamePad) const;

	bool IsReleaseButton(uint32_t gamePadNum, uint32_t xInputGamePad) const;

	GamePadInput* GetGamePadInput(uint32_t gamePadNum) { return gamePadInputs_[gamePadNum].get(); }

	//* Singleton *//

	static Input* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* KeyInput *//

	std::unique_ptr<KeyboardInput> keyboardInput_;

	//* GamePadInput *//

	static const uint32_t kGamePadNum_ = 1; //!< 使用するGamePadの個数
	std::array<std::unique_ptr<GamePadInput>, kGamePadNum_> gamePadInputs_;

};