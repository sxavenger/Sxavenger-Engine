#include "Input.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void KeyboardInput::Init(const HINSTANCE& hInst, const HWND& hWnd) {
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

		// flagの設定
		DWORD flags = 0;
		flags |= DISCL_FOREGROUND;
		flags |= DISCL_NONEXCLUSIVE;
		flags |= DISCL_NOWINKEY;

		/*
			DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
			DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
			DISCL_NOWINKEY     -> Windowsキーの無効化
		*/

		// 排他制御レベルのセット
		hr = keyboard_->SetCooperativeLevel(
			hWnd,
			flags
		);
		assert(SUCCEEDED(hr));
	}
}

void KeyboardInput::Update() {

	// preKeyに保存
	memcpy(preKeys_, keys_, kKeyboradButtonNum_); //!< sizeof(BYTE, unsigned) = 1 なので省略

	// キーボード情報の取得開始
	keyboard_->Acquire();

	// キーボードの入力状態を取得
	keyboard_->GetDeviceState(sizeof(keys_), keys_);

}

bool KeyboardInput::IsPressKey(uint8_t dik) const {
	return keys_[dik];
}

bool KeyboardInput::IsTriggerKey(uint8_t dik) const {
	return keys_[dik] && !preKeys_[dik];
}

bool KeyboardInput::IsReleaseKey(uint8_t dik) const {
	return !keys_[dik] && preKeys_[dik];
}

////////////////////////////////////////////////////////////////////////////////////////////
// GamePadInput class methods
////////////////////////////////////////////////////////////////////////////////////////////
void GamePadInput::Init(uint32_t gamePadNumber) {
	// numberの保存
	gamePadNumber_ = gamePadNumber;
}

void GamePadInput::Update() {

	// 1frame前のコントローラー状態の保存
	state_.second = state_.first;

	// 現在のコントローラー状態の取得
	auto dr = XInputGetState(gamePadNumber_, &state_.first);

	isConnect_ = (dr == ERROR_SUCCESS); //!< 接続されてるかの確認
	// [retval] true: 接続されてる, false: 接続されてない

}

bool GamePadInput::IsPressButton(uint32_t xInputGamePad) const {
	return state_.first.Gamepad.wButtons & xInputGamePad;
}

bool GamePadInput::IsTriggerButton(uint32_t xInputGamePad) const {
	return (state_.first.Gamepad.wButtons & xInputGamePad) && !(state_.second.Gamepad.wButtons & xInputGamePad);
}

bool GamePadInput::IsReleaseButton(uint32_t xInputGamePad) const {
	return !(state_.first.Gamepad.wButtons & xInputGamePad) && (state_.second.Gamepad.wButtons & xInputGamePad);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Input class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Input::Init(const HINSTANCE& hInst, const HWND& hWnd) {

	// keyboardInput
	keyboardInput_ = std::make_unique<KeyboardInput>();
	keyboardInput_->Init(hInst, hWnd);

	// gamePadInput
	for (uint32_t i = 0; i < kGamePadNum_; ++i) {
		gamePadInputs_[i] = std::make_unique<GamePadInput>();
		gamePadInputs_[i]->Init(i);
	}


}

void Input::Update() {

	// keyboardInput
	keyboardInput_->Update();
	
	// gamePadInput
	for (auto& gamePad : gamePadInputs_) {
		gamePad->Update();
	}
}

bool Input::IsPressKey(uint8_t dik) const {
	return keyboardInput_->IsPressKey(dik);
}

bool Input::IsTriggerKey(uint8_t dik) const {
	return keyboardInput_->IsTriggerKey(dik);
}

bool Input::IsReleaseKey(uint8_t dik) const {
	return keyboardInput_->IsReleaseKey(dik);
}

bool Input::IsConnectGamePad(uint32_t gamePadNum) const {
	return gamePadInputs_[gamePadNum]->IsConnect();
}

bool Input::IsPressButton(uint32_t gamePadNum, uint32_t xInputGamePad) const {
	return gamePadInputs_[gamePadNum]->IsPressButton(xInputGamePad);
}

bool Input::IsTriggerButton(uint32_t gamePadNum, uint32_t xInputGamePad) const {
	return gamePadInputs_[gamePadNum]->IsTriggerButton(xInputGamePad);
}

bool Input::IsReleaseButton(uint32_t gamePadNum, uint32_t xInputGamePad) const {
	return gamePadInputs_[gamePadNum]->IsReleaseButton(xInputGamePad);
}

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}
