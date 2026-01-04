#include "Input.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

//* externals
#include <imgui.h>

//* c++
#include <bitset>

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

	//* DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
	//* DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
}

void KeyboardInput::AsyncUpdate() {

	InputData& input = inputs_[static_cast<uint8_t>(InputType::Async_Stack)];

	input.isEnableAcquire = SetCooperativeLevel(System::GetForcusWindow());

	if (!input.isEnableAcquire) {
		input.Clear();
		return;
	}

	// キーボード情報の取得開始
	keyboardDevice_->Acquire();

	KeyInput current = {};

	// キーボードの入力状態を取得
	keyboardDevice_->GetDeviceState(sizeof(current), current.data());

	// 現在の状態にスタックさせる
	for (size_t i = 0; i < kKeyCount; ++i) {
		input.keys[i] |= current[i];
	}

}

void KeyboardInput::UpdateInputState() {
	// Main_Previousの設定
	inputs_[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Current)];

	// Main_Currentの設定
	inputs_[static_cast<uint8_t>(InputType::Main_Current)] = inputs_[static_cast<uint8_t>(InputType::Async_Stack)];

	// Async_Stackのクリア
	inputs_[static_cast<uint8_t>(InputType::Async_Stack)].Clear();
	
}

bool KeyboardInput::IsPress(KeyId id) const {
	if (!IsEnableAcquire(InputType::Main_Current)) {
		return false;
	}

	return GetKey(id, InputType::Main_Current);
}

bool KeyboardInput::IsTrigger(KeyId id) const {
	if (!IsEnableAcquire(InputType::Main_Current)) {
		return false;
	}

	return GetKey(id, InputType::Main_Current) && !GetKey(id, InputType::Main_Previous);
}

bool KeyboardInput::IsRelease(KeyId id) const {
	if (!IsEnableAcquire(InputType::Main_Current)) {
		return false;
	}

	return !GetKey(id, InputType::Main_Current) && GetKey(id, InputType::Main_Previous);
}

bool KeyboardInput::IsPressAny() const {
	if (!IsEnableAcquire(InputType::Main_Current)) {
		return false;
	}

	return std::any_of(
		inputs_[static_cast<uint8_t>(InputType::Main_Current)].keys.begin(),
		inputs_[static_cast<uint8_t>(InputType::Main_Current)].keys.end(),
		[](BYTE key) { return key; }
	);
}

Vector2i KeyboardInput::GetDirection(const std::array<KeyId, 4>& keys) const {
	if (!IsEnableAcquire(InputType::Main_Current)) {
		return {};
	}

	Vector2i direction = {};

	if (IsPress(keys[0])) { // Up
		direction.y += 1;
	}

	if (IsPress(keys[1])) { // Down
		direction.y -= 1;
	}

	if (IsPress(keys[2])) { // Left
		direction.x -= 1;
	}

	if (IsPress(keys[3])) { // Right
		direction.x += 1;
	}

	return direction;
}

void KeyboardInput::SystemDebugGui() {
	
	bool isEnableAcquire = IsEnableAcquire(InputType::Main_Current);
	ImGui::Text("Acquire State: %s", isEnableAcquire ? "Enable" : "Disable");

	ImGui::BeginDisabled(!isEnableAcquire);

	ImGui::BeginTable("## keyboard input state table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);
	ImGui::TableSetupColumn("KeyId");
	ImGui::TableSetupColumn("Input State");
	ImGui::TableHeadersRow();

	for (const auto& [id, name] : magic_enum::enum_entries<KeyId>()) {
		std::bitset<2> state = {};
		state[static_cast<uint8_t>(InputType::Main_Current)]  = inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetKey(id);
		state[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetKey(id);

		if (state.none()) {
			//!< 押されていない場合は表示しない
			continue;
		}

		ImGui::TableNextRow();

		{ //!< KeyId
			ImGui::TableNextColumn();
			ImGui::Text(name.data());
			ImGui::SameLine();
			ImGui::Dummy({ 16.0f, 0.0f });
		}

		{ //!< Input State
			ImGui::TableNextColumn();
			switch (state.to_ulong()) {
				case 0b01: //!< Trigger
					ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "Trigger");
					break;

				case 0b11: //!< Hold
					ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "Hold");
					break;

				case 0b10: //!< Release
					ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Release");
					break;
			}
		}
	}

	ImGui::EndTable();

	ImGui::EndDisabled();
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
		
		currentHwnd_ = hwnd;
	}

	return true;
}

bool KeyboardInput::IsEnableAcquire(InputType type) const {
	return inputs_[static_cast<uint8_t>(type)].IsEnableAcquire();
}

