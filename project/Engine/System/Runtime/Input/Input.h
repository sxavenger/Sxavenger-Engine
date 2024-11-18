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

//* c++
#include <cstdint>
#include <array>

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

	KeyboardInput()  = default;
	~KeyboardInput() { Term(); }

	void Init(IDirectInput8* dInput, const HWND& hwnd);

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

	//* member *//

	static const uint32_t kKeyNum_ = 256;
	InputState<std::array<BYTE, kKeyNum_>> keys_;

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

	void Init();

	void Term();

private:
};

