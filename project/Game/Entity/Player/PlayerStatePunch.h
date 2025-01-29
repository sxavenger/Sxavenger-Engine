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
// PlayerStatePunch class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerStatePunch
	: public BasePlayerState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerStatePunch(Player* player) : BasePlayerState(player) {};
	~PlayerStatePunch() = default;

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
	//* nullptrの場合はRootに戻る

	std::unique_ptr<PlayerAttackCollider> attackCollider_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ActionGamepad();

	void UpdateAnimation();

};