bool KeyboardInput::GetKey(KeyId id, InputType type) const {
	return inputs_[static_cast<uint8_t>(type)].GetKey(id);
}

////////////////////////////////////////////////////////////////////////////////////////////
// MouseInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MouseInput::Init(IDirectInput8* dInput) {

	// デバイスの生成
	auto hr = dInput->CreateDevice(
		GUID_SysMouse, &mouseDevice_, NULL
	);
	StreamLogger::AssertA(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(
		&c_dfDIMouse2 // 標準形式
	);
	StreamLogger::AssertA(SUCCEEDED(hr));

	flags_ |= DISCL_FOREGROUND;
	flags_ |= DISCL_NONEXCLUSIVE;

	//* DISCL_FOREGROUND   -> 画面が手前にある場合のみ入力を受け付け
	//* DISCL_NONEXCLUSIVE -> デバイスをこのアプリで占有しない
	
}

void MouseInput::AsyncUpdate() {

	InputData& input = inputs_[static_cast<uint8_t>(InputType::Async_Stack)];

	input.isEnableAcquire = SetCooperativeLevel(System::GetForcusWindow());

	if (!input.isEnableAcquire) {
		input.Clear();
		return;
	}

	DIMOUSESTATE2 current = {};

	// マウス情報の取得開始
	mouseDevice_->Acquire();

	// マウスの入力状態を取得
	mouseDevice_->GetDeviceState(sizeof(current), &current);

	// 現在の状態にスタックさせる
	for (size_t i = 0; i < 8; ++i) {
		input.mouse.rgbButtons[i] |= current.rgbButtons[i];
	}

	input.mouse.lX += current.lX;
	input.mouse.lY += current.lY;
	input.mouse.lZ = std::clamp<int32_t>(input.mouse.lZ + current.lZ, -WHEEL_DELTA, WHEEL_DELTA);
	// FIXME: ホイールの加算値が期待している数値にならない.
}

void MouseInput::UpdateInputState() {
	// Main_Previousの設定
	inputs_[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Current)];

	// Main_Currentの設定
	inputs_[static_cast<uint8_t>(InputType::Main_Current)] = inputs_[static_cast<uint8_t>(InputType::Async_Stack)];

	// Async_Stackのクリア
	inputs_[static_cast<uint8_t>(InputType::Async_Stack)].Clear();

	ShowCousor(true);
}

Vector2i MouseInput::GetPosition() const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return {};
	}

	POINT point = {};
	GetCursorPos(&point);
	ScreenToClient(currentHwnd_, &point);

	return { point.x, point.y };
}

Vector2i MouseInput::GetPosition(const DirectXWindowContext* window) const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
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
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return {};
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetDeltaPosition();
}

void MouseInput::SetPosition(const Vector2i& position) const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return;
	}

	if (currentHwnd_ == nullptr) {
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
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id);
}

bool MouseInput::IsTrigger(MouseId id) const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id)
		&& !inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);
}

bool MouseInput::IsRelease(MouseId id) const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return false;
	}

	return !inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id)
		&& inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);
}

int32_t MouseInput::GetDeltaWheel() const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return {};
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetDeltaWheel();
}

float MouseInput::GetDeltaWheelNormalized() const {
	return static_cast<float>(GetDeltaWheel()) / static_cast<float>(WHEEL_DELTA);
}

bool MouseInput::IsWheel() const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsWheel();
}

bool MouseInput::IsWheelUp() const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsWheelUp();
}

bool MouseInput::IsWheelDown() const {
	if (!inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsWheelDown();
}

void MouseInput::SystemDebugGui() {
	
	bool isEnableAcquire = inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsEnableAcquire();
	ImGui::Text("Acquire State: %s", isEnableAcquire ? "Enable" : "Disable");

	ImGui::BeginDisabled(!isEnableAcquire);

	ImGui::Text(std::format("Position:       {}", GetPosition()).c_str());
	ImGui::Text(std::format("Delta Position: {}", GetDeltaPosition()).c_str());
	ImGui::Text(std::format("Delta Wheel:    {}", GetDeltaWheel()).c_str());

	ImGui::BeginTable("## mouse input state table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);
	ImGui::TableSetupColumn("MouseId");
	ImGui::TableSetupColumn("Input State");
	ImGui::TableHeadersRow();

	for (const auto& [id, name] : magic_enum::enum_entries<MouseId>()) {
		std::bitset<2> state = {};
		state[static_cast<uint8_t>(InputType::Main_Current)]  = inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id);
		state[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);

		if (state.none()) {
			//!< 押されていない場合は表示しない
			continue;
		}

		ImGui::TableNextRow();

		{ //!< MouseId
			ImGui::TableNextColumn();
			ImGui::Text(name.data());
			ImGui::SameLine();
			ImGui::Dummy({ 16.0f, 0.0f });
		}

		{ //!< Input State
			ImGui::TableNextColumn();
			switch (state.to_ulong()) {
				case 0b01: //!< Trigger
					ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "Trigger");
					break;

				case 0b11: //!< Hold
					ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "Hold");
					break;

				case 0b10: //!< Release
					ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Release");
					break;
			}
		}
	}

	ImGui::EndTable();

	ImGui::EndDisabled();
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
// GamepadInput InputData structure methods
////////////////////////////////////////////////////////////////////////////////////////////


