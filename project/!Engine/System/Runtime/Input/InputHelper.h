#pragma once


//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include "Input.h"

////////////////////////////////////////////////////////////////////////////////////////////
// InputHelper class
////////////////////////////////////////////////////////////////////////////////////////////
class InputHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* direction methods *//

	//! @brief keyboardの方向取得
	//! @param[in] input キーボード入力管理クラス
	//! @param[in] id { up, down, left, right } のKeyId配列
	//! @return 方向ベクトル
	static Vector2f GetDirection(const KeyboardInput* input, const std::array<KeyId, 4>& id);

	//! @brief gamepad-stickの方向取得
	//! @param input ゲームパッド入力管理クラス
	//! @param id スティックID
	//! @return 方向ベクトル
	static Vector2f GetDirection(const GamepadInput* input, GamepadStickId id, float deadzone = 0.0f);

	//! @brief 方向ベクトルの正規化取得
	//! @param direction 方向ベクトル
	//! @return input用に正規化された方向ベクトル
	static Vector2f GetDirectionNormalize(const Vector2f& direction);

private:
};
