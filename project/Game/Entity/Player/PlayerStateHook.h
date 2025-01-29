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

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateHook class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStateHook
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStateHook(Player* player) : BasePlayerState(player) {};
	~PlayerStateHook() = default;

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

	//* next state *//

	std::optional<std::unique_ptr<BasePlayerState>> nextAttackState_ = std::nullopt; //!< 次の攻撃状態

	//* collider *//

	std::unique_ptr<PlayerAttackCollider> attackCollider_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ActionGamepad();

	void UpdateAnimation();

};
