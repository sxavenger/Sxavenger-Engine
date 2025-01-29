#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BasePlayerState.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStateRolling
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStateRolling(Player* player) : BasePlayerState(player) {};
	~PlayerStateRolling() = default;

	void Init() override;

	void Term() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* time *//

	TimePointf<TimeUnit::second> time_;
	TimePointf<TimeUnit::second> duration_;

	//* parameter *//

	float speed_ = 5.0f;


};
