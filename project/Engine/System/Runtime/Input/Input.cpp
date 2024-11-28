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

	const GameWindow* window = SxavengerSystem::GetForcusWindow();

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

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class methods
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
}

void Input::Term() {
}

void Input::Update() {
	keyboard_->Update();
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
