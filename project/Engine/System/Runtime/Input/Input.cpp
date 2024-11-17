#include "Input.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void KeyboardInput::Init(IDirectInput8* dInput, const HWND& hwnd) {
	
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

	// flagの設定
	DWORD flags = 0;
	flags |= DISCL_FOREGROUND;
	flags |= DISCL_NONEXCLUSIVE;
	/*flags |= DISCL_NOWINKEY;*/

	/*
		DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
		DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
		DISCL_NOWINKEY     -> Windowsキーの無効化
	*/

	// 排他制御レベルのセット
	hr = keyboardDevice_->SetCooperativeLevel(
		hwnd,
		flags
	);
	Assert(SUCCEEDED(hr));
	
}

void KeyboardInput::Term() {
}

void KeyboardInput::Update() {

	// 前frameのkey状態の保存
	keys_.second = keys_.first;

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
