#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* state
#include "BasePlayerState.h"

//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>
#include <Engine/Module/Collider/Collider.h>

//* game
#include "PlayerAttackCollider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStateKick
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStateKick(Player* player) : BasePlayerState(player) {};
	~PlayerStateKick() = default;

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

	//* collider *//

	std::unique_ptr<PlayerAttackCollider> attackCollider_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateAnimation();

};

