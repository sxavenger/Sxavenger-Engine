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
	Assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = keyboardDevice_->SetDataFormat(
		&c_dfDIKeyboard // 標準形式
	);
	Assert(SUCCEEDED(hr));

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

	SetCooperativeLevel(SxavengerSystem::GetForcusWindow());

	// 前frameのkey状態の保存
	keys_.second = keys_.first;

	keys_.first = {};

	// キーボード情報の取得開始
	keyboardDevice_->Acquire();

	// キーボードの入力状態を取得
	keyboardDevice_->GetDeviceState(sizeof(keys_.first), keys_.first.data());
	
}

bool KeyboardInput::IsPress(KeyId id) const {
	return keys_.first[static_cast<uint8_t>(id)];
}

bool KeyboardInput::IsTrigger(KeyId id) const {
	return keys_.first[static_cast<uint8_t>(id)] && !keys_.second[static_cast<uint8_t>(id)];
}

bool KeyboardInput::IsRelease(KeyId id) const {
	return !keys_.first[static_cast<uint8_t>(id)] && keys_.second[static_cast<uint8_t>(id)];
}

void KeyboardInput::SetCooperativeLevel(const Window* window) {

	if (window != nullptr) {
		HWND hwnd = window->GetHwnd();

		if (hwnd != currentHwnd_) {
			//* 現在のhwndと違う場合, 再設定
			// 排他制御レベルのセット
			keyboardDevice_->SetCooperativeLevel(
				hwnd,
				flags_
			);
			//Assert(SUCCEEDED(hr)); // HACK:

			currentHwnd_ = hwnd;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MouseInput::Init(IDirectInput8* dInput) {

	// デバイスの生成
	auto hr = dInput->CreateDevice(
		GUID_SysMouse, &mouseDevice_, NULL
	);
	Assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(
		&c_dfDIMouse2 // 標準形式
	);
	Assert(SUCCEEDED(hr));

	flags_ |= DISCL_FOREGROUND;
	flags_ |= DISCL_NONEXCLUSIVE;

	//* DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
	//* DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない

}

void MouseInput::Term() {
}

void MouseInput::Update() {

	SetCooperativeLevel(SxavengerSystem::GetForcusWindow());

	// 前frameのmouse状態の保存
	mouse_.second = mouse_.first;

	// マウス情報の取得開始
	mouseDevice_->Acquire();

	// マウスの入力状態を取得
	mouseDevice_->GetDeviceState(sizeof(mouse_.first), &mouse_.first);

}

Vector2i MouseInput::GetPosition() const {
	POINT point = {};
	GetCursorPos(&point);
	ScreenToClient(currentHwnd_, &point);

	return { point.x, point.y };
}

Vector2i MouseInput::GetPosition(const Window* window) const {
	POINT point = {};
	GetCursorPos(&point);

	if (window != nullptr) {
		ScreenToClient(window->GetHwnd(), &point);
	}

	return { point.x, point.y };
}

Vector2i MouseInput::GetDeltaPosition() const {
	return { mouse_.first.lX, mouse_.first.lY };
}

bool MouseInput::IsPress(MouseId id) const {
	return mouse_.first.rgbButtons[static_cast<uint8_t>(id)];
}

bool MouseInput::IsTrigger(MouseId id) const {
	return mouse_.first.rgbButtons[static_cast<uint8_t>(id)] && !mouse_.second.rgbButtons[static_cast<uint8_t>(id)];;
}

bool MouseInput::IsRelease(MouseId id) const {
	return !mouse_.first.rgbButtons[static_cast<uint8_t>(id)] && mouse_.second.rgbButtons[static_cast<uint8_t>(id)];;
}

float MouseInput::GetDeltaWheel() const {
	return static_cast<float>(mouse_.first.lZ) / WHEEL_DELTA; //!< wheelの最大値でnormalize
}

bool MouseInput::IsWheelUp() const {
	return mouse_.first.lZ > 0;
}

bool MouseInput::IsWheelDown() const {
	return mouse_.first.lZ < 0;
}

void MouseInput::SetCooperativeLevel(const Window* window) {

	if (window != nullptr) {
		HWND hwnd = window->GetHwnd();

		if (hwnd != currentHwnd_) {
			//* 現在のhwndと違う場合, 再設定
			// 排他制御レベルのセット
			mouseDevice_->SetCooperativeLevel(
				hwnd,
				flags_
			);
			//Assert(SUCCEEDED(hr)); // HACK:

			currentHwnd_ = hwnd;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Input class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Input::Init(const Window* mainWindow) {

	auto hr = DirectInput8Create(
		mainWindow->GetHInst(),
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr
	);
	Assert(SUCCEEDED(hr));

	//* dinput *//

	keyboard_ = std::make_unique<KeyboardInput>();
	keyboard_->Init(directInput_.Get());

	mouse_ = std::make_unique<MouseInput>();
	mouse_->Init(directInput_.Get());
}

void Input::Term() {
}

void Input::Update() {
	keyboard_->Update();
	mouse_->Update();
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
