#include "Input.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// Input class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Input::Init(const HINSTANCE& hInst, const HWND& hWnd) {

	// DirectInputの初期化
	{
		auto hr = DirectInput8Create(
			hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
			(void**)&directInput_, nullptr
		);

		assert(SUCCEEDED(hr));
	}

	// キーボードデバイス
	{
		// キーボードデバイスの生成
		auto hr = directInput_->CreateDevice(
			GUID_SysKeyboard, &keyboard_, NULL
		);
		assert(SUCCEEDED(hr));

		// 入力データ形式のセット
		hr = keyboard_->SetDataFormat(
			&c_dfDIKeyboard // 標準形式
		);
		assert(SUCCEEDED(hr));

		// 排他制御レベルのセット
		hr = keyboard_->SetCooperativeLevel(
			hWnd,
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
		);
		assert(SUCCEEDED(hr));

		/* flag
			DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
			DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
			DISCL_NOWINKEY     -> Windowsキーの無効化
		*/
	}
	
}

void Input::Term() {
	directInput_ = nullptr;
	keyboard_ = nullptr;
}

void Input::Update() {
	// preKeyに保存
	memcpy(preKey_, key_, kMaxKey_); //!< sizeof(BYTE, unsigned) = 1 なので省略

	// キーボード情報の取得開始
	keyboard_->Acquire();

	// キーボードの入力状態を取得
	keyboard_->GetDeviceState(sizeof(key_), key_);

}

bool Input::IsPressKey(uint8_t dik) const {
	return key_[dik];
}

bool Input::IsTriggerKey(uint8_t dik) const {
	return key_[dik] && !preKey_[dik];
}

bool Input::IsReleaseKey(uint8_t dik) const {
	return !key_[dik] && preKey_[dik];
}

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}
