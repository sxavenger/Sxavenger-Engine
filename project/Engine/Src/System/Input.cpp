#include "Input.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Logger.h"

////////////////////////////////////////////////////////////////////////////////////////////
// KeyboardInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void KeyboardInput::Init(IDirectInput8* dInput, const HWND& hWnd) {
	
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
		hWnd,
		flags
	);
	Assert(SUCCEEDED(hr));
	
}

void KeyboardInput::Update() {

	// 前frameのkey状態の保存
	keys_.second = keys_.first;

	// キーボード情報の取得開始
	keyboardDevice_->Acquire();

	// キーボードの入力状態を取得
	keyboardDevice_->GetDeviceState(sizeof(keys_.first), keys_.first.data());

}

bool KeyboardInput::IsPressKey(uint8_t dik) const {
	return keys_.first[dik];
}

bool KeyboardInput::IsTriggerKey(uint8_t dik) const {
	return keys_.first[dik] && !keys_.second[dik];
}

bool KeyboardInput::IsReleaseKey(uint8_t dik) const {
	return !keys_.first[dik] && keys_.second[dik];
}

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MouseInput::Init(IDirectInput8* dInput, const HWND& hWnd) {

	// デバイスの生成
	auto hr = dInput->CreateDevice(
		GUID_SysMouse, &mouseDevice_, NULL
	);
	Assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(
		&c_dfDIMouse // 標準形式
	);
	Assert(SUCCEEDED(hr));

	// flagの設定
	DWORD flags = 0;
	flags |= DISCL_FOREGROUND;
	flags |= DISCL_NONEXCLUSIVE;

	/*
		DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
		DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
	*/

	// 排他制御レベルのセット
	hr = mouseDevice_->SetCooperativeLevel(
		hWnd,
		flags
	);
	Assert(SUCCEEDED(hr));

	// hwndの保存
	hWnd_ = &hWnd;

	// mouseの初期位置の設定
	CalculteMousePos();
}

void MouseInput::Update() {

	// 前frameのmouse状態の保存
	mouse_.second = mouse_.first;

	// マウス情報の取得開始
	mouseDevice_->Acquire();

	// マウスの入力状態を取得
	mouseDevice_->GetDeviceState(sizeof(mouse_.first), &mouse_.first);

	// マウス座標の更新
	CalculteMousePos();

}

const Vector2i& MouseInput::GetMousePos() const {
	return mousePos_;
}

Vector2i MouseInput::GetDeltaMousePos() const {
	return { mouse_.first.lX, mouse_.first.lY };
}

bool MouseInput::IsPressMouse(uint8_t buttonNum) const {
	Assert(buttonNum < 4); //!< DIMOUSESTATE構造体
	return mouse_.first.rgbButtons[buttonNum];
}

int32_t MouseInput::GetDeltaWheel() const {
	return mouse_.first.lZ / WHEEL_DELTA; //!< wheelの最大値でnormalize
}

bool MouseInput::IsWheelUp() const {
	return mouse_.first.lZ > 0;
}

bool MouseInput::IsWheelDown() const {
	return mouse_.first.lZ < 0;
}

void MouseInput::CalculteMousePos() {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(*hWnd_, &point);

	mousePos_ = { point.x, point.y };
}

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadInput class methods
////////////////////////////////////////////////////////////////////////////////////////////
void GamepadInput::Init(uint32_t gamepadNumber) {
	// numberの保存
	gamepadNumber_ = gamepadNumber;
}

void GamepadInput::Update() {

	// 1frame前のコントローラー状態の保存
	gamepad_.second = gamepad_.first;

	// 現在のコントローラー状態の取得
	auto dr = XInputGetState(gamepadNumber_, &gamepad_.first);

	isConnect_ = (dr == ERROR_SUCCESS); //!< 接続されてるかの確認
	// [retval] true: 接続されてる, false: 接続されてない

}