bool GamepadInput::InputData::GetButton(GamepadTriggerId id) const {
	switch (id) {
		case GamepadTriggerId::TRIGGER_LEFT:
			return xinput.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		case GamepadTriggerId::TRIGGER_RIGHT:
			return xinput.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		default:
			return false;
	}
}

Vector2i GamepadInput::InputData::GetStick(GamepadStickId id) const {
	switch (id) {
		case GamepadStickId::STICK_LEFT:
			return { xinput.Gamepad.sThumbLX, xinput.Gamepad.sThumbLY };

		case GamepadStickId::STICK_RIGHT:
			return { xinput.Gamepad.sThumbRX, xinput.Gamepad.sThumbRY };

		default:
			return {};
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadInput class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GamepadInput::Init(uint8_t number) {
	StreamLogger::AssertA(number < XUSER_MAX_COUNT, "Gamepad number is out of range.");

	// numberの設定
	number_ = number;
}

void GamepadInput::AsyncUpdate() {

	InputData& input = inputs_[static_cast<uint8_t>(InputType::Async_Stack)];

	XINPUT_STATE current = {};

	// コントローラーの入力状態を取得
	auto dr = XInputGetState(number_, &current);

	input.isConnect = (dr == ERROR_SUCCESS);

	if (!input.isConnect) {
		//!< 接続されてない場合, 状態をクリア
		input.Clear();
		return;
	}

	if (current.dwPacketNumber == input.xinput.dwPacketNumber) {
		return; //!< 前回と同じ状態
	}

	// 現在の状態にスタックさせる
	input.xinput.Gamepad.wButtons      |= current.Gamepad.wButtons;
	input.xinput.Gamepad.bLeftTrigger  = std::max(input.xinput.Gamepad.bLeftTrigger,  current.Gamepad.bLeftTrigger);
	input.xinput.Gamepad.bRightTrigger = std::max(input.xinput.Gamepad.bRightTrigger, current.Gamepad.bRightTrigger);

	input.xinput.Gamepad.sThumbLX = current.Gamepad.sThumbLX;
	input.xinput.Gamepad.sThumbLY = current.Gamepad.sThumbLY;
	input.xinput.Gamepad.sThumbRX = current.Gamepad.sThumbRX;
	input.xinput.Gamepad.sThumbRY = current.Gamepad.sThumbRY;


}

void GamepadInput::UpdateInputState() {
	// Main_Previousの設定
	inputs_[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Current)];

	// Main_Currentの設定
	inputs_[static_cast<uint8_t>(InputType::Main_Current)] = inputs_[static_cast<uint8_t>(InputType::Async_Stack)];

	// Async_Stackのクリア
	inputs_[static_cast<uint8_t>(InputType::Async_Stack)].Clear();
}

bool GamepadInput::IsConnect() const {
	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsConnect();
}

bool GamepadInput::IsPress(GamepadButtonId id) const {
	if (!IsConnect()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id);
}

bool GamepadInput::IsPress(GamepadTriggerId id) const {
	if (!IsConnect()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id);
}

bool GamepadInput::IsTrigger(GamepadButtonId id) const {
	if (!IsConnect()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id)
		&& !inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);
}

bool GamepadInput::IsTrigger(GamepadTriggerId id) const {
	if (!IsConnect()) {
		return false;
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id)
		&& !inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);
}

bool GamepadInput::IsRelease(GamepadButtonId id) const {
	if (!IsConnect()) {
		return false;
	}

	return !inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id)
		&& inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);
}

bool GamepadInput::IsRelease(GamepadTriggerId id) const {
	if (!IsConnect()) {
		return false;
	}

	return !inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id)
		&& inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);
}

Vector2i GamepadInput::GetStick(GamepadStickId id) const {
	if (!IsConnect()) {
		return {};
	}

	return inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetStick(id);
}

Vector2f GamepadInput::GetStickNormalized(GamepadStickId id) const {
	return static_cast<Vector2f>(GetStick(id)) / static_cast<float>(SHRT_MAX);
}

