#include "Input.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void KeyboardInput::Init(IDirectInput8* dInput) {
	
	// キーボードデバイスの生成
	auto hr = dInput->CreateDevice(
		GUID_SysKeyboard, &keyboardDevice_, NULL
	);
	DxObject::Assert(hr, L"keyboard device create failed.");

	// 入力データ形式のセット
	hr = keyboardDevice_->SetDataFormat(
		&c_dfDIKeyboard // 標準形式
	);
	DxObject::Assert(hr, L"keyboard set data format failed.");

	flags_ |= DISCL_FOREGROUND;
	flags_ |= DISCL_NONEXCLUSIVE;
	/*flags_ |= DISCL_NOWINKEY;*/

	//* DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
	//* DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
	//* DISCL_NOWINKEY     -> Windowsキーの無効化
}

void KeyboardInput::Term() {
}

void KeyboardInput::Update() {

	isEnableAquire_ = SetCooperativeLevel(SxavengerSystem::GetForcusWindow());

	// 前frameのkey状態の保存
	keys_.second = keys_.first;

	keys_.first = {};

	// キーボード情報の取得開始
	keyboardDevice_->Acquire();

	// キーボードの入力状態を取得
	keyboardDevice_->GetDeviceState(sizeof(keys_.first), keys_.first.data());
	
}

bool KeyboardInput::IsPress(KeyId id) const {
	if (!isEnableAquire_) {
		return false;
	}

	return keys_.first[static_cast<uint8_t>(id)];
}

bool KeyboardInput::IsTrigger(KeyId id) const {
	if (!isEnableAquire_) {
		return false;
	}

	return keys_.first[static_cast<uint8_t>(id)] && !keys_.second[static_cast<uint8_t>(id)];
}

bool KeyboardInput::IsRelease(KeyId id) const {
	if (!isEnableAquire_) {
		return false;
	}

	return !keys_.first[static_cast<uint8_t>(id)] && keys_.second[static_cast<uint8_t>(id)];
}

