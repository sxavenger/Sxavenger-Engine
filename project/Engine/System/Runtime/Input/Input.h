#pragma once

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのversion指定

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* id
#include "InputId.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>

//* lib
#include <lib/Sxl/Flag.h>

//* externals
#include <magic_enum.hpp>

//* input
#include <dinput.h>
#include <Xinput.h>

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
// InputType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class InputType : uint8_t {
	Main_Current,
	Main_Previous,
	Async_Stack,
};

////////////////////////////////////////////////////////////////////////////////////////////
// InputState enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class InputState : size_t {
};

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class
////////////////////////////////////////////////////////////////////////////////////////////
class KeyboardInput
	: public ISystemDebugGui {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	static const size_t kKeyCount = 256; //!< keyの数
	using KeyInput = std::array<BYTE, kKeyCount>; //!< key input data type

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Clear() { *this = InputData(); }

		bool IsEnableAcquire() const { return isEnableAcquire; }

		bool GetKey(KeyId id) const { return keys.at(static_cast<uint8_t>(id)); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isEnableAcquire = false; //!< inputが取得可能かどうか
		KeyInput keys        = {};    //!< keyの状態

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	KeyboardInput() = default;
	KeyboardInput(IDirectInput8* dInput) { Init(dInput); }

	void Init(IDirectInput8* dInput);

	void AsyncUpdate();

	void UpdateInputState();

	//* keyboard input option *//

	bool IsPress(KeyId id) const;

	bool IsTrigger(KeyId id) const;

	bool IsRelease(KeyId id) const;

	//* debug gui *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> keyboardDevice_;

	DWORD flags_ = NULL;

	HWND currentHwnd_ = NULL;

	//* member *//

	std::array<InputData, magic_enum::enum_count<InputType>()> inputs_;

	
	//=========================================================================================
	// private variables
	//=========================================================================================

	bool SetCooperativeLevel(const DirectXWindowContext* window);

};

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class
////////////////////////////////////////////////////////////////////////////////////////////
class MouseInput
	: public ISystemDebugGui {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Clear() { *this = InputData(); }

		bool IsEnableAcquire() const { return isEnableAcquire; }

		Vector2i GetDeltaPosition() const { return { mouse.lX, mouse.lY }; }

		int32_t GetDeltaWheel() const { return mouse.lZ; }
		bool IsWheel() const { return mouse.lZ != 0; }
		bool IsWheelUp() const { return mouse.lZ > 0; }
		bool IsWheelDown() const { return mouse.lZ < 0; }

		bool GetButton(MouseId id) const { return mouse.rgbButtons[static_cast<uint8_t>(id)]; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isEnableAcquire = false; //!< inputが取得可能かどうか
		DIMOUSESTATE2 mouse  = {};    //!< mouse input data

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MouseInput()  = default;
	MouseInput(IDirectInput8* dInput) { Init(dInput); }

	void Init(IDirectInput8* dInput);

	void AsyncUpdate();

	void UpdateInputState();

	//* mouse position option *//

	Vector2i GetPosition() const;
	Vector2i GetPosition(const DirectXWindowContext* window) const;

	Vector2i GetDeltaPosition() const;

	void SetPosition(const Vector2i& position) const;

	//* cursor option *//

	void ShowCousor(bool isShow) const;

	//* mouse input option *//

	bool IsPress(MouseId id) const;

	bool IsTrigger(MouseId id) const;

	bool IsRelease(MouseId id) const;

	//* mouse wheel *//

	int32_t GetDeltaWheel() const;

	float GetDeltaWheelNormalized() const;

	bool IsWheel() const;

	bool IsWheelUp() const;

	bool IsWheelDown() const;

	//* debug gui *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> mouseDevice_;

	DWORD flags_ = NULL;

	HWND currentHwnd_ = nullptr;

	//* member *//

	std::array<InputData, magic_enum::enum_count<InputType>()> inputs_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool SetCooperativeLevel(const DirectXWindowContext* window);

};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadInput class
////////////////////////////////////////////////////////////////////////////////////////////
class GamepadInput
	: public ISystemDebugGui {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Clear() { *this = InputData(); }

		bool IsConnect() const { return isConnect; }

		bool GetButton(GamepadButtonId id) const { return (xinput.Gamepad.wButtons & static_cast<uint16_t>(id)) != 0; }
		bool GetButton(GamepadTriggerId id) const;

		Vector2i GetStick(GamepadStickId id) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isConnect      = false; //!< gamepadが接続されているかどうか
		XINPUT_STATE xinput = {};    //!< xinput state

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GamepadInput() = default;
	GamepadInput(uint8_t number) { Init(number); }

	void Init(uint8_t number);

	void AsyncUpdate();

	void UpdateInputState();

	//* gamepad state option *//

	bool IsConnect() const;

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

	//* debug gui *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* member *//

	uint8_t number_ = NULL;

	//* state *//

	std::array<InputData, magic_enum::enum_count<InputType>()> inputs_;

};


////////////////////////////////////////////////////////////////////////////////////////////
// Input class
////////////////////////////////////////////////////////////////////////////////////////////
class Input
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Input() = default;

	void Init(const DirectXWindowContext* mainWindow);

	void Shutdown();

	void Update();

	//* key input option *//

	bool IsPressKey(KeyId id);

	bool IsTriggerKey(KeyId id);

	bool IsReleaseKey(KeyId id);

	//* getter *//

	const KeyboardInput* GetKeyboardInput() const { return keyboard_.get(); }

	const MouseInput* GetMouseInput() const { return mouse_.get(); }

	const GamepadInput* GetGamepadInput(uint8_t number) const { return gamepads_[number].get(); }

	//* debug gui *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::thread thread_;
	std::mutex mutex_;

	bool isTerminate_ = false;

	//* directInput *//

	ComPtr<IDirectInput8> directInput_;

	//* dinput *//

	std::unique_ptr<KeyboardInput> keyboard_;
	std::unique_ptr<MouseInput>    mouse_;

	//* xinput *//

	std::array<std::unique_ptr<GamepadInput>, XUSER_MAX_COUNT> gamepads_;


	// todo: stringからinput取得する仕組みを作る

};
