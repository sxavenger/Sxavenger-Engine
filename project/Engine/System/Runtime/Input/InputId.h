#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include <dinput.h>
#include <Xinput.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// KeyId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class KeyId : uint8_t {

	//* 数字キー
	KEY_0 = DIK_0,
	KEY_1 = DIK_1,
	KEY_2 = DIK_2,
	KEY_3 = DIK_3,
	KEY_4 = DIK_4,
	KEY_5 = DIK_5,
	KEY_6 = DIK_6,
	KEY_7 = DIK_7,
	KEY_8 = DIK_8,
	KEY_9 = DIK_9,

	//* アルファベットキー
	KEY_A = DIK_A,
	KEY_B = DIK_B,
	KEY_C = DIK_C,
	KEY_D = DIK_D,
	KEY_E = DIK_E,
	KEY_F = DIK_F,
	KEY_G = DIK_G,
	KEY_H = DIK_H,
	KEY_I = DIK_I,
	KEY_J = DIK_J,
	KEY_K = DIK_K,
	KEY_L = DIK_L,
	KEY_M = DIK_M,
	KEY_N = DIK_N,
	KEY_O = DIK_O,
	KEY_P = DIK_P,
	KEY_Q = DIK_Q,
	KEY_R = DIK_R,
	KEY_S = DIK_S,
	KEY_T = DIK_T,
	KEY_U = DIK_U,
	KEY_V = DIK_V,
	KEY_W = DIK_W,
	KEY_X = DIK_X,
	KEY_Y = DIK_Y,
	KEY_Z = DIK_Z,

	//* ファンクションキー
	KEY_F1  = DIK_F1,
	KEY_F2  = DIK_F2,
	KEY_F3  = DIK_F3,
	KEY_F4  = DIK_F4,
	KEY_F5  = DIK_F5,
	KEY_F6  = DIK_F6,
	KEY_F7  = DIK_F7,
	KEY_F8  = DIK_F8,
	KEY_F9  = DIK_F9,
	KEY_F10 = DIK_F10,
	KEY_F11 = DIK_F11,
	KEY_F12 = DIK_F12,

	//* 矢印キー
	KEY_UP    = DIK_UP,
	KEY_DOWN  = DIK_DOWN,
	KEY_LEFT  = DIK_LEFT,
	KEY_RIGHT = DIK_RIGHT,

	//* 特殊キー
	KEY_ESCAPE    = DIK_ESCAPE,
	KEY_TAB       = DIK_TAB,
	KEY_CAPSLOCK  = DIK_CAPSLOCK,
	KEY_LSHIFT    = DIK_LSHIFT,
	KEY_RSHIFT    = DIK_RSHIFT,
	KEY_LCONTROL  = DIK_LCONTROL,
	KEY_RCONTROL  = DIK_RCONTROL,
	KEY_LALT      = DIK_LALT,
	KEY_RALT      = DIK_RALT,
	KEY_SPACE     = DIK_SPACE,
	KEY_ENTER     = DIK_RETURN,
	KEY_BACKSPACE = DIK_BACK,
	KEY_INSERT    = DIK_INSERT,
	KEY_DELETE    = DIK_DELETE,
	KEY_HOME      = DIK_HOME,
	KEY_END       = DIK_END,
	KEY_PAGEUP    = DIK_PGUP,
	KEY_PAGEDOWN  = DIK_PGDN,

	//* テンキー
	KEY_NUMPAD0     = DIK_NUMPAD0,
	KEY_NUMPAD1     = DIK_NUMPAD1,
	KEY_NUMPAD2     = DIK_NUMPAD2,
	KEY_NUMPAD3     = DIK_NUMPAD3,
	KEY_NUMPAD4     = DIK_NUMPAD4,
	KEY_NUMPAD5     = DIK_NUMPAD5,
	KEY_NUMPAD6     = DIK_NUMPAD6,
	KEY_NUMPAD7     = DIK_NUMPAD7,
	KEY_NUMPAD8     = DIK_NUMPAD8,
	KEY_NUMPAD9     = DIK_NUMPAD9,
	KEY_NUMPADENTER = DIK_NUMPADENTER,
	KEY_ADD         = DIK_ADD,
	KEY_SUBTRACT    = DIK_SUBTRACT,
	KEY_MULTIPLY    = DIK_MULTIPLY,
	KEY_DIVIDE      = DIK_DIVIDE,
	KEY_DECIMAL     = DIK_DECIMAL,

	//* その他のキー
	KEY_PRINTSCREEN = DIK_SYSRQ,
	KEY_SCROLLLOCK  = DIK_SCROLL,
	KEY_PAUSE       = DIK_PAUSE,
	KEY_SEMICOLON   = DIK_SEMICOLON,
	KEY_EQUALS      = DIK_EQUALS,
	KEY_COMMA       = DIK_COMMA,
	KEY_MINUS       = DIK_MINUS,
	KEY_PERIOD      = DIK_PERIOD,
	KEY_SLASH       = DIK_SLASH,
	KEY_BACKTICK    = DIK_GRAVE,
	KEY_LBRACKET    = DIK_LBRACKET,
	KEY_RBRACKET    = DIK_RBRACKET,
	KEY_BACKSLASH   = DIK_BACKSLASH,
	KEY_APOSTROPHE  = DIK_APOSTROPHE,

	//* メディアキー (一部環境依存)
	KEY_MEDIA_PLAY_PAUSE = DIK_MEDIASELECT,
	KEY_MEDIA_NEXT       = DIK_NEXTTRACK,
	KEY_MEDIA_PREV       = DIK_PREVTRACK,
	KEY_VOLUME_UP        = DIK_VOLUMEUP,
	KEY_VOLUME_DOWN      = DIK_VOLUMEDOWN,
	KEY_MUTE             = DIK_MUTE,
};