bool GamepadInput::IsPressButton(uint32_t xInputGamepad) const {
	return gamepad_.first.Gamepad.wButtons & xInputGamepad;
}

bool GamepadInput::IsTriggerButton(uint32_t xInputGamepad) const {
	return (gamepad_.first.Gamepad.wButtons & xInputGamepad) && !(gamepad_.second.Gamepad.wButtons & xInputGamepad);
}

bool GamepadInput::IsReleaseButton(uint32_t xInputGamepad) const {
	return !(gamepad_.first.Gamepad.wButtons & xInputGamepad) && (gamepad_.second.Gamepad.wButtons & xInputGamepad);
}

Vector2i GamepadInput::GetLStick() const {
	return { gamepad_.first.Gamepad.sThumbLX, gamepad_.first.Gamepad.sThumbLY };
}

Vector2f GamepadInput::GetLStickNormalize() const {
	return { static_cast<float>(gamepad_.first.Gamepad.sThumbLX) / SHRT_MAX, static_cast<float>(gamepad_.first.Gamepad.sThumbLY) / SHRT_MAX };
}

Vector2i GamepadInput::GetRStick() const {
	return { gamepad_.first.Gamepad.sThumbRX, gamepad_.first.Gamepad.sThumbRY };
}

Vector2f GamepadInput::GetRStickNormalize() const {
	return { static_cast<float>(gamepad_.first.Gamepad.sThumbRX) / SHRT_MAX, static_cast<float>(gamepad_.first.Gamepad.sThumbRY) / SHRT_MAX };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Input class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Input::Init(const HINSTANCE& hInst, const HWND& hWnd) {

	CreateDirectInput(hInst);

	// keyboardInput
	keyboardInput_ = std::make_unique<KeyboardInput>();
	keyboardInput_->Init(directInput_.Get(), hWnd);

	// mouseInput
	mouseInput_ = std::make_unique<MouseInput>();
	mouseInput_->Init(directInput_.Get(), hWnd);

	// gamePadInput
	for (uint32_t i = 0; i < kGamepadNum_; ++i) {
		gamepadInputs_[i] = std::make_unique<GamepadInput>();
		gamepadInputs_[i]->Init(i);
	}


}

void Input::Update() {

	// keyboardInput
	keyboardInput_->Update();

	// mouseInput
	mouseInput_->Update();
	
	// gamePadInput
	for (auto& gamePad : gamepadInputs_) {
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

const Vector2i& Input::GetMousePos() const {
	return mouseInput_->GetMousePos();
}

Vector2i Input::GetDeltaMousePos() const {
	return mouseInput_->GetDeltaMousePos();
}

bool Input::IsPressMouse(uint8_t buttonNum) const {
	return mouseInput_->IsPressMouse(buttonNum);
}

int32_t Input::GetDeltaWheel() const {
	return mouseInput_->GetDeltaWheel();
}

bool Input::IsWheelUp() const {
	return mouseInput_->IsWheelUp();
}

bool Input::IsWheelDown() const {
	return mouseInput_->IsWheelDown();
}

bool Input::IsConnectGamepad(uint32_t gamepadNum) const {
	return gamepadInputs_[gamepadNum]->IsConnect();
}

bool Input::IsPressButton(uint32_t gamepadNum, uint32_t xInputGamepad) const {
	return gamepadInputs_[gamepadNum]->IsPressButton(xInputGamepad);
}

bool Input::IsTriggerButton(uint32_t gamepadNum, uint32_t xInputGamepad) const {
	return gamepadInputs_[gamepadNum]->IsTriggerButton(xInputGamepad);
}

bool Input::IsReleaseButton(uint32_t gamepadNum, uint32_t xInputGamepad) const {
	return gamepadInputs_[gamepadNum]->IsReleaseButton(xInputGamepad);
}

void Input::CreateDirectInput(const HINSTANCE& hInst) {

	auto hr = DirectInput8Create(
		hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr
	);

	Assert(SUCCEEDED(hr));
}