bool KeyboardInput::SetCooperativeLevel(const DirectXWindowContext* window) {
	if (window == nullptr) {
		return false;
	}

	HWND hwnd = window->GetHwnd();

	if (hwnd != currentHwnd_) {
		//* 現在のhwndと違う場合, 再設定
		// 排他制御レベルのセット
		keyboardDevice_->SetCooperativeLevel(
			hwnd,
			flags_
		);
		//Exception::Assert(SUCCEEDED(hr)); // HACK:

		currentHwnd_ = hwnd;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MouseInput::Init(IDirectInput8* dInput) {

	// デバイスの生成
	auto hr = dInput->CreateDevice(
		GUID_SysMouse, &mouseDevice_, NULL
	);
	Exception::Assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(
		&c_dfDIMouse2 // 標準形式
	);
	Exception::Assert(SUCCEEDED(hr));

	flags_ |= DISCL_FOREGROUND;
	flags_ |= DISCL_NONEXCLUSIVE;

	//* DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
	//* DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない

}

void MouseInput::Term() {
}

void MouseInput::Update() {

	isEnableAquire_ = SetCooperativeLevel(SxavengerSystem::GetForcusWindow());

	// 前frameのmouse状態の保存
	mouse_.second = mouse_.first;

	// マウス情報の取得開始
	mouseDevice_->Acquire();

	// マウスの入力状態を取得
	mouseDevice_->GetDeviceState(sizeof(mouse_.first), &mouse_.first);

	ShowCousor(true);
}

Vector2i MouseInput::GetPosition() const {
	if (!isEnableAquire_) {
		return {};
	}

	POINT point = {};
	GetCursorPos(&point);
	ScreenToClient(currentHwnd_, &point);

	return { point.x, point.y };
}

Vector2i MouseInput::GetPosition(const DirectXWindowContext* window) const {
	if (!isEnableAquire_) {
		return {};
	}

	POINT point = {};
	GetCursorPos(&point);

	if (window != nullptr) {
		ScreenToClient(window->GetHwnd(), &point);
	}

	return { point.x, point.y };
}

Vector2i MouseInput::GetDeltaPosition() const {
	if (!isEnableAquire_) {
		return {};
	}

	return { mouse_.first.lX, mouse_.first.lY };
}

void MouseInput::SetPosition(const Vector2i& position) const {
	if (!isEnableAquire_) {
		return;
	}

	POINT point = { position.x, position.y };
	ClientToScreen(currentHwnd_, &point);
	SetCursorPos(point.x, point.y);
}

void MouseInput::ShowCousor(bool isShow) const {
	while (isShow ? ShowCursor(isShow) < 0 : ShowCursor(isShow) >= 0) {}
}

bool MouseInput::IsPress(MouseId id) const {
	if (!isEnableAquire_) {
		return false;
	}

	return mouse_.first.rgbButtons[static_cast<uint8_t>(id)];
}

bool MouseInput::IsTrigger(MouseId id) const {
	if (!isEnableAquire_) {
		return false;
	}

	return mouse_.first.rgbButtons[static_cast<uint8_t>(id)] && !mouse_.second.rgbButtons[static_cast<uint8_t>(id)];;
}

bool MouseInput::IsRelease(MouseId id) const {
	if (!isEnableAquire_) {
		return false;
	}

	return !mouse_.first.rgbButtons[static_cast<uint8_t>(id)] && mouse_.second.rgbButtons[static_cast<uint8_t>(id)];;
}

float MouseInput::GetDeltaWheel() const {
	if (!isEnableAquire_) {
		return 0.0f;
	}

	return static_cast<float>(mouse_.first.lZ) / WHEEL_DELTA; //!< wheelの最大値でnormalize
}

bool MouseInput::IsWheel() const {
	return mouse_.first.lZ != 0;
}

bool MouseInput::IsWheelUp() const {
	if (!isEnableAquire_) {
		return false;
	}

	return mouse_.first.lZ > 0;
}

bool MouseInput::IsWheelDown() const {
	if (!isEnableAquire_) {
		return false;
	}

	return mouse_.first.lZ < 0;
}

bool MouseInput::SetCooperativeLevel(const DirectXWindowContext* window) {
	if (window == nullptr) {
		return false;
	}

	HWND hwnd = window->GetHwnd();

	if (hwnd != currentHwnd_) {
		//* 現在のhwndと違う場合, 再設定
		// 排他制御レベルのセット
		mouseDevice_->SetCooperativeLevel(
			hwnd,
			flags_
		);
		//Exception::Assert(SUCCEEDED(hr)); // HACK:

		currentHwnd_ = hwnd;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GamepadInput::Init(uint8_t number) {
	// numberの設定
	number_ = number;
}

void GamepadInput::Term() {
}

void GamepadInput::Update() {

	// 1frame前のコントローラー状態の保存
	state_.second = state_.first;

	// コントローラーの入力状態を取得
	auto dr = XInputGetState(number_, &state_.first);

	// 接続状態の更新
	isConnect_ = (dr == ERROR_SUCCESS);
	//!< [retval] true: 接続されてる, false: 接続されてない

}

bool GamepadInput::IsPress(GamepadButtonId id) const {
	const auto& [first, second] = state_;
	return (first.Gamepad.wButtons & static_cast<uint16_t>(id));
}

bool GamepadInput::IsPress(GamepadTriggerId id) const {
	const auto& [first, second] = state_;

	if (id == GamepadTriggerId::TRIGGER_LEFT) {
		return (first.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	} else if (id == GamepadTriggerId::TRIGGER_RIGHT) {
		return (first.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	return {}; //!< error case
}

bool GamepadInput::IsTrigger(GamepadButtonId id) const {
	const auto& [first, second] = state_;
	return (first.Gamepad.wButtons & static_cast<uint16_t>(id)) && !(second.Gamepad.wButtons & static_cast<uint16_t>(id));
}

bool GamepadInput::IsTrigger(GamepadTriggerId id) const {
	const auto& [first, second] = state_;

	if (id == GamepadTriggerId::TRIGGER_LEFT) {
		return (first.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) && !(second.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	} else if (id == GamepadTriggerId::TRIGGER_RIGHT) {
		return (first.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) && !(second.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	return {}; //!< error case
}

bool GamepadInput::IsRelease(GamepadButtonId id) const {
	const auto& [first, second] = state_;
	return !(first.Gamepad.wButtons & static_cast<uint16_t>(id)) && (second.Gamepad.wButtons & static_cast<uint16_t>(id));
}

bool GamepadInput::IsRelease(GamepadTriggerId id) const {
	const auto& [first, second] = state_;

	if (id == GamepadTriggerId::TRIGGER_LEFT) {
		return !(first.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) && (second.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	} else if (id == GamepadTriggerId::TRIGGER_RIGHT) {
		return !(first.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) && (second.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	return {}; //!< error case
}

Vector2i GamepadInput::GetStick(GamepadStickId id) const {
	if (id == GamepadStickId::STICK_LEFT) {
		return { state_.first.Gamepad.sThumbLX, state_.first.Gamepad.sThumbLY };

	} else if (id == GamepadStickId::STICK_RIGHT) {
		return { state_.first.Gamepad.sThumbRX, state_.first.Gamepad.sThumbRY };
	}

	return {}; //!< error case
}

Vector2f GamepadInput::GetStickNormalized(GamepadStickId id) const {
	return static_cast<Vector2f>(GetStick(id)) / static_cast<float>(SHRT_MAX);
}
////////////////////////////////////////////////////////////////////////////////////////////
// Input class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Input::Init(const DirectXWindowContext* mainWindow) {

	auto hr = DirectInput8Create(
		mainWindow->GetHinst(),
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr
	);
	DxObject::Assert(hr, L"DirectInput8Create failed.");

	//* dinput *//

	keyboard_ = std::make_unique<KeyboardInput>();
	keyboard_->Init(directInput_.Get());

	mouse_ = std::make_unique<MouseInput>();
	mouse_->Init(directInput_.Get());

	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i) {
		gamepads_[i] = std::make_unique<GamepadInput>();
		gamepads_[i]->Init(i);
	}
}

void Input::Term() {
}

void Input::Update() {
	keyboard_->Update();
	mouse_->Update();

	for (auto& gamepad : gamepads_) {
		gamepad->Update();
	}
}

bool Input::IsPressKey(KeyId id) {
	return keyboard_->IsPress(id);
}

bool Input::IsTriggerKey(KeyId id) {
	return keyboard_->IsTrigger(id);
}

bool Input::IsReleaseKey(KeyId id) {
	return keyboard_->IsRelease(id);
}