////////////////////////////////////////////////////////////////////////////////////////////
// MouseId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MouseId : uint8_t {
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,

	MOUSE_BUTTON4,
	MOUSE_BUTTON5,
	MOUSE_BUTTON6,
	MOUSE_BUTTON7,
	MOUSE_BUTTON8,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadButtonId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GamepadButtonId : uint32_t {
	//* Directional Pad (D-Pad)
	DPAD_UP    = XINPUT_GAMEPAD_DPAD_UP,
	DPAD_DOWN  = XINPUT_GAMEPAD_DPAD_DOWN,
	DPAD_LEFT  = XINPUT_GAMEPAD_DPAD_LEFT,
	DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,

	//* Special Buttonsk
	BUTTON_BACK  = XINPUT_GAMEPAD_BACK,
	BUTTON_START = XINPUT_GAMEPAD_START,

	//* Thumbstick Buttons
	BUTTON_LS = XINPUT_GAMEPAD_LEFT_THUMB,
	BUTTON_RS = XINPUT_GAMEPAD_RIGHT_THUMB,

	//* Shoulder Buttons
	BUTTON_LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
	BUTTON_RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,

	//* Face Buttons
	BUTTON_A = XINPUT_GAMEPAD_A,
	BUTTON_B = XINPUT_GAMEPAD_B,
	BUTTON_X = XINPUT_GAMEPAD_X,
	BUTTON_Y = XINPUT_GAMEPAD_Y,
};

template <> //!< magic_enumの範囲を調整
struct magic_enum::customize::enum_range<GamepadButtonId> {
	static constexpr auto min = XINPUT_GAMEPAD_DPAD_UP;
	static constexpr auto max = XINPUT_GAMEPAD_Y;
};

template<>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<GamepadButtonId>(GamepadButtonId value) noexcept {
	switch (value) {
		case GamepadButtonId::DPAD_UP:
		case GamepadButtonId::DPAD_DOWN:
		case GamepadButtonId::DPAD_LEFT:
		case GamepadButtonId::DPAD_RIGHT:
		case GamepadButtonId::BUTTON_BACK:
		case GamepadButtonId::BUTTON_START:
		case GamepadButtonId::BUTTON_LS:
		case GamepadButtonId::BUTTON_RS:
		case GamepadButtonId::BUTTON_LB:
		case GamepadButtonId::BUTTON_RB:
		case GamepadButtonId::BUTTON_A:
		case GamepadButtonId::BUTTON_B:
		case GamepadButtonId::BUTTON_X:
		case GamepadButtonId::BUTTON_Y:
			return default_tag;

		default:
			return invalid_tag;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadTriggerId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GamepadTriggerId : uint8_t {
	TRIGGER_LEFT,
	TRIGGER_RIGHT,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadStickId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GamepadStickId : uint8_t {
	STICK_LEFT,
	STICK_RIGHT,
};