void GamepadInput::SystemDebugGui() {
	ImGui::PushID(number_);
	ImGui::Text(std::format("controller number: {}", number_).c_str());

	bool isConnect = inputs_[static_cast<uint8_t>(InputType::Main_Current)].IsConnect();
	ImGui::Text("Connection State: %s", isConnect ? "Connected" : "Disconnected");

	ImGui::BeginDisabled(!isConnect);

	ImGui::Text(std::format("Left Stick:  {}", GetStick(GamepadStickId::STICK_LEFT)).c_str());
	ImGui::Text(std::format("Right Stick: {}", GetStick(GamepadStickId::STICK_RIGHT)).c_str());

	ImGui::BeginTable("## gamepad input state table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);
	ImGui::TableSetupColumn("ButtonId");
	ImGui::TableSetupColumn("Input State");
	ImGui::TableHeadersRow();

	for (const auto& [id, name] : magic_enum::enum_entries<GamepadButtonId>()) {
		std::bitset<2> state = {};
		state[static_cast<uint8_t>(InputType::Main_Current)]  = inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id);
		state[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);

		if (state.none()) {
			//!< 押されていない場合は表示しない
			continue;
		}

		ImGui::TableNextRow();

		{ //!< ButtonId
			ImGui::TableNextColumn();
			ImGui::Text(name.data());
			ImGui::SameLine();
			ImGui::Dummy({ 16.0f, 0.0f });
		}

		{ //!< Input State
			ImGui::TableNextColumn();
			switch (state.to_ulong()) {
				case 0b01: //!< Trigger
					ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "Trigger");
					break;

				case 0b11: //!< Hold
					ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "Hold");
					break;

				case 0b10: //!< Release
					ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Release");
					break;
			}
		}
	}

	for (const auto& [id, name] : magic_enum::enum_entries<GamepadTriggerId>()) {
		std::bitset<2> state = {};
		state[static_cast<uint8_t>(InputType::Main_Current)]  = inputs_[static_cast<uint8_t>(InputType::Main_Current)].GetButton(id);
		state[static_cast<uint8_t>(InputType::Main_Previous)] = inputs_[static_cast<uint8_t>(InputType::Main_Previous)].GetButton(id);


		if (state.none()) {
			//!< 押されていない場合は表示しない
			continue;
		}

		ImGui::TableNextRow();

		{ //!< ButtonId
			ImGui::TableNextColumn();
			ImGui::Text(name.data());
			ImGui::SameLine();
			ImGui::Dummy({ 16.0f, 0.0f });
		}

		{ //!< Input State
			ImGui::TableNextColumn();
			switch (state.to_ulong()) {
				case 0b01: //!< Trigger
					ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "Trigger");
					break;

				case 0b11: //!< Hold
					ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "Hold");
					break;

				case 0b10: //!< Release
					ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Release");
					break;
			}
		}
	}

	ImGui::EndTable();

	ImGui::EndDisabled();

	ImGui::PopID();
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

	keyboard_ = std::make_unique<KeyboardInput>(directInput_.Get());
	mouse_    = std::make_unique<MouseInput>(directInput_.Get());

	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i) {
		gamepads_[i] = std::make_unique<GamepadInput>(i);
	}

	//* thread *//

	thread_ = std::thread([this]() {
		StreamLogger::EngineThreadLog("[Input] begin input thread.");

		while (!isTerminate_) {
			{
				std::lock_guard<std::mutex> lock(mutex_);

				keyboard_->AsyncUpdate();
				mouse_->AsyncUpdate();

				for (auto& gamepad : gamepads_) {
					gamepad->AsyncUpdate();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		StreamLogger::EngineThreadLog("[Input] end input thread.");
	});
}

void Input::Shutdown() {
	isTerminate_ = true;

	if (thread_.joinable()) {
		thread_.join();
	}
}

void Input::Update() {
	std::lock_guard<std::mutex> lock(mutex_);

	keyboard_->UpdateInputState();
	mouse_->UpdateInputState();

	for (auto& gamepad : gamepads_) {
		gamepad->UpdateInputState();
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

void Input::SystemDebugGui() {
	if (ImGui::BeginMenu("keyboard")) {
		ImGui::SeparatorText("keyboard input state");
		keyboard_->SystemDebugGui();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("mouse")) {
		ImGui::SeparatorText("mouse input state");
		mouse_->SystemDebugGui();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("gamepad")) {
		ImGui::SeparatorText("gamepad input state");

		for (auto& gamepad : gamepads_) {
			gamepad->SystemDebugGui();
			ImGui::Separator();
		}
		ImGui::EndMenu();
	}
}
