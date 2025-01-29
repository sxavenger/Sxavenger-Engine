#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BasePlayerState.h"

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRoot class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStateRoot
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStateRoot(Player* player) : BasePlayerState(player) {}
	~PlayerStateRoot() = default;

	void Init() override;

	void Term() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* paraemter *//

	float speed_    = 1.0f;
	float runSpeed_ = 4.0f;

	Vector2f rotationDelta_ = { 0.02f, 0.01f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RotateCameraGamepad();
	void MoveGamepad();
	void ActionGamepad();

	void MoveKeyboard();

	// HACK: 移動を扱う
};
